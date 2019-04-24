#include "core\core.h"
#include "core\injects.h"

DWORD WINAPI ExplorerEntry( void* )
{
	OutputDebugStringA("Inner Explorer.exe");
	return 0;
}

int main()
{
	if( !Core::Init() ) return false;
	DWORD pid = Process::GetExplorerPID();
	if( pid )
	{
		InjectIntoProcess2( pid, ExplorerEntry );
	}
}

/*
StringDecoded DECODE_STRING( const char* codeStr )
{
	char* s = Str::Duplication(codeStr);
	s += 2;
	s[ Str::Len(s) - 2 ] = 0;
	return StringDecoded(s);
}
*/