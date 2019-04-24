#include "stdafx.h"
#include "misc.h"

static int MsgDlgV( HWND wnd, const char* msg, const char* caption, int style, va_list args )
{
	char buf[512];
	wvsprintf( buf, msg, args );
	return MessageBox( wnd, buf, caption, style );
}

int MsgDlg( HWND wnd, const char* msg, const char* caption, int style, ... )
{
	va_list args;
	va_start( args, style );
	int ret = MsgDlgV( wnd, msg, caption, style, args );
	va_end(args);
	return ret; 
}

int MsgDlgErr( HWND wnd, const char* msg, int style, ... )
{
	va_list args;
	va_start( args, style );
	int ret = MsgDlgV( wnd, msg, "Ошибка", style | MB_ICONERROR, args );
	va_end(args);
	return ret;
}

int MsgDlgWarn( HWND wnd, const char* msg, int style, ... )
{
	va_list args;
	va_start( args, style );
	int ret = MsgDlgV( wnd, msg, "Предупреждение", style | MB_ICONWARNING, args );
	va_end(args);
	return ret;
}

int MsgDlgMsg( HWND wnd, const char* msg, int style, ... )
{
	va_list args;
	va_start( args, style );
	int ret = MsgDlgV( wnd, msg, "Сообщение", style | MB_ICONINFORMATION, args );
	va_end(args);
	return ret;
}

int MsgDlgQue( HWND wnd, const char* msg, int style, ... )
{
	va_list args;
	va_start( args, style );
	int ret = MsgDlgV( wnd, msg, "Вопрос", style | MB_ICONQUESTION, args );
	va_end(args);
	return ret;
}

