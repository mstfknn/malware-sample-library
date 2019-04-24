#pragma once

#include "core\core.h"
#include "core\socket.h"

namespace WndRec
{

class HTTPS
{
		HINTERNET inet;
		HINTERNET connect;
		HINTERNET request;
		CRITICAL_SECTION cs;
		Mem::Data data;

		static DWORD WINAPI ThreadReader( void* v );

	public:

		HTTPS();
		~HTTPS()
		{
			Close();
		}
		void Close();
		bool Connect( const char* ip );
		int Write( Mem::Data& data );
		int Read( Mem::Data& _data, int maxRead, int wait );
		bool Connected()
		{
			return request != nullptr;
		}

};


}
