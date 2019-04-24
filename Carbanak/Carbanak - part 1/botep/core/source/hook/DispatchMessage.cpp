#include "core\hook.h"

namespace Hook
{

static bool HandlerBefore( int id, ParamsDispatchMessage& params )
{
	int n = 0;
	bool stop = false;
	do
	{
		ADDR func = GetJoinFunc( id, n, params.tag, true );
		if( !func ) break;
		stop = ((typeStruDispatchMessage)func)(params);
	} while (!stop);
	return stop;
}

static void HandlerAfter( int id, ParamsDispatchMessage& params )
{
	int n = 0;
	bool stop = false;
	do
	{
		ADDR func = GetJoinFunc( id, n, params.tag, false );
		if( !func ) break;
		stop = ((typeStruDispatchMessage)func)(params);
	} while(!stop);
}

static LRESULT WINAPI Hook_DispatchMessageA( const MSG* lpmsg )
{
	ParamsDispatchMessage params;
	params.lpmsg = (MSG*)lpmsg;
	params._ansi = true;
	//вызываем функции до реальной функции
	if( !HandlerBefore( ID_DispatchMessageA, params ) )
	{
		typeDispatchMessageA realFunc = (typeDispatchMessageA)GetProxyFunc(ID_DispatchMessageA);
		params._ret = realFunc( params.lpmsg );
		//вызываем функции после реальной функции
		HandlerAfter( ID_DispatchMessageA, params );
	}
	return params._ret;
}

static LRESULT WINAPI Hook_DispatchMessageW( const MSG* lpmsg )
{
	ParamsDispatchMessage params;
	params.lpmsg = (MSG*)lpmsg;
	params._ansi = false;
	//вызываем функции до реальной функции
	if( !HandlerBefore( ID_DispatchMessageW, params ) )
	{
		typeDispatchMessageW realFunc = (typeDispatchMessageW)GetProxyFunc(ID_DispatchMessageW);
		params._ret = realFunc( params.lpmsg );
		//вызываем функции после реальной функции
		HandlerAfter( ID_DispatchMessageW, params );
	}
	return params._ret;
}


bool Join_DispatchMessage( typeStruDispatchMessage myFunc, void* tag, bool before )
{
	bool ret = Join_Func( ID_DispatchMessageA, API(USER32, DispatchMessageA), Hook_DispatchMessageA, myFunc, tag, before ); 
	ret &= Join_Func( ID_DispatchMessageW, API(USER32, DispatchMessageW), Hook_DispatchMessageW, myFunc, tag, before ); 
	return ret;
}

}
