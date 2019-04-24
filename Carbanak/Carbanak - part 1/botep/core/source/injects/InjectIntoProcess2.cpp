#include "core\injects.h"
#include "core\debug.h"

bool InjectIntoProcess2( DWORD pid, typeFuncThread func )
{
	bool ret = false;
	OBJECT_ATTRIBUTES oa;
	oa.uLength = sizeof(OBJECT_ATTRIBUTES);

	CLIENT_ID clientID;
	clientID.UniqueProcess = (HANDLE)pid;
	clientID.UniqueThread  = 0;

	HANDLE hprocess = API(KERNEL32, OpenProcess)( PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
	ret = RunInjectCode3( hprocess, 0, func, InjectCode2 );
	API(KERNEL32, CloseHandle)(hprocess);
	return ret;
}

