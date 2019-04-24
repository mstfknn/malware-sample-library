#pragma once
#include "stdafx.h"

int MsgDlg( HWND wnd, const char* msg, const char* caption, int style = MB_OK, ... );
int MsgDlgErr( HWND wnd, const char* msg, int style = MB_OK, ... );
int MsgDlgWarn( HWND wnd, const char* msg, int style = MB_YESNO, ... );
int MsgDlgMsg( HWND wnd, const char* msg, int style = MB_OK, ... );
int MsgDlgQue( HWND wnd, const char* msg, int style = MB_YESNO, ... );

