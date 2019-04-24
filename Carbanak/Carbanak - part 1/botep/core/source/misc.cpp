#include "core\core.h"
#include "core\misc.h"
#include "core\debug.h"
#include "Iphlpapi.h"

uint CalcHash( const byte* ptr, int c_ptr )
{
	uint hash = 0;
	if( ptr && c_ptr > 0 )
	{
		for( int i = 0; i < c_ptr; i++, ptr++ )
		{
			hash = (hash << 4) + *ptr;
			unsigned t;
			if( (t = hash & 0xf0000000) != 0)
				hash = ((hash ^ (t >> 24)) & (0x0fffffff));
		}
	}
	return hash;
}

int GetMacAddress( byte* mac )
{
	PIP_ADAPTER_INFO adapters, adapter;
	ULONG lb = 0;
	API(IPHLPAPI, GetAdaptersInfo)( 0, &lb );
	adapters = (PIP_ADAPTER_INFO) Mem::Alloc(lb);
	int ret = 0;
	if( API(IPHLPAPI, GetAdaptersInfo)( adapters, &lb ) == NO_ERROR )
	{
		bool stop = false;
		int typeAdapter = MIB_IF_TYPE_ETHERNET;
		while( !stop )
		{
			adapter = adapters;
			while( adapter )
			{
				if( adapter->Type == typeAdapter || typeAdapter == 0 )
				{
					ret = sizeof(adapter->Address);
					Mem::Copy( mac, adapter->Address, ret );
					stop = true;
					break;
				}
				adapter = adapter->Next;
			}
			switch( typeAdapter )
			{
				case MIB_IF_TYPE_ETHERNET: typeAdapter = IF_TYPE_IEEE80211; break;
				case IF_TYPE_IEEE80211: typeAdapter = 0; break;
			}
		}
	}
	Mem::Free(adapters);
	return ret;
}

bool RunThread( typeFuncThread func, void* data )
{
	HANDLE thread = RunThread( func, data, nullptr );
	if( thread )
	{
		API(KERNEL32, CloseHandle)(thread);
		return true;
	}
	return false;
}

HANDLE RunThread( typeFuncThread func, void* data, DWORD* threadId )
{
    return API(KERNEL32, CreateThread)( NULL, NULL, (LPTHREAD_START_ROUTINE)func, data, NULL, threadId );
}

void Delay( uint v )
{
	API(KERNEL32, Sleep)(v);
}

void CriticalSection::Init( CRITICAL_SECTION& cs )
{
	API(KERNEL32, InitializeCriticalSection)(&cs);
}

void CriticalSection::Enter( CRITICAL_SECTION& cs )
{
	API(KERNEL32, EnterCriticalSection)(&cs);
}

void CriticalSection::Leave( CRITICAL_SECTION& cs )
{
	API(KERNEL32, LeaveCriticalSection)(&cs);
}

bool ExtractIpPort( const char* s, char* ip, int& port )
{
	int p = Str::IndexOf( s, ':' );
	if( p < 7 ) return false;//минимальный размер ip адреса (1.1.1.1)
	Str::Copy( ip, s, p );
	port = Str::ToInt( s + p + 1 );
	if( port <= 0 ) return false;
	return true;
}
