#include "core\PipeSocket.h"
#include "core\socket.h"
#include "core\debug.h"

PipeSocketServer::PipeSocketServer( const char* pipeName, const char* _ip, int _port )
{
	SetName(pipeName);
	Str::Copy( ip, sizeof(ip), _ip );
	port = _port;
}

PipeSocketServer::~PipeSocketServer()
{
}

void PipeSocketServer::Disconnect()
{
	DbgMsg( "Close tunnel pipe %s", name.c_str() );
}

int PipeSocketServer::Handler( Pipe::Msg* msgIn, void** msgOut )
{
	int ret = 0;
	switch( msgIn->cmd )
	{
		case CmdConnect:
			{
				DataConnect* dc = (DataConnect*)msgIn->data;
				int sc;
				if( dc->port == 0 )
					sc = Socket::ConnectIP( ip, port );
				else
					sc = Socket::ConnectIP( dc->ip, dc->port );
				DbgMsg( "Connect %s:%d, sc=%d", ip, port, sc );
				result.Copy( &sc, sizeof(sc) );
				*msgOut = result.Ptr();
				ret = result.Len();
			}
			break;
		case CmdRecv:
			{
				DataRecv* dr = (DataRecv*)msgIn->data;
				Mem::Data data;
				DbgMsg( "Recv sc=%d size=%d wait=%d", dr->sc, dr->maxRead, dr->wait );
				int res = Socket::Read( dr->sc, data, dr->maxRead, dr->wait );
				DbgMsg( "Recv sc=%d size=%d res=%d", dr->sc, dr->maxRead, res );
				result.Clear();
				ret = sizeof(DataRecvAnswer) + data.Len();
				result.MakeEnough(ret);
				DataRecvAnswer* a = (DataRecvAnswer*)result.Ptr();
				a->res = res;
				a->c_data = data.Len();
				Mem::Copy( a->data, data.Ptr(), data.Len() );
				*msgOut = result.Ptr();
			}
			break;
		case CmdSend:
			{
				DataSend* ds = (DataSend*)msgIn->data;
				int res = Socket::Write( ds->sc, ds->data, ds->c_data );
				DbgMsg( "Send sc=%d size=%d res=%d", ds->sc, ds->c_data, res );
				result.Copy( &res, sizeof(res) );
				*msgOut = result.Ptr();
				ret = result.Len();
			}
			break;
		case CmdClose:
			{
				int sc = *((int*)msgIn->data);
				DbgMsg( "Close sc %d", sc );
				Socket::Close(sc);
			}
			break;
	}
	return ret;
}

bool EstablishConnection( const char* comp, const char* rsrc, const char* user, const char* psw )
{
	char remoteRsrc[128];
	Str::Format( remoteRsrc, _CS_("\\\\%s\\%s"), comp, rsrc );
	NETRESOURCE nr;
	nr.dwType = RESOURCETYPE_ANY;
	nr.lpLocalName = NULL;
	nr.lpRemoteName = remoteRsrc;
	nr.lpProvider = NULL;
	DWORD res = API(MPR, WNetAddConnection2A)( &nr, psw, user, FALSE );
	if( res == NO_ERROR )
	{
		return true;
	}
	else
	{
		DbgMsg( "Error connect to %s %d", remoteRsrc, res );
		return false;
	}
}


PipeSocketClient::PipeSocketClient( const char* pipeName, const char* user, const char* password ) : PipeClient(pipeName)
{
	EstablishConnection( remote, "IPC$", user, password );
}

int PipeSocketClient::ConnectIP( const char* ip, int port )
{
	PipeSocketServer::DataConnect dc;
	Str::Copy( dc.ip, sizeof(dc.ip), ip );
	dc.port = port;
	Mem::Data result;
	if( Request( PipeSocketServer::CmdConnect, &dc, sizeof(PipeSocketServer::DataConnect), result ) )
		if( result.Len() == sizeof(int) )
			return *((int*)result.Ptr());
	return SOCKET_ERROR;
}

int PipeSocketClient::Write( int sc, const void* data, int c_data )
{
	if( data == 0 || c_data <= 0 ) return 0;
	int c_ds = sizeof(PipeSocketServer::DataSend) + c_data;
	PipeSocketServer::DataSend* ds = (PipeSocketServer::DataSend*)Mem::Alloc(c_ds);
	ds->sc = sc;
	ds->c_data = c_data;
	Mem::Copy( ds->data, data, c_data );
	Mem::Data result;
	int ret = SOCKET_ERROR;
	if( Request( PipeSocketServer::CmdSend, ds, c_ds, result ) )
		if( result.Len() == sizeof(int) )
			ret = *((int*)result.Ptr());
	Mem::Free(ds);
	DbgMsg( "PipeSocket write res %d", ret );
	return ret;
}

int PipeSocketClient::Read( int sc, Mem::Data& data, int max_read, int wait )
{
	PipeSocketServer::DataRecv dr;
	dr.sc = sc;
	dr.maxRead = max_read;
	dr.wait = wait;
	Mem::Data result;
	data.Clear();
	DbgMsg( "PipeSocket read max_read %d, wait %d", max_read, wait );
	if( Request( PipeSocketServer::CmdRecv, &dr, sizeof(PipeSocketServer::DataRecv), result ) )
		if( result.Len() >= sizeof(PipeSocketServer::DataRecvAnswer) )
		{
			PipeSocketServer::DataRecvAnswer* a = (PipeSocketServer::DataRecvAnswer*)result.Ptr();
			data.Copy( a->data, a->c_data );
			DbgMsg( "PipeSocket read res %d, c_data %d", a->res, a->c_data );
			return a->res;
		}
	return SOCKET_ERROR;
}

void PipeSocketClient::Close( int sc )
{
	Send( PipeSocketServer::CmdClose, &sc, sizeof(sc) );
}
