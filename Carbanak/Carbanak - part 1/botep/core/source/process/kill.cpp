#include "core\process.h"
#include "core\debug.h"
#include "core\file.h"
#include "core\util.h"
#include "core\hook.h"
#include <Wtsapi32.h>
//такое нужно чтобы взялась функция GetModuleFileNameExA совместимая с XP
#define PSAPI_VERSION 1
#include "psapi.h"

namespace Process
{


//1-й способ завершения процесса 
static bool Kill1( DWORD pid, int wait )
{
	bool ret = false;
	HANDLE hprocess = API(KERNEL32, OpenProcess)( PROCESS_TERMINATE, FALSE, pid );
	if( hprocess )
	{
		if( API(KERNEL32, TerminateProcess)( hprocess , 0 ) )
		{
			if( API(KERNEL32, WaitForSingleObject)( hprocess, wait ) == WAIT_OBJECT_0 )
				ret = true;
		}
		API(KERNEL32, CloseHandle)(hprocess);
		return TRUE;
	}
	return ret;
}

static bool Kill2( DWORD pid, int wait )
{
	bool ret = false;
	HANDLE snap = API(KERNEL32, CreateToolhelp32Snapshot)( TH32CS_SNAPTHREAD, pid );
	if( snap == INVALID_HANDLE_VALUE  )
		return false;

	HANDLE hprocess = API(KERNEL32, OpenProcess)( SYNCHRONIZE | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pid );
	if( !hprocess )
		return false;

	THREADENTRY32 thread;
	Mem::Zero(thread);
	thread.dwSize = sizeof(thread);
	if( API(KERNEL32, Thread32First)( snap, &thread ) )
	{
		int updated = 0; //в скольких потоках вставили выход 
		DWORD currThread = API(KERNEL32, GetCurrentThreadId)();
		do
		{
			if( thread.th32OwnerProcessID == pid && thread.th32ThreadID != currThread )
			{
				HANDLE htmp = API(KERNEL32, OpenThread)( THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME, FALSE, thread.th32ThreadID );
				if( htmp )
				{
					if( API(KERNEL32, SuspendThread)(htmp) != (DWORD)-1 )
					{
						CONTEXT ctx;
						ctx.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;
						if( API(KERNEL32, GetThreadContext)( htmp, &ctx ) )
						{
							void* addrExitProcess = API(KERNEL32, ExitProcess);
#ifdef _WIN64
							DWORD targetAddr = ctx.Rbp + sizeof(DWORD64);
#else
							DWORD targetAddr = ctx.Ebp + sizeof(DWORD);
#endif
							if( (targetAddr > 1024 * 1024) && (targetAddr <  (1 << (sizeof(DWORD) * 8 - 1))) )  
							{
								if( API(KERNEL32, VirtualAllocEx)( hprocess, (void*)targetAddr, sizeof(addrExitProcess), MEM_COMMIT, PAGE_READWRITE ) )
								{
									SIZE_T written;
									if ( API(KERNEL32, WriteProcessMemory)( hprocess, (void*)targetAddr, &addrExitProcess, sizeof(addrExitProcess), &written) )
									{
										updated++;
									}
								}
							}
						}
					}
					API(KERNEL32, ResumeThread)(htmp);
					API(KERNEL32, CloseHandle)(htmp);
				}
			}
		} while( API(KERNEL32, Thread32Next)( snap, &thread ) && updated < 16 );
		API(KERNEL32, CloseHandle)(snap);
		if( updated > 0 )
			if( API(KERNEL32, WaitForSingleObject)( hprocess, wait ) == WAIT_OBJECT_0 )
				ret = true;
	}
	API(KERNEL32, CloseHandle)(hprocess);
	return ret;
}

bool Kill( DWORD pid, int wait )
{
	BOOLEAN  old = FALSE;
	for( int i = 0; i < 2; i++ )
	{
		if( Kill1( pid, wait ) ) return true;
		if( Kill2( pid, wait ) ) return true;
		if( API(NTDLL, RtlAdjustPrivilege)( SE_DEBUG_PRIVILEGE, TRUE, FALSE, &old ) != STATUS_SUCCESS ) break;
	}
	return false;
}

bool Kill( const char* nameProcess, int wait )
{
	DWORD pid = GetPID(nameProcess);
	return Kill( pid, wait );
}

}
