#include "core\core.h"
#include "core\rand.h"

bool RdpWrapInstall();
bool RdpWrapUninstall();

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

extern "C"
{

int WINAPI install( void* )
{
	if( !Mem::Init() ) return false;
	if( !Str::Init() ) return false;
	Rand::Init();
	bool res = RdpWrapInstall();

	Str::Release();
	Mem::Release();
	return res ? 1 : 0;
}

int WINAPI uninstall( void* )
{
	if( !Mem::Init() ) return false;
	if( !Str::Init() ) return false;

	bool res = RdpWrapUninstall();

	Str::Release();
	Mem::Release();
	return res ? 1 : 0;
}


}
