#include "core\version.h"
#include "core\debug.h"

char* NT_VERSION_NAMES[] = 
{
	_CT_("WinXPSP0"),
	_CT_("WinXPSP1"),
	_CT_("WinXPSP2"),
	_CT_("WinXPSP3"),
	_CT_("Win2003SP0"),
	_CT_("Win2003SP1"),
	_CT_("Win2003SP2"),
	_CT_("Win2003SP3"),
	_CT_("WinVistaSP0"),
	_CT_("WinVistaSP1"),
	_CT_("WinVistaSP2"),
	_CT_("WinVistaSP3"),
	_CT_("Win2008SP0"),
	_CT_("Win2008SP1"),
	_CT_("Win2008SP2"),
	_CT_("Win2008R2SP0"),
	_CT_("Win2008R2SP1"),
	_CT_("Win2008R2SP2"),
	_CT_("Win7SP0"),
	_CT_("Win7SP1"),
	_CT_("Win7SP2"),
	_CT_("Win7SP3"),
	_CT_("Win8SP0"),
	_CT_("Win8SP1"),
	_CT_("Win8SP2"),
	_CT_("Win8SP3"),
	_CT_("Win2012SP0"),
	_CT_("Win2012SP1"),
	_CT_("Win2012SP2"),
	_CT_("Win2012SP3"),
	_CT_("Win81SP0"),
	_CT_("Win81SP1"),
	_CT_("Win81SP2"),
	_CT_("Win81SP3"),
	_CT_("Win2012R2SP0"),
	_CT_("Win2012R2SP1"),
	_CT_("Win2012R2SP2"),
	_CT_("Win2012R2SP3"),
	_CT_("WinHigh")
};

