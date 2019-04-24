/*	Benjamin DELPY `gentilkiwi`
	http://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : http://creativecommons.org/licenses/by/3.0/fr/
*/

#include "kuhl_m_ts.h"
#include "core\string_crypt.h"

/*
const KUHL_M_C kuhl_m_c_ts[] = {
	{kuhl_m_ts_multirdp,	L"multirdp",	L"[experimental] patch Terminal Server service to allow multiples users"},
};
const KUHL_M kuhl_m_ts = {
	L"ts",	L"Terminal Server module", NULL,
	sizeof(kuhl_m_c_ts) / sizeof(KUHL_M_C), kuhl_m_c_ts, NULL, NULL
};
*/

#ifdef _M_X64
BYTE PTRN_WN60_Query__CDefPolicy[]	= {0x8b, 0x81, 0x38, 0x06, 0x00, 0x00, 0x39, 0x81, 0x3c, 0x06, 0x00, 0x00, 0x75};
BYTE PTRN_WN6x_Query__CDefPolicy[]	= {0x39, 0x87, 0x3c, 0x06, 0x00, 0x00, 0x0f, 0x84};
BYTE PTRN_WN81_Query__CDefPolicy[]	= {0x39, 0x81, 0x3c, 0x06, 0x00, 0x00, 0x0f, 0x84};
BYTE PATC_WN60_Query__CDefPolicy[]	= {0xc7, 0x81, 0x3c, 0x06, 0x00, 0x00, 0xff, 0xff, 0xff, 0x7f, 0x90, 0x90, 0xeb};
BYTE PATC_WN6x_Query__CDefPolicy[]	= {0xc7, 0x87, 0x3c, 0x06, 0x00, 0x00, 0xff, 0xff, 0xff, 0x7f, 0x90, 0x90};
BYTE PATC_WN81_Query__CDefPolicy[]	= {0xc7, 0x81, 0x3c, 0x06, 0x00, 0x00, 0xff, 0xff, 0xff, 0x7f, 0x90, 0x90};
#elif defined _M_IX86
BYTE PTRN_WN60_Query__CDefPolicy[]	= {0x3b, 0x91, 0x20, 0x03, 0x00, 0x00, 0x5e, 0x0f, 0x84};
BYTE PTRN_WN6x_Query__CDefPolicy[]	= {0x3b, 0x86, 0x20, 0x03, 0x00, 0x00, 0x0f, 0x84};
BYTE PTRN_WN6x_Query__CDefPolicy1[]	= {0x85, 0xc0, 0x74, 0x09, 0x38, 0x5d, 0xfa};
BYTE PTRN_WN6x_Query__CDefPolicy2[]	= {0x74, 0x2f, 0x68, 0x88, 0x62};
BYTE PTRN_WN81_Query__CDefPolicy[]	= {0x3b, 0x81, 0x20, 0x03, 0x00, 0x00, 0x0f, 0x84};
BYTE PATC_WN60_Query__CDefPolicy[]	= {0xc7, 0x81, 0x20, 0x03, 0x00, 0x00, 0xff, 0xff, 0xff, 0x7f, 0x5e, 0x90, 0x90};
//BYTE PATC_WN6x_Query__CDefPolicy[]	= {0xc7, 0x86, 0x20, 0x03, 0x00, 0x00, 0xff, 0xff, 0xff, 0x7f, 0x90, 0x90};
BYTE PATC_WN6x_Query__CDefPolicy[]	= {0xb8, 0x00, 0x01, 0x00, 0x00, 0x90, 0x89, 0x86, 0x20, 0x03, 0x00 };
BYTE PATC_WN6x_Query__CDefPolicy1[]	= {0x90}; //offset -20
BYTE PATC_WN6x_Query__CDefPolicy2[]	= {0xe9, 0x2c, 0x00, 0x00, 0x00}; //offset 0
BYTE PATC_WN81_Query__CDefPolicy[]	= {0xc7, 0x81, 0x20, 0x03, 0x00, 0x00, 0xff, 0xff, 0xff, 0x7f, 0x90, 0x90};
#endif
BYTE PTRN_WIN5_TestLicence[]		= {0x83, 0xf8, 0x02, 0x7f};
BYTE PATC_WIN5_TestLicence[]		= {0x90, 0x90};

