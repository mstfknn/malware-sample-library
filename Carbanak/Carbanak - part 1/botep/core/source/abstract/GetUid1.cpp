#include "core\abstract.h"
#include "Iphlpapi.h"

namespace Abstract
{

bool _GetUid1( StringBuilder& uid )
{
	PIP_ADAPTER_INFO adapters, adapter;
	ULONG lb = 0;
	API(IPHLPAPI, GetAdaptersInfo)( 0, &lb );
	adapters = (PIP_ADAPTER_INFO) Mem::Alloc(lb);
	bool ret = false;
	if( API(IPHLPAPI, GetAdaptersInfo)( adapters, &lb ) == NO_ERROR )
	{
		adapter = adapters;
		while( adapter )
		{
			if( adapter->Type == MIB_IF_TYPE_ETHERNET )
			{
				uint a1 = *((uint*) adapter->Address);
				uint a2 = *((uint*) (adapter->Address + 2));
				uint a = (a1 ^ a2) | 0x80000000;
				char format[3]; format[0] = '%'; format[1] = 'x'; format[2] = 0;
				uid.Format( format, a );
				ret = true;
				break;
			}
			adapter = adapter->Next;
		}
	}
	Mem::Free(adapters);
	return ret;
}

}
