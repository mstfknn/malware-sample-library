#include "core\pipe.h"
#include "core\rand.h"
#include "core\crypt.h"
#include "core\debug.h"
#include <Sddl.h>

namespace Pipe
{

PipeResponse* serverPipeResponse = 0;

bool SendAnswer( const Msg* msg, int cmd, const void* data, int sz_data )
{
	PipeClient pipe(msg->answer);
	return pipe.Send( cmd, data, sz_data, 0, msg->func, msg->tag );
}

bool InitServerPipeResponse()
{
	serverPipeResponse = new PipeResponse();
	if( serverPipeResponse )
	{
		DbgMsg( "Pipe сервер приема ответов '%s'", serverPipeResponse->GetName().c_str() );
		serverPipeResponse->StartAsync();
		return true;
	}
	return false;
}

}

PipePoint::PipePoint()
{
	pipe = 0;
	remote += '.';
}

PipePoint::~PipePoint()
{
	Close();
}

void PipePoint::SetName( const char* _name )
{
	int n = Str::IndexOf( _name, '\\' );
	if( n > 0 )
	{
		remote.Copy( _name, n );
		name.Copy( _name + n + 1 );
	}
	else
		name = _name;
}

void PipePoint::GenName( StringBuilder& newName )
{
	Rand::Gen( newName, newName.Size() / 2, newName.Size() - 1 );
}

void PipePoint::Close()
{
	if( pipe )
	{
		API(KERNEL32, CloseHandle)(pipe);
		pipe = 0;
	}
}

StringBuilder& PipePoint::GetFullName( StringBuilder& fullName )
{
	fullName.SetLen(0);
	fullName += '\\';
	fullName += '\\';
	fullName += remote;
	fullName += '\\';
	fullName += 'p'; fullName += 'i'; fullName += 'p'; fullName += 'e';
	fullName += '\\';
	fullName += name;
	return fullName;
}

bool PipePoint::Read( Mem::Data& data )
{
	//читаем длину сообщения
	int size;
	DWORD readed;
	if( !API(KERNEL32, ReadFile)( pipe, &size, sizeof(size), &readed, nullptr ) ) return false;
	if( readed != sizeof(size) ) return false;
	if( !data.MakeEnough(size) ) return false;
	//читаем сообщение
	data.SetLen(0);
	while( data.Len() < size )
	{
		if( !API(KERNEL32, ReadFile)( pipe, data.PtrWrite(), size - data.Len(), &readed, nullptr ) ) return false;
		if( readed == 0 ) return false; 
		data.AddLen(readed);
	}
	//декодируем
	Crypt::EncodeDecodeXorVector( data.Ptr(), data.Len() );
	return true;
}

bool PipePoint::Write( const void* data, int c_data )
{
	if( c_data < 0 ) return false;
	if( c_data == 0 ) return true;
	bool ret = false;
	//копируем в отдельную память данные, чтобы их можно было зашифровать не портя исходные
	void* data2 = Mem::Duplication( data, c_data );
	if( data2 )
	{
		Crypt::EncodeDecodeXorVector( data2, c_data );
		DWORD writed;
		//отправляем размер сообщения
		if( API(KERNEL32, WriteFile)( pipe, &c_data, sizeof(c_data), &writed, nullptr ) ) 
		{
			//отправляем данные
			int sended = 0;
			ret = true;
			while( sended < c_data )
			{
				if( !API(KERNEL32, WriteFile)( pipe, (byte*)data2 + sended, c_data - sended, &writed, nullptr ) ) 
				{
					ret = false;
					break;
				}
				if( writed == 0 )
				{
					ret = false;
					break;
				}
				sended += c_data;
			}
			API(KERNEL32, FlushFileBuffers)(pipe);
		}
		Mem::Free(data2);
	}
	return ret;
}

PipeServer::PipeServer()
{
	//генерируем случайное имя для канала
	GenName(name);
}

PipeServer::~PipeServer()
{
}

int PipeServer::Handler( Pipe::Msg* msgIn, void** msgOut )
{
	int res = 0;
	return res;
}

void PipeServer::Disconnect()
{
}

