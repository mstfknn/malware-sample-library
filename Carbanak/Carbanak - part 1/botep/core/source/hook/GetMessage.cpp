#include "core\hook.h"

namespace Hook
{

static bool HandlerBefore( int id, ParamsGetMessage& params )
{
	int n = 0;
	bool stop = false;
	do
	{
		ADDR func = GetJoinFunc( id, n, params.tag, true );
		if( !func ) break;
		stop = ((typeStruGetMessage)func)(params);
	} while (!stop);
	return stop;
}

static void HandlerAfter( int id, ParamsGetMessage& params )
{
	int n = 0;
	bool stop = false;
	do
	{
		ADDR func = GetJoinFunc( id, n, params.tag, false );
		if( !func ) break;
		stop = ((typeStruGetMessage)func)(params);
	} while(!stop);
}

static LRESULT WINAPI Hook_GetMessageA( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax )
{
	ParamsGetMessage params;
	params.lpMsg = lpMsg;
	params.hWnd = hWnd;
	params.wMsgFilterMin = wMsgFilterMin;
	params.wMsgFilterMax = wMsgFilterMax;
	params._ansi = true;
	//вызываем функции до реальной функции
	if( !HandlerBefore( ID_GetMessageA, params ) )
	{
		typeGetMessageA realFunc = (typeGetMessageA)GetProxyFunc(ID_GetMessageA);
		params._ret = realFunc( params.lpMsg, params.hWnd, params.wMsgFilterMin, params.wMsgFilterMax );
		//вызываем функции после реальной функции
		HandlerAfter( ID_GetMessageA, params );
	}
	return params._ret;
}

static LRESULT WINAPI Hook_GetMessageW( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax )
{
	ParamsGetMessage params;
	params.lpMsg = lpMsg;
	params.hWnd = hWnd;
	params.wMsgFilterMin = wMsgFilterMin;
	params.wMsgFilterMax = wMsgFilterMax;
	params._ansi = false;
	//вызываем функции до реальной функции
	if( !HandlerBefore( ID_GetMessageW, params ) )
	{
		typeGetMessageW realFunc = (typeGetMessageW)GetProxyFunc(ID_GetMessageW);
		params._ret = realFunc( params.lpMsg, params.hWnd, params.wMsgFilterMin, params.wMsgFilterMax );
		//вызываем функции после реальной функции
		HandlerAfter( ID_GetMessageW, params );
	}
	return params._ret;
}


bool Join_GetMessage( typeStruGetMessage myFunc, void* tag, bool before )
{
	bool ret = Join_Func( ID_GetMessageA, API(USER32, GetMessageA), Hook_GetMessageA, myFunc, tag, before ); 
	ret &= Join_Func( ID_GetMessageW, API(USER32, GetMessageW), Hook_GetMessageW, myFunc, tag, before ); 
	return ret;
}

}
