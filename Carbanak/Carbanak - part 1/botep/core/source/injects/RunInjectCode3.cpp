#include "core\injects.h"
#include "core\pe.h"
#include "core\debug.h"
#include "core\ntdll.h"

bool RunInjectCode3( HANDLE hprocess, HANDLE hthread, typeFuncThread startFunc, typeInjectCode func )
{
	ADDR addr = func( hprocess, startFunc, 0 );
	if( addr == 0 ) return false;
	DWORD id;
	HANDLE hthread2 = API(KERNEL32, CreateRemoteThread)( hprocess, NULL, 0, (LPTHREAD_START_ROUTINE)addr, NULL, 0 /*CREATE_SUSPENDED*/, &id );
	if( hthread2 )
	{
		//ZwResumeThread((DWORD)hthread2, 0);
		return true;
	}
	else 
	{
		HANDLE hthread;
		CLIENT_ID cid;
		if( API(NTDLL, RtlCreateUserThread)( hprocess, nullptr, FALSE, 0, 0, 0, (void*)addr, 0, &hthread, &cid ) == STATUS_SUCCESS )
		{
			API(KERNEL32, CloseHandle)(hthread);
			return true;
		}
	}
	return false;
}