bool PipeServer::Start( bool onlyRead )
{
	//создаем канал
	StringBuilderStack<48> namePipe;
	GetFullName(namePipe);
	DWORD openMode = PIPE_ACCESS_INBOUND;
	if( !onlyRead ) openMode |= PIPE_ACCESS_OUTBOUND;
	int outBuffer = onlyRead ? 0 : SizeOutBuffer;

	SECURITY_ATTRIBUTES* sa = nullptr;
    SECURITY_ATTRIBUTES securityAttributes;
    SECURITY_DESCRIPTOR securityDescriptor;

    if( API(ADVAPI32, InitializeSecurityDescriptor)( &securityDescriptor, SECURITY_DESCRIPTOR_REVISION ) )
//		if( API(ADVAPI32, SetSecurityDescriptorDacl)( &securityDescriptor, TRUE, NULL, FALSE) )
		if( API(ADVAPI32, SetSecurityDescriptorDacl)( &securityDescriptor, TRUE, NULL, TRUE) )
		{
		    securityAttributes.nLength = sizeof(securityAttributes);
			securityAttributes.lpSecurityDescriptor = &securityDescriptor;
//			securityAttributes.bInheritHandle = FALSE;
			securityAttributes.bInheritHandle = TRUE;
			sa = &securityAttributes;
		}
/*
	PSECURITY_DESCRIPTOR sd;
	if( ConvertStringSecurityDescriptorToSecurityDescriptorA( "S:(ML;;NW;;;S-1-16-0)", SDDL_REVISION_1, &sd, 0 ) )
	{
		DbgMsg( "---- %s", namePipe.c_str() );
	    securityAttributes.nLength = sizeof(securityAttributes);
		securityAttributes.lpSecurityDescriptor = sd;
		securityAttributes.bInheritHandle = FALSE;
		sa = &securityAttributes;
	}
*/
	pipe = API(KERNEL32, CreateNamedPipeA)( namePipe, openMode, PIPE_READMODE_BYTE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, outBuffer, SizeInBuffer, NMPWAIT_USE_DEFAULT_WAIT, sa );
	if( pipe == INVALID_HANDLE_VALUE )
	{
		pipe = 0;
		return false;
	}

	onlyReadOrWrite = onlyRead;
	Loop();
	Close();
	return true;
}

//запуск в отдельном потоке
DWORD WINAPI PipeServerThread( void* data )
{
	PipeServer* server = (PipeServer*)data;
	server->StartAnotherThread();
	return 0;
}

bool PipeServer::StartAsync( bool onlyRead )
{
	onlyReadOrWrite = onlyRead;
	return RunThread( &PipeServerThread, this );
}

void PipeServer::Loop()
{
	bool stop = false;
	Mem::Data data;
	while( !stop )
	{
		if( API(KERNEL32, ConnectNamedPipe)( pipe, nullptr ) )
		{
			if( Read(data) )
			{
				void* out = 0;
				if( ((Pipe::Msg*)data.Ptr())->cmd == CmdDisconnect )
				{
					stop = true;
				}
				else
				{
					int res = Handler( (Pipe::Msg*)data.Ptr(), &out );
					if( res > 0 && out ) //нужно отправить данные
					{
						Write( out, res );
					}
					else
						if( res < 0 )
							stop = true;
				}
			}
			API(KERNEL32, DisconnectNamedPipe)(pipe);
		}
		else
			stop = true;
	}
	Disconnect();
}

struct StruHandlerAsync
{
	Pipe::typeReceiverPipeAnswer func;
	Pipe::Msg* msg;
	DWORD tag;
};

DWORD WINAPI HandlerAsyncThread( void* data )
{
	StruHandlerAsync* data2 = (StruHandlerAsync*)data;
	data2->func( Pipe::AutoMsg(data2->msg), data2->tag );
	Mem::Free(data);
	return 0;
}

bool PipeServer::HandlerAsync( Pipe::typeReceiverPipeAnswer func, Pipe::Msg* msg, DWORD tag )
{
	if( func == 0 ) return false;
	StruHandlerAsync* data = (StruHandlerAsync*)Mem::Alloc( sizeof(StruHandlerAsync) );
	if( data == 0 ) return false;
	data->func = func;
	//дублируем сообщение
	int sizeMsg = sizeof(Pipe::Msg) + msg->sz_data; 
	data->msg = (Pipe::Msg*)Mem::Duplication( msg, sizeMsg );
	data->tag = tag;
	RunThread( HandlerAsyncThread, data );
	return true;
}

