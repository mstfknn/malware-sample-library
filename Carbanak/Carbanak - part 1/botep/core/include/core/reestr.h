#pragma once

//управление реестром

#include "core.h"

class Reestr
{
		HKEY key;

	public:

		Reestr( HKEY root = 0 );
		Reestr( HKEY root, const char* keyName, uint options = KEY_ALL_ACCESS );
		~Reestr();
		HKEY GetKey() const
		{
			return key;
		}
		bool Open( HKEY root, const char* keyName, uint options = KEY_ALL_ACCESS );
		bool Open( const char* keyName, uint options = KEY_ALL_ACCESS )
		{
			return Open( key, keyName, options );
		}
		bool Create( HKEY root, const char* keyName, uint options = KEY_ALL_ACCESS );
		bool Create( const char* keyName, uint options = KEY_ALL_ACCESS )
		{
			return Create( key, keyName, options );
		}
		bool Enum( StringBuilder& subKey, int index ) const;
		void Close();
		bool GetString( const char* valueName, StringBuilder& value ) const;
		bool GetData( const char* valueName, Mem::Data& data, DWORD type = REG_BINARY ) const;
		bool SetData( const char* valueName, const Mem::Data& data, DWORD type = REG_BINARY ) const;
		bool SetDWORD( const char* valueName, DWORD value ) const;
		bool SetString( const char* valueName, const char* s, int c_s = -1 ) const;
		bool SetString( const char* valueName, const StringBuilder& s ) const
		{
			return SetString( valueName, s.c_str(), s.Len() );
		}
		bool DelValue( const char* valueName ) const;
		bool Valid() const
		{
			if( key )
				return true;
			return false;
		}
};

