#include "core\hook.h"

namespace Hook
{

static NTSTATUS WINAPI Hook_DestroyWindow( HWND hWnd )
{
	ParamsDestroyWindow params;
	params.hWnd = hWnd;
	int n = 0;
	bool stop = false;
	//вызываем функции до реальной функции
	do
	{
		ADDR func = GetJoinFunc( ID_DestroyWindow, n, params.tag, true );
		if( !func ) break;
		stop = ((typeStruDestroyWindow)func)(params);
	} while(!stop);
	if( !stop )
	{
		typeDestroyWindow realFunc = (typeDestroyWindow)GetProxyFunc(ID_DestroyWindow);
		params._ret = realFunc( params.hWnd );
		//вызываем функции после реальной функции
		n = 0;
		do
		{
			ADDR func = GetJoinFunc( ID_DestroyWindow, n, params.tag, false );
			if( !func ) break;
			stop = ((typeStruDestroyWindow)func)(params);
		} while(!stop);
	}
	return params._ret;
}

bool Join_DestroyWindow( typeStruDestroyWindow myFunc, void* tag, bool before )
{
	return Join_Func( ID_DestroyWindow, API(USER32, DestroyWindow), Hook_DestroyWindow, myFunc, tag, before ); 
}

}
