#include "WndRec\stream.h"
#include "core\file.h"
#include "core\debug.h"

namespace WndRec
{

//используется для отсылки данных при получении команды ID_STREAM_RECREATE
#pragma pack(1)
struct InfoStreamRecreate
{
	uint idOld;
	uint idNew;
	byte data[256]; //сохраненная информация о потоке при его создании, для отсылки при пересоздании
};
#pragma pack()

struct InfoStream
{
	uint id; //первый раз полученный ид
	uint idReal; //ид который получен после разрыва связи с сервером (когда сервер вырубили), получаем при получении команды пересоздания потока
	InfoStreamRecreate isr; //для оптимизации по выделению памяти, используем структуру, что ее отправлять при получении команды ID_STREAM_RECREATE
	int c_data;
};

static InfoStream* streams = 0;
int c_streams, max_streams;
CRITICAL_SECTION lockStreams;

bool InitStreams()
{
	CriticalSection::Init(lockStreams);
	c_streams = 0;
	max_streams = 4;
	streams = (InfoStream*)Mem::Alloc( sizeof(InfoStream) * max_streams );
	if( streams == 0 ) return false;
	return true;
}

static InfoStream* GetStream( uint id )
{
	InfoStream* ret = nullptr;
	if( id == 0 ) //создается новый поток
	{
		if( c_streams >= max_streams )
		{
			InfoStream* streams2 = (InfoStream*)Mem::Realloc( streams, (max_streams + 4) * sizeof(InfoStream) );
			if( streams2 )
			{
				max_streams += 4;
				streams = streams2;
			}
			else
				return nullptr;
		}
		ret = &streams[c_streams];
		Mem::Set( ret, 0, sizeof(InfoStream) );
		c_streams++;
	}
	else
	{
		if( streams )
		{
			for( int i = 0; i < c_streams; i++ )
				if( streams[i].id == id )
				{
					ret = &streams[i];
					break;
				}
		}
	}
	return ret;
}

static uint CreateServerStream( ServerData* server, uint typeId, const char* typeName, const char* fileName, const char* ext, int wait )
{
	InfoStream* stream = GetStream(0);
	if( stream == 0 ) return 0;
	byte* data = stream->isr.data;
	int i = 0;
	data[i++] = typeId;
	int c_typeName = Str::Len(typeName);
	int c_fileName = Str::Len(fileName);
	int c_ext = Str::Len(ext);
	if( typeId == STREAM_FILE || typeId == STREAM_PIPE )
	{
		if( c_typeName > sizeof(stream->isr.data) / 2 ) c_typeName = sizeof(stream->isr.data) / 2;
		data[i++] = c_typeName;
		Mem::Copy( data + i, typeName, c_typeName );
		i += c_typeName;
	}
	if( i + c_fileName + 2 > sizeof(stream->isr.data) ) c_fileName = sizeof(stream->isr.data) - i - 2; //2 - с учетом отсылки расширения
	data[i++] = c_fileName;
	Mem::Copy( data + i, fileName, c_fileName );
	i += c_fileName;
	if( typeId == STREAM_FILE )
	{
		if( i + c_ext + 1 > sizeof(stream->isr.data) ) c_ext = sizeof(stream->isr.data) - i - 1;
		data[i++] = c_ext;
		Mem::Copy( data + i, ext, c_ext );
		i += c_ext;
	}
	stream->c_data = i;
	byte answer[32];
	int res = GetData2( server, ID_STREAM_CREATE, data, i, answer, sizeof(answer), 4, wait );
	if( res > 0 )
	{
		stream->id = stream->idReal = *((uint*)answer);
	}
	return stream->id;
}

static uint CreateFileStream( const char* fileName )
{
	return (uint)File::Open( fileName, GENERIC_WRITE, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH );
}

uint CreateStream( ServerData* server, uint typeId, const char* typeName, const char* fileName, const char* ext, int wait )
{
	if( server )
	{
		CriticalSection lock(lockStreams);
		return CreateServerStream( server, typeId, typeName, fileName, ext, wait );
	}
	else
		return CreateFileStream(fileName);
}

uint RecreateStream( ServerData* server, uint idOld, uint idNew )
{
	if( server == 0 ) return 0;
	CriticalSection lock(lockStreams);
	for( int i = 0; i < c_streams; i++ )
	{
		if( streams[i].idReal == idOld )
		{
			streams[i].isr.idOld = idOld;
			streams[i].isr.idNew = idNew;
			WriteData( server, ID_STREAM_RECREATE, &streams[i].isr, streams[i].c_data + 8 );
			streams[i].idReal = idNew;
			return idNew;
		}
	}
	return 0;
}

void WriteStream( ServerData* server, uint id, const void* data, int c_data )
{
	if( id == 0 ) return;
	if( server )
	{
		CriticalSection lock(lockStreams);
		InfoStream* stream = GetStream(id);
		uint idReal = stream ? stream->idReal : id;
		WriteData( server, ID_STREAM_DATA, data, c_data, 0, idReal );
	}
	else
		File::Write( (HANDLE)id, data, c_data );
}

void CloseStream( ServerData* server, uint id )
{
	if( id == 0 ) return;
	if( server )
	{
		CriticalSection lock(lockStreams);
		InfoStream* stream = GetStream(id);
		if( stream )
		{
			WriteData( server, ID_STREAM_CLOSE, 0, 0, 0, stream->idReal );
		}
	}
	else
		File::Close( (HANDLE)id );
}

}
