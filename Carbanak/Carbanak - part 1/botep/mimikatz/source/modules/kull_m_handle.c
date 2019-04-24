/*	Benjamin DELPY `gentilkiwi`
	http://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : http://creativecommons.org/licenses/by/3.0/fr/
*/
#include "kull_m_handle.h"

NTSTATUS kull_m_handle_getHandles(PKULL_M_HANDLE_ENUM_CALLBACK callBack, PVOID pvArg)
{
	NTSTATUS status = STATUS_INFO_LENGTH_MISMATCH;
	ULONG i;
	PSYSTEM_HANDLE_INFORMATION buffer = NULL;

	for(i = 0x1000; (status == STATUS_INFO_LENGTH_MISMATCH) && (buffer = (PSYSTEM_HANDLE_INFORMATION) API(KERNEL32, LocalAlloc)(LPTR, i)) ; i <<= 1)
	{
		status = API(NTDLL, NtQuerySystemInformation)(SystemHandleInformation, buffer, i, NULL);
		if(!NT_SUCCESS(status))
			API(KERNEL32, LocalFree)(buffer);
	}
	
	if(NT_SUCCESS(status))
	{
		for(i = 0; (i < buffer->HandleCount) && callBack(&buffer->Handles[i], pvArg); i++);
		API(KERNEL32, LocalFree)(buffer);
	}
	return status;
}