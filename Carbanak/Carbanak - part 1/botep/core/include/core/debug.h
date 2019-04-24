#pragma once

#ifdef DEBUG_STRINGS

void DbgMsg( const char *format, ... );

#else

#define DbgMsg( ... ) __noop

#endif //DEBUG_STRINGS
