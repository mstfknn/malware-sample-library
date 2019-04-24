#include "core\ThroughTunnel.h"
#include "core\socket.h"
#include "core\debug.h"

ThroughTunnel::ThroughTunnel( int _portIn, const char* _ipOut, int _portOut ) : portIn(_portIn), portOut(_portOut)
{
	Str::Copy( ipOut, sizeof(ipOut), _ipOut );
}

ThroughTunnel::~ThroughTunnel()
{
}

static DWORD WINAPI ThroughTunnelThread( void* data )
{
	ThroughTunnel* through = (ThroughTunnel*)data;
	through->Start();
	return 0;
}


bool ThroughTunnel::Start()
{
	Loop();
	return true;
}

bool ThroughTunnel::StartAsync()
{
	return RunThread( ThroughTunnelThread, this );
}

int ThroughTunnel::Connected( int sc )
{
	return 0;
}

void ThroughTunnel::Loop()
{
	DbgMsg( "Старт ThroughTunnel, порт: %d", portIn );
	int sc = Socket::CreateListen(portIn);
	if( !sc ) return;
	bool stop = false;
	while( !stop )
	{
		int scIn = Socket::Accept(sc);
		if( scIn == SOCKET_ERROR ) break;
		int scOut = Connected(scIn);
		if( scOut <= 0 ) scOut = Socket::ConnectIP( ipOut, portOut );
		if( scOut > 0 )
			Socket::StartTunnel( scIn, scOut );
		else
			Socket::Close(scIn);
	}
	DbgMsg( "Остановка ThroughTunnel" );
}
