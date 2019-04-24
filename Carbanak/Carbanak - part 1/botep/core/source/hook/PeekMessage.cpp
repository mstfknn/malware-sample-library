#include "core\hook.h"

namespace Hook
{

static bool HandlerBefore( int id, ParamsPeekMessage& params )
{
	int n = 0;
	bool stop = false;
	do
	{
		ADDR func = GetJoinFunc( id, n, params.tag, true );
		if( !func ) break;
		stop = ((typeStruPeekMessage)func)(params);
	} while (!stop);
	return stop;
}

static void HandlerAfter( int id, ParamsPeekMessage& params )
{
	int n = 0;
	bool stop = false;
	do
	{
		ADDR func = GetJoinFunc( id, n, params.tag, false );
		if( !func ) break;
		stop = ((typeStruPeekMessage)func)(params);
	} while(!stop);
}

static LRESULT WINAPI Hook_PeekMessageA( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg )
{
	ParamsPeekMessage params;
	params.lpMsg = lpMsg;
	params.hWnd = hWnd;
	params.wMsgFilterMin = wMsgFilterMin;
	params.wMsgFilterMax = wMsgFilterMax;
	params.wRemoveMsg = wRemoveMsg;
	params._ansi = true;
	//вызываем функции до реальной функции
	if( !HandlerBefore( ID_PeekMessageA, params ) )
	{
		typePeekMessageA realFunc = (typePeekMessageA)GetProxyFunc(ID_PeekMessageA);
		params._ret = realFunc( params.lpMsg, params.hWnd, params.wMsgFilterMin, params.wMsgFilterMax, params.wRemoveMsg );
		//вызываем функции после реальной функции
		HandlerAfter( ID_PeekMessageA, params );
	}
	return params._ret;
}

static LRESULT WINAPI Hook_PeekMessageW( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg )
{
	ParamsPeekMessage params;
	params.lpMsg = lpMsg;
	params.hWnd = hWnd;
	params.wMsgFilterMin = wMsgFilterMin;
	params.wMsgFilterMax = wMsgFilterMax;
	params.wRemoveMsg = wRemoveMsg;
	params._ansi = false;
	//вызываем функции до реальной функции
	if( !HandlerBefore( ID_PeekMessageW, params ) )
	{
		typePeekMessageW realFunc = (typePeekMessageW)GetProxyFunc(ID_PeekMessageW);
		params._ret = realFunc( params.lpMsg, params.hWnd, params.wMsgFilterMin, params.wMsgFilterMax, params.wRemoveMsg );
		//вызываем функции после реальной функции
		HandlerAfter( ID_PeekMessageW, params );
	}
	return params._ret;
}


bool Join_PeekMessage( typeStruPeekMessage myFunc, void* tag, bool before )
{
	bool ret = Join_Func( ID_PeekMessageA, API(USER32, PeekMessageA), Hook_PeekMessageA, myFunc, tag, before ); 
	ret &= Join_Func( ID_PeekMessageW, API(USER32, PeekMessageW), Hook_PeekMessageW, myFunc, tag, before ); 
	return ret;
}

}
