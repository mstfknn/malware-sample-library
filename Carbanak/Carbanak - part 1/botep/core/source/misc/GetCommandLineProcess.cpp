#include "core\core.h"
#include "core\debug.h"

namespace Process
{

bool GetCommandLine( uint pid, StringBuilder& s )
{
	HANDLE hprocess = API(KERNEL32, OpenProcess)( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid );
	if( !hprocess ) return false;
	PROCESS_BASIC_INFORMATION pbi;
	bool ret = false;
	if( API(NTDLL, NtQueryInformationProcess)( hprocess, (PROCESSINFOCLASS)0, &pbi, sizeof(pbi), NULL ) >= 0 )
	{
		void* rtlUserProcParamsAddress;
		if( API(KERNEL32, ReadProcessMemory)( hprocess, (PCHAR)pbi.PebBaseAddress + 0x10, &rtlUserProcParamsAddress, sizeof(rtlUserProcParamsAddress), NULL) )
		{
			UNICODE_STRING commandLine;
			if( API(KERNEL32, ReadProcessMemory)( hprocess, (PCHAR)rtlUserProcParamsAddress + 0x40, &commandLine, sizeof(commandLine), NULL) )
			{
				wchar_t* commandLineContents = (wchar_t*)Mem::Alloc(commandLine.Length);
				if( API(KERNEL32, ReadProcessMemory)( hprocess, commandLine.Buffer, commandLineContents, commandLine.Length, NULL))
				{
					s.ToWin1251( commandLineContents, commandLine.Length / sizeof(wchar_t) );
					ret = true;
				}
				Mem::Free(commandLineContents);
			}
		}
	}
	API(KERNEL32, CloseHandle)(hprocess);
	return ret;
}

}
