#include "core\socket.h"
#include "core\debug.h"

namespace Socket
{

bool Init()
{
	WSADATA wsa; 
	if( API(WS2_32, WSAStartup)( MAKEWORD(2,2), &wsa ) ) return false;
	return true;
}

void Release()
{
	API(WS2_32, WSACleanup)();
}

char* HostToIP( const char* host, char* ip )
{
	if( host == 0 || ip == 0 ) return 0;
	hostent* h = API(WS2_32, gethostbyname)(host);
	if( !h ) return 0;
	in_addr addr;
	addr.S_un.S_addr = *((ULONG*)h->h_addr_list[0]);
	const char* s = API(WS2_32, inet_ntoa)(addr);
	if( !s ) return 0;
	Str::Copy( ip, s );
	return ip;
}

int ConnectIP( const char* ip, int port )
{
	int sc = API(WS2_32, socket)( AF_INET, SOCK_STREAM, 0 );
	if( sc == INVALID_SOCKET ) return -2; //не создался сокет 
	sockaddr_in addr;
	Mem::Zero(addr);
	addr.sin_family = AF_INET;
	addr.sin_port = API(WS2_32, htons)(port);
	addr.sin_addr.s_addr = API(WS2_32, inet_addr)(ip);
	if( API(WS2_32, connect)( sc, (sockaddr*) &addr, sizeof(addr) ) != 0 )
	{
		API(WS2_32, closesocket)(sc);
		return -3; //не установилось соединение
	}
	return sc;
}

int ConnectHost( const char* host, int port )
{
	char ip[32];
	if( HostToIP( host, ip ) )
	{
		return ConnectIP( ip, port );
	}
	return -1;
}

int Write( int sc, const void* data, int c_data )
{
	if( data == 0 || c_data < 0 ) return SOCKET_ERROR;
	int res = API(WS2_32, send)( sc, (const char*)data, c_data, 0 );
	return res;
}

int Read( int sc, void* data, int sz_data, int wait )
{
	if( data == 0 || sz_data < 0 ) return SOCKET_ERROR;
	fd_set fdsr;
	fdsr.fd_count = 1;
	fdsr.fd_array[0] = sc;
	timeval waitEvent;
	if( wait )
	{
		waitEvent.tv_sec = 0;
		waitEvent.tv_usec = wait * 1000; //в микросекундах
	}
	else //вечное ожижание
	{
		waitEvent.tv_sec = INT_MAX;
		waitEvent.tv_usec = 0;
	}
	int events = API(WS2_32, select)( 0, &fdsr, NULL, NULL, &waitEvent );
	if( events == 0 ) return -2; //ничего не пришло, нужно ждать
	int c = API(WS2_32, recv)( sc, (char*)data, sz_data, 0 );
	return c;
}

int Read( int sc, Mem::Data& data, int max_read, int wait )
{
	const int step = 1024; //количество считываемых данных за один раз по умолчанию
	int ret = 0;
	int readed = 0;
	for(;;)
	{
		int count = step; //сколько максимум можно прочесть
		if( max_read > 0 ) 
			count = max_read - readed;
		if( !data.MakeEnough(count) ) //недостаточно памяти
		{
			ret = 0;
			break;
		}
		int c = Read( sc, data.PtrWrite(), count, wait );
		if( c == 0 ) //чтение окончено
		{
			ret = 2;
			break;
		}
		if( c == SOCKET_ERROR ) //ошибка при чтении
		{
			ret = 0;
			break;
		}
		if( c == -2 ) //вышло время ожидания
		{
			ret = 3;
			break;
		}
		//считано какое-то количество байт
		data.AddLen(c);
		readed += c;
		if( max_read < 0 || (max_read > 0 && readed >= max_read) ) //прочли указанное количество байт
		{
			ret = 1;
			break;
		}
	}
	return ret;
}

void Close( int sc )
{
	API(WS2_32, closesocket)(sc);
}

int CreateListen( int port )
{
	SOCKET sc = API(WS2_32, socket)( AF_INET, SOCK_STREAM, 0 );
	if( sc != INVALID_SOCKET )
	{
		sockaddr_in sa;
		Mem::Zero(sa);
		sa.sin_family = AF_INET;
		sa.sin_addr.s_addr = INADDR_ANY;
		sa.sin_port = API(WS2_32, htons)(port);
		int sz = sizeof(sa);
		if( !API(WS2_32, bind)( sc, (sockaddr*)&sa, sz ) && !API(WS2_32, listen)( sc, 100 ) )
		{
		}
		else
		{
			Close(sc);
			sc = 0;
		}
    }
	else
		sc = 0;
	return sc;
}

int Accept( int sc )
{
	int newSc = API(WS2_32, accept)( sc, nullptr, nullptr );
	if( newSc < 0 ) newSc = 0;
	return newSc;
}

struct StruTunnel
{
	int sc1, sc2;
};

static DWORD WINAPI TunnelSocketThread( void* data )
{
	StruTunnel* st = (StruTunnel*)data;
	int s1 = st->sc1;
	int s2 = st->sc2;
	Mem::Free(data);
	LoopTunnel( s1, s2 );
	return 0;
}

void LoopTunnel( int s1, int s2 )
{
    DbgMsg("Старт туннеля %d<->%d", s1, s2 );
	bool stop = false;
    while( !stop )
    {
        fd_set fdsr;
        fdsr.fd_count = 2;
        fdsr.fd_array[0] = s1;
        fdsr.fd_array[1] = s2;

        if( API(WS2_32, select)( 0, &fdsr, NULL, NULL, NULL ) < 1 )
            break;
        for( uint i = 0; i < fdsr.fd_count; i++ )
        {
            char buf[1024];
            SOCKET hread = fdsr.fd_array[i];
            SOCKET hwrite = (hread == s1) ? s2 : s1;
            int dw = API(WS2_32, recv)( hread, buf, sizeof(buf), 0 );
			if( dw <= 0 ) 
			{
				stop = true;
				break;
			}
			if( API(WS2_32, send)( hwrite, buf, dw, 0 ) != dw )
			{
				stop = true;
				break;
			}
        }
    }
    DbgMsg( "Остановка туннеля %d<->%d", s1, s2 );
	Socket::Close(s1);
    Socket::Close(s2);
}


bool StartTunnel( int s1, int s2 )
{
	StruTunnel* data = (StruTunnel*)Mem::Alloc( sizeof(StruTunnel) );
	data->sc1 = s1;
	data->sc2 = s2;
	return RunThread( TunnelSocketThread, data );
}

}
