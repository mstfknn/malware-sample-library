#include "core\HttpProxy.h"
#include "core\socket.h"
#include "core\misc.h"
#include "core\debug.h"

HttpProxy::HttpProxy( int _port ) : port(_port)
{
}

HttpProxy::~HttpProxy()
{
}

static DWORD WINAPI HttpProxyThread( void* data )
{
	HttpProxy* proxy = (HttpProxy*)data;
	proxy->Start();
	return 0;
}


bool HttpProxy::Start()
{
	Loop();
	return true;
}

bool HttpProxy::StartAsync()
{
	return RunThread( HttpProxyThread, this );
}

static DWORD WINAPI HttpRequestThread( void* data )
{
	Mem::Data buf;
	int sc = (int)data;
	char host[64];
	char end[3]; end[0] = '\r'; end[1] = '\n'; end[2] = 0;
	Str::Copy( host, _CS_("Host:") );
	for(;;)
	{
		int res = Socket::Read( sc, buf, -1, 1000 );
		if( !res ) break;
		int p = buf.IndexOf( host, 5 );
		if( p > 0 )
		{
			p += 5;
			int p2 = buf.IndexOf( p, end, 2 ); //ищем конец строки
			if( p2 > 0 ) //имя хоста считано
			{
				while( buf.p_char()[p] == ' ' ) p++;
				Str::Copy( host, sizeof(host), buf.p_char() + p, p2 - p );
				DbgMsg( "Прокси коннект с %s", host );
				int sc2 = Socket::ConnectHost(host);
				if( sc2 > 0 ) 
				{
					//убираем в 1-й строке имя домена
					int p3 = buf.IndexOf( _CS_("http://"), 7 );
					if( p3 > 0 )
					{
						char slash[1]; slash[0] = '/';
						int p4 = buf.IndexOf( p3 + 7, slash, 1 );
						if( p4 > 0 )
						{
							buf.Remove( p3, p4 - p3 );
						}
					}
					//buf.p_char()[buf.Len()] = 0;
					//DbgMsg(buf.p_char());
					Socket::Write( sc2, buf.Ptr(), buf.Len() );
					Socket::LoopTunnel( sc, sc2 );
					sc = 0;
				}
				break;
			}
		}
		if( res == 2 ) break;
	}
	Socket::Close(sc);
	return 0;
}

void HttpProxy::Loop()
{
	DbgMsg( "Старт HttpProxy, порт: %d", port );
	int sc = Socket::CreateListen(port);
	if( !sc ) return;
	bool stop = false;
	while( !stop )
	{
		int sc2 = Socket::Accept(sc);
		if( !sc2 ) break;
		RunThread( HttpRequestThread, (void*)sc2 );
	}
	DbgMsg( "Остановка HttpProxy" );
}
