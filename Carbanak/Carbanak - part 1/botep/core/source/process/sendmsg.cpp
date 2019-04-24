#include "core\process.h"
#include "core\debug.h"
#include "core\util.h"

namespace Process
{

bool SendMsg( HWND hwnd, DWORD msg, WPARAM wparam, LPARAM lparam )
{
	MSG msgs;
	msgs.hwnd = hwnd;
	msgs.message = msg;
	msgs.wParam = wparam;
	msgs.lParam = lparam;
	return SendMsg( hwnd, &msgs, 1 );
}

bool SendMsg( HWND hwnd, MSG* msgs, int c_msgs )
{
	bool ret = false;
	DWORD pid;
	DWORD tid = API(USER32, GetWindowThreadProcessId)( hwnd, &pid );
	DWORD curThreadID = API(KERNEL32, GetCurrentThreadId)();
	HANDLE proc = API(KERNEL32, OpenProcess)( PROCESS_QUERY_INFORMATION + SYNCHRONIZE, FALSE, pid );
	if( proc )
	{
		API(USER32, AttachThreadInput)( curThreadID, tid, TRUE );
		API(USER32, BlockInput)(TRUE);
		for( int i = 0; i < c_msgs; i++ )
		{
			API(USER32, PostMessageA)( msgs[i].hwnd, msgs[i].message, msgs[i].wParam, msgs[i].lParam );
			API(USER32, WaitForInputIdle)( proc, INFINITE );
		}
		API(USER32, AttachThreadInput)( curThreadID, tid, FALSE );
		ret = true;
	}
	API(KERNEL32, CloseHandle)(proc);
	return ret;
}

}
