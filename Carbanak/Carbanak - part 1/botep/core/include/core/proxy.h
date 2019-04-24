#pragma once

#include "core.h"
#include "misc.h"

struct AddressIpPort
{
	char ip[128]; //тут может быть как айпи адрес так и имя домена
	int port;

	bool Parse( const char* s )
	{
		return ExtractIpPort( s, ip, port );
	}
};

namespace Proxy
{

enum Type
{
	HTTP, HTTPS, SOCKS5 //в порядке важности
};

struct Info
{
	Type type;
	AddressIpPort ipPort;
	char authentication[128]; //строка аутентификации для HTTP заголовка
};

//создает соединение с указанным хостом, и если не удается
//то соединяется с одим из прокси, при этом номер успешного прокси запоминается
//для использования в следующий раз
class Connector
{
		static const int MAX_PROXY = 10;

	public:
		Info addr[MAX_PROXY];
		int c_addr;
	
	private:
		int curr; //номер прокси через который в последний раз был коннект
		CRITICAL_SECTION lock;

	public:

		Connector();
		~Connector();
		void Add( const Info* _addr, int count );
		void Del( const Info* _addr, int count );
		//соединяется с указанным хостом или с прокси, в authentication будет строка аутентификации для прокси, если такое есть
		int Connect( const char* host, int port, bool& proxy, StringBuilder& authentication );
};

}