bool GetWindowsVersion( WindowsVersion& wv )
{
	wv.idVer = WIN_VERSION_HIGH;
	API(KERNEL32, GetSystemInfo)( &wv.systemInfo );

	BOOL wow64;
	HANDLE hprocess = API(KERNEL32, GetCurrentProcess)();
	API(KERNEL32, IsWow64Process)( hprocess, &wow64 );

	if( wow64 || wv.systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 )
		wv.architecture = Sys64bit;
	else
		wv.architecture = Sys32bit;

	OSVERSIONINFOEXA osvi;
	Mem::Zero(osvi);
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
	if( !API(KERNEL32, GetVersionExA)( (LPOSVERSIONINFOA)&osvi ) )
	{
		return false;
	}

	if( osvi.dwMajorVersion == 6 )
	{
		if( osvi.dwMinorVersion == 0 )
		{

			if( osvi.wProductType == VER_NT_WORKSTATION )
			{
				switch( osvi.wServicePackMajor )
				{
					case 0:	wv.idVer = MICROSOFT_WINDOWS_VISTA_SP0; break;
					case 1:	wv.idVer = MICROSOFT_WINDOWS_VISTA_SP1; break;
					case 2:	wv.idVer = MICROSOFT_WINDOWS_VISTA_SP2; break;
					case 3:	wv.idVer = MICROSOFT_WINDOWS_VISTA_SP3; break;
				}
			}
			else
			{
				switch( osvi.wServicePackMajor )
				{
					case 0:	wv.idVer = MICROSOFT_WINDOWS_2008_SP0; break;
					case 1:	wv.idVer = MICROSOFT_WINDOWS_2008_SP1; break;
					case 2:	wv.idVer = MICROSOFT_WINDOWS_2008_SP2; break;
				}
			}
		}
		else if( osvi.dwMinorVersion == 1 )
		{
			if( osvi.wProductType == VER_NT_WORKSTATION )
			{
				switch( osvi.wServicePackMajor )
				{
					case 0:	wv.idVer = MICROSOFT_WINDOWS_7_SP0; break;
					case 1:	wv.idVer = MICROSOFT_WINDOWS_7_SP1; break;
					case 2:	wv.idVer = MICROSOFT_WINDOWS_7_SP2; break;
				}
			}
			else 
			{ //x64
				switch( osvi.wServicePackMajor )
				{
					case 0:	wv.idVer = MICROSOFT_WINDOWS_2008R2_SP0; break;
					case 1:	wv.idVer = MICROSOFT_WINDOWS_2008R2_SP1; break;
					case 2:	wv.idVer = MICROSOFT_WINDOWS_2008R2_SP2; break;
				}
			}
		}
		else if( osvi.dwMinorVersion == 2 )
		{
			if( osvi.wProductType == VER_NT_WORKSTATION )
			{
				switch( osvi.wServicePackMajor )
				{
					case 0:	wv.idVer = MICROSOFT_WINDOWS_8_SP0; break;
					case 1:	wv.idVer = MICROSOFT_WINDOWS_8_SP1; break;
					case 2:	wv.idVer = MICROSOFT_WINDOWS_8_SP2; break;
				}
			}
			else 
			{//x64
				switch( osvi.wServicePackMajor )
				{
					case 0:	wv.idVer = MICROSOFT_WINDOWS_2012_SP0; break;
					case 1:	wv.idVer = MICROSOFT_WINDOWS_2012_SP1; break;
					case 2:	wv.idVer = MICROSOFT_WINDOWS_2012_SP2; break;
				}
			}
		}
		else if( osvi.dwMinorVersion == 3 )
		{
			if( osvi.wProductType == VER_NT_WORKSTATION )
			{
				switch( osvi.wServicePackMajor )
				{
					case 0:	wv.idVer = MICROSOFT_WINDOWS_81_SP0; break;
					case 1:	wv.idVer = MICROSOFT_WINDOWS_81_SP1; break;
					case 2:	wv.idVer = MICROSOFT_WINDOWS_81_SP2; break;
				}
			}
			else 
			{//x64
				switch( osvi.wServicePackMajor )
				{
					case 0:	wv.idVer = MICROSOFT_WINDOWS_2012R2_SP0; break;
					case 1:	wv.idVer = MICROSOFT_WINDOWS_2012R2_SP1; break;
					case 2:	wv.idVer = MICROSOFT_WINDOWS_2012R2_SP2; break;
				}
			}
		}
	}
	else if( osvi.dwMajorVersion == 5 )
	{
		if( osvi.dwMinorVersion == 1 || (osvi.wProductType == VER_NT_WORKSTATION && osvi.dwMinorVersion == 2 && wv.architecture == Sys64bit) )
		{
			switch (osvi.wServicePackMajor)
			{
				case 0:	wv.idVer = MICROSOFT_WINDOWS_XP0; break;
				case 1:	wv.idVer = MICROSOFT_WINDOWS_XP1; break;
				case 2:	wv.idVer = MICROSOFT_WINDOWS_XP2; break;
				case 3:	wv.idVer = MICROSOFT_WINDOWS_XP3; break;
			}
		}
		else if( osvi.dwMinorVersion == 2 )
		{
			switch( osvi.wServicePackMajor )
			{
				case 0:	wv.idVer = MICROSOFT_WINDOWS_2003_SP0; break;
				case 1:	wv.idVer = MICROSOFT_WINDOWS_2003_SP1; break;
				case 2:	wv.idVer = MICROSOFT_WINDOWS_2003_SP2; break;
				case 3:	wv.idVer = MICROSOFT_WINDOWS_2003_SP3; break;
			}
		}
	}
	return true;
}

NT_VERSION GetNTVersion( bool* x64 )
{
	WindowsVersion wv;
	GetWindowsVersion(wv);
	if( x64 )
	{
		*x64 = (wv.architecture == Sys64bit) ? true : false;
	}
	return wv.idVer;
}

NT_VERSION GetNTVersion( StringBuilder& s )
{
	bool x64;
	NT_VERSION id = GetNTVersion(&x64);
	s = DECODE_STRING( NT_VERSION_NAMES[id] );
	if( x64 ) s += _CS_(".x64");
	return id;
}
