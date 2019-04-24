#include "core\sniffer.h"
#include "core\socket.h"
#include "core\debug.h"

namespace Sniffer
{

static bool FilterIP( ULONG ip, ULONG* ips, int c_ip )
{
	if( c_ip <= 0 || ips == 0 ) return true;
	for( int i = 0; i < c_ip; i++ )
		if( ips[i] == ip )
			return true;
	return false;
}

void Filter( ULONG* ipSrc, int c_ipSrc, ULONG* ipDst, int c_ipDst, UCHAR protocol, typeIPPacket func, int wait, void* tag )
{
	char name[64];
	API(WS2_32,  gethostname)( name, sizeof(name) );
	hostent* phe = API(WS2_32, gethostbyname)(name);
	SOCKADDR_IN sa;
	Mem::Zero(sa);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = ((struct in_addr *)phe->h_addr_list[0])->s_addr;
	Mem::Data data(65536);
	bool stop = false;
	int life = 10; //сколько раз восстанавливать работу сокета, после ошибки чтения
	while( !stop )
	{
		int s = API(WS2_32, socket)( AF_INET, SOCK_RAW, IPPROTO_IP );
		if( s <= 0 )
		{
			//DbgMsg( "Невозможно открыть сокет для снифера (%d)", WSAGetLastError() );
			break;
		}
		API(WS2_32, bind)( s, (SOCKADDR *)&sa, sizeof(SOCKADDR) );
		// Включение promiscuous mode.
		unsigned long flag = 1;  // Флаг PROMISC Вкл/выкл.
		if( API(WS2_32, ioctlsocket)( s, SIO_RCVALL, &flag ) == 0 )
		{
			for(;;)
			{
				int res = Socket::Read( s, data, -1, wait );
				if( res == 0 )
				{
					DbgMsg("ОШИБКА: в снифере при чтении произошла ошибка");
					if( life-- == 0 )
						stop = true;
					else
						Delay(1000);
					break;
				}
				bool res2 = false;
				if( res == 3 ) //вышло время ожидания
				{
					res2 = func( 0, 0, 0, tag );
				}
				else
				{
					life = 10;
					IPHeader* header = (IPHeader*)data.Ptr();
					if( header->iph_length >= data.Len() ) //весь пакет считан
					{
						if( (protocol == 0 || protocol == header->iph_protocol) && FilterIP( header->iph_src, ipSrc, c_ipSrc ) && FilterIP( header->iph_dest, ipDst, c_ipDst )  )
						{
							res2 = func( header, data.p_byte() + sizeof(IPHeader), data.Len() - sizeof(IPHeader), tag );
						}
						data.DelLeft(header->iph_length);
					}
				}
				if( res2 ) 
				{
					stop = true;
					break;
				}
			}
		}
		else
		{
			DbgMsg( "Невозможно для снифера перевести сокет %d в режим перехвата пакетов (%d)", s, WSAGetLastError() );
			stop = true;
		}
		Socket::Close(s);
	}
}

}
