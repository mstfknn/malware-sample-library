#include "core\reestr.h"
#include "core\debug.h"

Reestr::Reestr( HKEY root ) : key(root)
{
}

Reestr::Reestr( HKEY root, const char* keyName, uint options )
{
	if( !Open( root, keyName, options ) )
		key = 0;
}

Reestr::~Reestr()
{
	Close();
}

bool Reestr::Open( HKEY root, const char* keyName, uint options  )
{
	if( API(ADVAPI32, RegOpenKeyExA)( root, keyName, 0, options, &key ) == ERROR_SUCCESS )
		return true;
	return false;
}

bool Reestr::Create( HKEY root, const char* keyName, uint options )
{
	LSTATUS res = API(ADVAPI32, RegCreateKeyExA)( root, keyName, 0, 0, REG_OPTION_NON_VOLATILE, options, 0, &key, 0 );
	if( res == ERROR_SUCCESS )
		return true;
	return false;
}

bool Reestr::Enum( StringBuilder& subKey, int index ) const
{
	DWORD size = subKey.Size();
	LSTATUS res = API(ADVAPI32, RegEnumKeyExA)( key, index, subKey.c_str(), &size, 0, 0, 0, 0 );
	if( res == ERROR_MORE_DATA )
		if( subKey.Grow( size - subKey.Size() )  )
		{
			size = subKey.Size();
			res = API(ADVAPI32, RegEnumKeyExA)( key, index, subKey.c_str(), &size, 0, 0, 0, 0 );
		}
	if( res == ERROR_SUCCESS )
	{
		subKey.SetLen(size);
		return true;
	}
	return false;
}

void Reestr::Close()
{
	API(ADVAPI32, RegCloseKey)(key);
	key = 0;
}

bool Reestr::GetString( const char* valueName, StringBuilder& value ) const
{
	DWORD size = value.Size();
	DWORD type = REG_EXPAND_SZ;
	LSTATUS res = API(ADVAPI32, RegQueryValueExA)( key, valueName, 0, &type, (BYTE*)value.c_str(), &size );
	if( res == ERROR_MORE_DATA || value.c_str() == 0 )
	{
		if( value.Grow( size - value.Size() ) )
			res = API(ADVAPI32, RegQueryValueExA)( key, valueName, 0, &type, (BYTE*)value.c_str(), &size );
	}
	if( res == ERROR_SUCCESS )
	{
		value.SetLen(size - 1); 
		return true;
	}
	return false;
}

bool Reestr::GetData( const char* valueName, Mem::Data& data, DWORD type ) const
{
	DWORD size = data.Size();
	LSTATUS res = API(ADVAPI32, RegQueryValueExA)( key, valueName, 0, &type, (BYTE*)data.Ptr(), &size );
	if( res == ERROR_MORE_DATA || data.Ptr() == 0 )
	{
		if( data.MakeEnough(size) )
			res = API(ADVAPI32, RegQueryValueExA)( key, valueName, 0, &type, (BYTE*)data.Ptr(), &size );
	}
	if( res == ERROR_SUCCESS )
	{
		data.SetLen(size);
		return true;
	}
	return false;
}

bool Reestr::SetData( const char* valueName, const Mem::Data& data, DWORD type ) const
{
	LSTATUS res = API(ADVAPI32, RegSetValueExA)( key, valueName, 0, type, (BYTE*)data.Ptr(), data.Len() );
	if( res == ERROR_SUCCESS )
		return true;
	return false;
}

bool Reestr::SetDWORD( const char* valueName, DWORD value ) const
{
	LSTATUS res = API(ADVAPI32, RegSetValueExA)( key, valueName, 0, REG_DWORD, (BYTE*)&value, sizeof(value) );
	if( res == ERROR_SUCCESS )
		return true;
	return false;
}

bool Reestr::DelValue( const char* valueName ) const
{
	LSTATUS res = API(ADVAPI32, RegDeleteValueA)( key, valueName );
	if( res == ERROR_SUCCESS )
		return true;
	return false;
}

bool Reestr::SetString( const char* valueName, const char* s, int c_s ) const
{
	if( c_s < 0 ) c_s = Str::Len(s);
	LSTATUS res = API(ADVAPI32, RegSetValueExA)( key, valueName, 0, REG_EXPAND_SZ, (BYTE*)s, c_s );
	if( res == ERROR_SUCCESS )
		return true;
	return false;
}
