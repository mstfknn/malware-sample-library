// botcmd_dll.cpp: определяет экспортированные функции для приложения DLL.
//

#include "core\core.h"
#include "core\process.h"

void Start()
{
	if( !WinAPI::Init() ) return;
	if( !Core::Init() ) return;
	Process::Exec( "regedit.exe" );
}