BYTE PTRN_WIN5_CURR1[]				= {0x83, 0x78, 0x24, 0x00, 0x74, 0x04, 0x33, 0xc0, 0xeb, 0x2c};
BYTE PTRN_WIN5_CURR2[]				= {0x3b, 0x46, 0x0c, 0x7f, 0x16};

BYTE PTRN_WIN5_CURR3[]				= {0xe8, 0x63, 0x5e, 0x02, 0x00};
BYTE PTRN_WIN5_CURR4[]				= {0xf7, 0xd8, 0x1b, 0xc0, 0x40, 0xa3};
BYTE PTRN_WIN5_CURR5[]				= {0x8b, 0xff, 0x55, 0x8b, 0xec, 0x8b, 0x45, 0x08, 0x57, 0x33, 0xff};
BYTE PTRN_WIN5_CURR6[]				= {0x8b, 0xff, 0x55, 0x8b, 0xec, 0x56, 0x57, 0x8b, 0xf1}; //нужно два раза, и замена из PATC_WIN5_NEW5
BYTE PTRN_WIN5_CURR7[]				= {0x8b, 0xff, 0x55, 0x8b, 0xec, 0x8b, 0x45, 0x08, 0x8b, 0x00, 0x8b, 0x80, 0x08, 0x34, 0x00, 0x00, 0x83, 0x78, 0x24, 0x00}; //замена из PATC_WIN5_NEW5
BYTE PTRN_WIN5_CURR8[]				= {0x8b, 0xff, 0x55, 0x8b, 0xec, 0x56, 0x8d, 0x71, 0x0c}; //замена из PATC_WIN5_NEW5
BYTE PTRN_WIN5_CURR9[]				= {0x8b, 0xff, 0x55, 0x8b, 0xec, 0x51, 0x53, 0x56, 0x57, 0xbf, 0x9c}; //замена из PATC_WIN5_NEW5
BYTE PTRN_WIN5_CURR10[]				= {0x57, 0x6a, 0x47, 0x59, 0x6a, 0x01};

BYTE PATC_WIN5_NEW1[]				= {0x75}; //offset 4
BYTE PATC_WIN5_NEW2[]				= {0x90, 0x90}; //offset 3
BYTE PATC_WIN5_NEW3[]				= {0x32, 0xc0, 0x40, 0x90, 0x90}; //offset 0
BYTE PATC_WIN5_NEW4[]				= {0x33}; //offset 2
BYTE PATC_WIN5_NEW5[]				= {0x33, 0xc0, 0xc2, 0x04, 0x00}; //offset 0
BYTE PATC_WIN5_NEW10[]				= {0x33, 0xc0, 0xc3}; //offset -14


