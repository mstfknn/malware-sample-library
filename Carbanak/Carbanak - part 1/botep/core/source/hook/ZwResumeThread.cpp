#include "core\hook.h"

namespace Hook
{

static NTSTATUS WINAPI Hook_ZwResumeThread( HANDLE hThread, PULONG PreviousSuspendCount )
{
	ParamsZwResumeThread params;
	params.hThread = hThread;
	params.PreviousSuspendCount = PreviousSuspendCount;
	int n = 0;
	bool stop = false;
	//вызываем функции до реальной функции
	do
	{
		ADDR func = GetJoinFunc( ID_ZwResumeThread, n, params.tag, true );
		if( !func ) break;
		stop = ((typeStruZwResumeThread)func)(params);
	} while(!stop);
	if( !stop )
	{
		typeZwResumeThread realFunc = (typeZwResumeThread)GetProxyFunc(ID_ZwResumeThread);
		params._ret = realFunc( params.hThread, params.PreviousSuspendCount );
		//вызываем функции после реальной функции
		n = 0;
		do
		{
			ADDR func = GetJoinFunc( ID_ZwResumeThread, n, params.tag, false );
			if( !func ) break;
			stop = ((typeStruZwResumeThread)func)(params);
		} while(!stop);
	}
	return params._ret;
}

bool Join_ZwResumeThread( typeStruZwResumeThread myFunc, void* tag, bool before )
{
	return Join_Func( ID_ZwResumeThread, API(NTDLL, ZwResumeThread), Hook_ZwResumeThread, myFunc, tag, before ); 
}

}
