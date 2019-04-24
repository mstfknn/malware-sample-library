#include "core\core.h"

#ifdef ON_CODE_STRING

StringDecoded DECODE_STRING( const char* codeStr )
{
	char* s = Str::Duplication(codeStr);
	s += 2;
	s[ Str::Len(s) - 2 ] = 0;
	return StringDecoded(s);
}

#else

StringDecoded DECODE_STRING( const char* codeStr )
{
	char* s = Str::Duplication(codeStr);
	return StringDecoded(s);
}

#endif //ON_CODE_STRING

