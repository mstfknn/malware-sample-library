#include "core\util.h"
#include "core\string.h"
#include "core\debug.h"

namespace Window
{

int GetCaption( HWND hwnd, StringBuilder& s )
{
	int len = API(USER32, GetWindowTextA)( hwnd, s, s.Size() );
	s.SetLen(len);
	return len;
}

int GetNameClass( HWND hwnd, StringBuilder& s )
{
	int len = API(USER32, GetClassNameA)( hwnd, s, s.Size() );
	s.SetLen(len);
	return len;
}

HWND GetParent( HWND hwnd )
{
#ifdef WINAPI_INVISIBLE
	return API(USER32, GetParent)(hwnd);
#else
	return ::API(USER32, GetParent)(hwnd);
#endif
}

HWND GetTopParent( HWND hwnd )
{
	HWND ret = 0;
	for(;;)
	{
		hwnd = Window::GetParent(hwnd);
		if( !hwnd ) break;
		ret = hwnd;
	}
	return ret;
}

HWND GetParentWithCaption( HWND hwnd )
{
	HWND ret = 0;
	while( hwnd )
	{
		ret = hwnd;
		uint style = API(USER32, GetWindowLongA)( hwnd, GWL_STYLE );
		if( style & WS_CAPTION ) break;
		hwnd = Window::GetParent(hwnd);
	}
	return ret;
}

}