KULL_M_PATCH_GENERIC TermSrvMultiRdpReferences[] = {
//	{KULL_M_WIN_BUILD_XP,		{sizeof(PTRN_WIN5_TestLicence),			PTRN_WIN5_TestLicence},			{sizeof(PATC_WIN5_TestLicence),			PATC_WIN5_TestLicence},			{3}},
//	{KULL_M_WIN_BUILD_XP,		{sizeof(PTRN_WIN5_CURR1),				PTRN_WIN5_CURR1},				{sizeof(PATC_WIN5_NEW1),				PATC_WIN5_NEW1},				{4}},
//	{KULL_M_WIN_BUILD_XP,		{sizeof(PTRN_WIN5_CURR2),				PTRN_WIN5_CURR2},				{sizeof(PATC_WIN5_NEW2),				PATC_WIN5_NEW2},				{3}},
	{KULL_M_WIN_BUILD_XP,		{sizeof(PTRN_WIN5_CURR3),				PTRN_WIN5_CURR3},				{sizeof(PATC_WIN5_NEW3),				PATC_WIN5_NEW3},				{0}},
	{KULL_M_WIN_BUILD_XP,		{sizeof(PTRN_WIN5_CURR4),				PTRN_WIN5_CURR4},				{sizeof(PATC_WIN5_NEW4),				PATC_WIN5_NEW4},				{2}},
	{KULL_M_WIN_BUILD_XP,		{sizeof(PTRN_WIN5_CURR5),				PTRN_WIN5_CURR5},				{sizeof(PATC_WIN5_NEW5),				PATC_WIN5_NEW5},				{0}},
	{KULL_M_WIN_BUILD_XP,		{sizeof(PTRN_WIN5_CURR6),				PTRN_WIN5_CURR6},				{sizeof(PATC_WIN5_NEW5),				PATC_WIN5_NEW5},				{0}},
	{KULL_M_WIN_BUILD_XP,		{sizeof(PTRN_WIN5_CURR7),				PTRN_WIN5_CURR7},				{sizeof(PATC_WIN5_NEW5),				PATC_WIN5_NEW5},				{0}},
	{KULL_M_WIN_BUILD_XP,		{sizeof(PTRN_WIN5_CURR8),				PTRN_WIN5_CURR8},				{sizeof(PATC_WIN5_NEW5),				PATC_WIN5_NEW5},				{0}},
	{KULL_M_WIN_BUILD_XP,		{sizeof(PTRN_WIN5_CURR9),				PTRN_WIN5_CURR9},				{sizeof(PATC_WIN5_NEW5),				PATC_WIN5_NEW5},				{0}},
	{KULL_M_WIN_BUILD_XP,		{sizeof(PTRN_WIN5_CURR10),				PTRN_WIN5_CURR10},				{sizeof(PATC_WIN5_NEW10),				PATC_WIN5_NEW10},				{-14}},
	
	{KULL_M_WIN_BUILD_VISTA,	{sizeof(PTRN_WN60_Query__CDefPolicy),	PTRN_WN60_Query__CDefPolicy},	{sizeof(PATC_WN60_Query__CDefPolicy),	PATC_WN60_Query__CDefPolicy},	{0}},
	{KULL_M_WIN_BUILD_7,		{sizeof(PTRN_WN6x_Query__CDefPolicy),	PTRN_WN6x_Query__CDefPolicy},	{sizeof(PATC_WN6x_Query__CDefPolicy),	PATC_WN6x_Query__CDefPolicy},	{0}},
	{KULL_M_WIN_BUILD_7,		{sizeof(PTRN_WN6x_Query__CDefPolicy1),	PTRN_WN6x_Query__CDefPolicy1},	{sizeof(PATC_WN6x_Query__CDefPolicy1),	PATC_WN6x_Query__CDefPolicy1},	{-20}},
	{KULL_M_WIN_BUILD_7,		{sizeof(PTRN_WN6x_Query__CDefPolicy2),	PTRN_WN6x_Query__CDefPolicy2},	{sizeof(PATC_WN6x_Query__CDefPolicy2),	PATC_WN6x_Query__CDefPolicy2},	{0}},
	{KULL_M_WIN_BUILD_BLUE,		{sizeof(PTRN_WN81_Query__CDefPolicy),	PTRN_WN81_Query__CDefPolicy},	{sizeof(PATC_WN81_Query__CDefPolicy),	PATC_WN81_Query__CDefPolicy},	{0}},
	{100000},
};
NTSTATUS kuhl_m_ts_multirdp(int argc, wchar_t * argv[])
{
	if( kull_m_patch_genericProcessOrServiceFromBuild(TermSrvMultiRdpReferences, sizeof(TermSrvMultiRdpReferences) / sizeof(KULL_M_PATCH_GENERIC), _WCS_("TermService"), _WCS_("termsrv.dll"), TRUE) ) 
		return STATUS_SUCCESS;
	return (NTSTATUS)1;
}

