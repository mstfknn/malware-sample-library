#include "core\winapi.h"
#include "core\debug.h"
#include "core\string.h"

#ifdef DEBUG_STRINGS

void DbgMsg( const char *format, ... )
{
	char buf[512];
	va_list va;
	va_start( va, format );
	Str::FormatVA( buf, format, va );
	API(KERNEL32, OutputDebugStringA)(buf);
}

#endif //DEBUG_STRINGS