void PipeServer::Stop()
{
	PipeClient::Send( name, CmdDisconnect, 0, 0 );
}

PipeResponse::PipeResponse()
{
}

PipeResponse::~PipeResponse()
{
}

int PipeResponse::Handler( Pipe::Msg* msgIn, void** msgOut )
{
	HandlerAsync( msgIn->func, msgIn, msgIn->tag );
	return 0;
}

PipeClient::PipeClient( const char* _name )
{
	SetName(_name);
}

PipeClient::~PipeClient()
{
}

bool PipeClient::Connect( bool onlyWrite )
{
	//создаем канал
	StringBuilderStack<64> namePipe;
	GetFullName(namePipe);
	//DbgMsg( "pipe name %s", namePipe.c_str() );
	DWORD access = GENERIC_WRITE;
	if( !onlyWrite ) access |= GENERIC_READ;
	
	SECURITY_ATTRIBUTES SecAttrib;
	Mem::Zero(SecAttrib);
	SECURITY_DESCRIPTOR SecDesc;
	InitializeSecurityDescriptor(&SecDesc, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&SecDesc, TRUE, NULL, TRUE);
	SecAttrib.nLength = sizeof(SECURITY_ATTRIBUTES);
	SecAttrib.lpSecurityDescriptor = &SecDesc;;
	SecAttrib.bInheritHandle = TRUE;

	for(;;)
	{
		pipe = API(KERNEL32, CreateFileA)( namePipe, access, 0, &SecAttrib, OPEN_EXISTING, 0, 0 );
		if( pipe == INVALID_HANDLE_VALUE )
		{
			if( API(KERNEL32, GetLastError)() != ERROR_PIPE_BUSY )
			{
				DbgMsg( "ERROR %d", GetLastError() );
				pipe = 0;
				return false;
			}
			else
				Delay(1);
		}
		else
			break;
	}
	return true;
}

bool PipeClient::Write( int cmd, const void* data, int sz_data, const char* nameReceiver, Pipe::typeReceiverPipeAnswer funcReceiver, DWORD tag )
{
	if( sz_data < 0 ) return false;
	int sizeMsg = sizeof(Pipe::Msg) + sz_data;
	Pipe::Msg* msg = (Pipe::Msg*)Mem::Alloc(sizeMsg);
	if( msg == 0 ) return false;
	msg->cmd = cmd;
	Str::Copy( msg->answer, sizeof(msg->answer), nameReceiver );
	msg->func = funcReceiver;
	msg->tag = tag;
	msg->sz_data = sz_data;
	Mem::Copy( msg->data, data, sz_data );
	bool ret = PipePoint::Write( msg, sizeMsg );
	Mem::Free(msg);
	return ret;
}

bool PipeClient::Request( int cmd, const void* data, int sz_data, Mem::Data& result )
{
	bool ret = false;
	Pipe::Msg* msg = nullptr;
	if( Connect(false) )
	{
		bool res = Write( cmd, data, sz_data );
		if( res )
		{
			if( Read(result) )
			{
				ret = true;
			}
		}
		Close();
	}
	return ret;
}

bool PipeClient::Send( int cmd, const void* data, int sz_data, const char* nameReceiver, Pipe::typeReceiverPipeAnswer funcReceiver, DWORD tag )
{
	bool ret = false;
	if( Connect() )
	{
		ret = Write( cmd, data, sz_data, nameReceiver, funcReceiver, tag );
		Close();
	}
	return ret;
}

bool PipeClient::Send( const void* data, int sz_data )
{
	bool ret = false;
	if( Connect() )
	{
		ret = PipePoint::Write( data, sz_data );
		Close();
	}
	return ret;
}

bool PipeClient::Send( const char* namePipe, int cmd, const void* data, int sz_data, const char* nameReceiver, Pipe::typeReceiverPipeAnswer funcReceiver, DWORD tag )
{
	PipeClient pipe(namePipe);
	return pipe.Send( cmd, data, sz_data, nameReceiver, funcReceiver, tag );
}

bool PipeClient::Send( const char* namePipe, const void* data, int sz_data )
{
	PipeClient pipe(namePipe);
	return pipe.Send( data, sz_data );
}
