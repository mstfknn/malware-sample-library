#include "core\util.h"
#include "core\string.h"
#include "core\debug.h"

namespace Mutex
{

static void CreateNameMutex( StringBuilder& nameMutex, const StringBuilder& name )
{
	nameMutex = _CS_("Global\\");
	nameMutex += name;
}

HANDLE Create( const StringBuilder& name )
{
	StringBuilderStack<64> nameMutex;
	CreateNameMutex( nameMutex, name );
	HANDLE mutex = API(KERNEL32, CreateMutexA)( 0, FALSE, nameMutex );
	if( mutex )
	{
		int err = API(KERNEL32, GetLastError)();	
		if( err == ERROR_ALREADY_EXISTS )
		{
			API(KERNEL32, ReleaseMutex)(mutex);
			mutex = 0;
		}
	}
	return mutex;
}

void Release( HANDLE mutex )
{
	API(KERNEL32, CloseHandle)(mutex);
}

}
