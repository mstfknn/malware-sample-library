#pragma once

#include <Windows.h>
#include "type.h"

#ifdef WINAPI_INVISIBLE

#ifdef __cplusplus
extern "C"
{
#endif

void* GetApiAddrFunc( int dll, uint hashFunc );

#ifdef __cplusplus
}
#endif

#define API(DLL, FUNC) ((type##FUNC)GetApiAddrFunc( DLL, hash##FUNC ))

#else

#define API(DLL, FUNC) FUNC

#endif

#define KERNEL32 0
#define USER32 1
#define NTDLL 2
#define SHLWAPI 3
#define IPHLPAPI 4
#define URLMON 5
#define WS2_32 6
#define CRYPT32 7
#define SHELL32 8
#define ADVAPI32 9
#define GDIPLUS 10
#define GDI32 11
#define OLE32 12
#define PSAPI 13
#define CABINET 14
#define IMAGEHLP 15
#define NETAPI32 16
#define WTSAPI32 17
#define MPR 18
#define WINHTTP 19
