#include "core\proxy.h"
#include "core\socket.h"
#include "core\debug.h"

namespace Proxy
{

Connector::Connector()
{
	c_addr = 0;
	curr = -1;
	CriticalSection::Init(lock);
}

Connector::~Connector()
{
}

void Connector::Add( const Info* _addr, int count )
{
	CriticalSection cs(lock);
	for( int i = 0; i < count; i++ )
	{
		if( c_addr >= MAX_PROXY ) break;
		int j = 0;
		for( ; j < c_addr; j++ )
			if( Str::Cmp( addr[j].ipPort.ip, _addr[i].ipPort.ip ) == 0 && addr[j].ipPort.port == _addr[i].ipPort.port )
				break;
		if( j < c_addr ) //такой уже есть
		{
			if( addr[j].type < _addr[i].type )
				addr[j].type = _addr[i].type;
			if( _addr->authentication[0] ) //есть строка аунтефикации
				Str::Copy( addr[j].authentication, _addr->authentication );
		}
		else
		{
			Str::Copy( addr[j].ipPort.ip, _addr[i].ipPort.ip );
			addr[j].ipPort.port = _addr[i].ipPort.port;
			addr[j].type = _addr[i].type;
			Str::Copy( addr[j].authentication, _addr->authentication );
			c_addr++;
		}
		DbgMsg( "Добавили прокси %d, %s:%d '%s'", (int)addr[j].type, addr[j].ipPort.ip, addr[j].ipPort.port, addr[j].authentication );
	}
}

void Connector::Del( const Info* _addr, int count )
{
	CriticalSection cs(lock);
	for( int i = 0; i < count; i++ )
	{
		int j = 0;
		for( ; j < c_addr; j++ )
			if( Str::Cmp( addr[j].ipPort.ip, _addr[i].ipPort.ip ) == 0 && addr[j].ipPort.port == _addr[i].ipPort.port )
				break;
		if( j < c_addr ) //такой есть
		{
			DbgMsg( "Удален прокси %d, %s:%d '%s'", (int)addr[j].type, addr[j].ipPort.ip, addr[j].ipPort.port, addr[j].authentication );
			Mem::Copy( addr + j, addr + j + 1, sizeof(Info) * (c_addr - j - 1) );
			c_addr--;
			if( j == curr )
				curr = -1;
			else
				if( j > curr )
					curr--;
		}
	}
}

static int ConnectToProxy( const Info& addr )
{
	int sc = 0;
	switch( addr.type )
	{
		case HTTP:
		case HTTPS:
			sc = Socket::ConnectHost( addr.ipPort.ip, addr.ipPort.port );
			break;
		case SOCKS5:
			break;
	}
	return sc;
}

int Connector::Connect( const char* host, int port, bool& proxy, StringBuilder& authentication )
{
	int sc = 0;
	proxy = 0;
	if( curr == -1 )
	{
		sc = Socket::ConnectHost( host, port );
		if( sc > 0 )
			return sc;
	}
	//сюда попадаем если напрямую к host соединиться не удалось
	//сначала подключаемся через удачную прокси на прошлом этапе
	if( curr >= 0 )
	{
		sc = ConnectToProxy( addr[curr] );
	}
	if( sc <= 0 )
	{
		//ищем проксю через которую можем сконектится
		CriticalSection cs(lock);
		if( curr < 0 ) curr = 0;
		for( int i = 0; i < c_addr; i++ )
		{
			sc = ConnectToProxy( addr[i] );
			if( sc > 0 ) break;
			curr++;
			if( curr >= c_addr ) curr = 0;
		}
	}
	if( sc > 0 )
	{
		proxy = true;
		authentication = addr[curr].authentication;
		DbgMsg( "Соединились через прокси %d, %s:%d", (int)addr[curr].type, addr[curr].ipPort.ip, addr[curr].ipPort.port );
	}
	else
		curr = -1;
	return sc;
}

};