BYTE PTRN_XP_CSRSRV[] = { 0x51, 0x57, 0xff, 0xd0, 0x39, 0x9d, 0x24 };
BYTE PATC_XP_CSRSRV[] = { 0x90, 0x90 }; //offset 2

KULL_M_PATCH_GENERIC CsrsrvRef[] =
{
	{ KULL_M_WIN_BUILD_XP, { sizeof(PTRN_XP_CSRSRV), PTRN_XP_CSRSRV }, { sizeof(PATC_XP_CSRSRV), PATC_XP_CSRSRV }, {2} },
	{10000}
};

BYTE PTRN_XP_MSGINA[] = { 0x6a, 0x00, 0xe8, 0x9d, 0x16, 0x01, 0x00 };
BYTE PATC_XP_MSGINA[] = { 0xeb, 0x05 }; //offeset 0

KULL_M_PATCH_GENERIC MsginaRef[] =
{
	{ KULL_M_WIN_BUILD_XP, { sizeof(PTRN_XP_MSGINA), PTRN_XP_MSGINA }, { sizeof(PATC_XP_MSGINA), PATC_XP_MSGINA }, {0} },
	{10000}
};

BYTE PTRN_XP_WINLOGON1[] = { 0xff, 0x75, 0x08, 0xe8, 0x12, 0x70, 0x00, 0x00 };
BYTE PTRN_XP_WINLOGON2[] = { 0x8b, 0x4d, 0xfc, 0xe8, 0x5d, 0x33, 0x01, 0x00 };

BYTE PATC_XP_WINLOGON[] = { 0x33, 0xc0, 0xc3 }; //для PTRN_XP_WINLOGON1 offset 40, для PTRN_XP_WINLOGON2 offset 12

KULL_M_PATCH_GENERIC WinlogonRef[] =
{
	{ KULL_M_WIN_BUILD_XP, { sizeof(PTRN_XP_WINLOGON1), PTRN_XP_WINLOGON1 }, { sizeof(PATC_XP_WINLOGON), PATC_XP_WINLOGON }, {40} },
	{ KULL_M_WIN_BUILD_XP, { sizeof(PTRN_XP_WINLOGON2), PTRN_XP_WINLOGON2 }, { sizeof(PATC_XP_WINLOGON), PATC_XP_WINLOGON }, {12} },
	{10000}
};


NTSTATUS kuhl_m_ts_csrsrv( DWORD pid )
{
	if( kull_m_patch_genericProcessOrServiceFromBuild(CsrsrvRef, sizeof(CsrsrvRef) / sizeof(KULL_M_PATCH_GENERIC), (wchar_t*)pid, _WCS_("csrsrv.dll"), FALSE) ) 
		return STATUS_SUCCESS;
	return (NTSTATUS)1;
}

NTSTATUS kuhl_m_ts_winlogon( DWORD pid )
{
	if( kull_m_patch_genericProcessOrServiceFromBuild(WinlogonRef, sizeof(WinlogonRef) / sizeof(KULL_M_PATCH_GENERIC), (wchar_t*)pid, _WCS_("winlogon.exe"), FALSE) ) 
		return STATUS_SUCCESS;
	return (NTSTATUS)1;
}

NTSTATUS kuhl_m_ts_msgina( DWORD pid )
{
	if( kull_m_patch_genericProcessOrServiceFromBuild(MsginaRef, sizeof(MsginaRef) / sizeof(KULL_M_PATCH_GENERIC), (wchar_t*)pid, _WCS_("msgina.dll"), FALSE) ) 
		return STATUS_SUCCESS;
	return (NTSTATUS)1;
}
