#include "core\process.h"
#include "core\debug.h"
#define PSAPI_VERSION 1
#include "psapi.h"

namespace Process
{

static bool EnumModules( ProcessInfo& pi, void* tag )
{
	HANDLE hprocess = API(KERNEL32, OpenProcess)( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pi.pid );
	HMODULE mods[128];
	DWORD needed;
	if( API(PSAPI, EnumProcessModules)( hprocess, mods, sizeof(mods), &needed ) )
	{
		for( int i = 0; i < needed / sizeof(HMODULE); i++ )
		{
			char fileName[MAX_PATH];
			if( API(PSAPI, GetModuleFileNameExA)( hprocess, mods[i], fileName, sizeof(fileName) ) > 0 )
			{
				char* p = Path::GetFileName(fileName);
				Str::Lower(p);
				if( Str::Cmp( p, (char*)tag ) == 0 )
				{
					Kill( pi.pid, 1000 );
					return false;
				}
			}
		}
	}
	return false;
}

void KillLoadedModule( const char* nameModule )
{
	ListProcess( EnumModules, (void*)nameModule );
}

}
