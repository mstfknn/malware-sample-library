#include "core\file.h"
#include "core\debug.h"

namespace File
{

HANDLE Open( const char* fileName, DWORD access, DWORD creation, DWORD flags )
{
	HANDLE file = API(KERNEL32, CreateFileA)( fileName, access, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, creation, flags, 0 );
    if( file == INVALID_HANDLE_VALUE ) return 0;
	return file;
}

bool Write( HANDLE file, const void* data, int c_data )
{
	DWORD writed = 0;
	if( API(KERNEL32, WriteFile)( file, data, c_data, &writed, 0 ) )
	{
		if( writed == c_data )
			return true;
	}
	return false;
}

uint Read( HANDLE file, void* data, int c_data )
{
	DWORD readed = 0;
	if( API(KERNEL32, ReadFile)( file, data, c_data, &readed, 0 ) )
	{
		return readed;
	}
	return 0;
}

void Close( HANDLE file )
{
	API(KERNEL32, CloseHandle)(file);
}

bool ReadAll( const char* fileName, Mem::Data& data )
{
	bool ret = false;
	HANDLE file = Open( fileName, GENERIC_READ, OPEN_EXISTING );
	if( file ) 
	{
		int size = API(KERNEL32, GetFileSize)( file, 0 );
		if( data.MakeEnough(size + 1) ) //выделяем на 1 байт больше, так как считанные данные могут быть представлены в виде строки
		{
			DWORD readed = 0;
			if( API(KERNEL32, ReadFile)( file, data.PtrWrite(), size, &readed, 0 ) )
			{
				if( readed == size )
				{
					data.AddLen(size);
					ret = true;
				}
			}
		}
		Close(file);
	}
	return ret;
}

bool WriteAll( const char* fileName, const void* data, int c_data )
{
	bool ret = false;
	HANDLE file = Open( fileName, GENERIC_WRITE, CREATE_ALWAYS );
	if( file )
	{
		DWORD writed = 0;
		if( API(KERNEL32, WriteFile)( file, data, c_data, &writed, 0 ) )
		{
			if( writed == c_data )
				ret = true;
		}
		Close(file);
	}
	return ret;
}

bool Append( const char* fileName, const void* data, int c_data )
{
	bool ret = false;
	HANDLE file = File::Open( fileName, GENERIC_WRITE, OPEN_ALWAYS );
	if( file )
	{
		API(KERNEL32, SetFilePointer)( file, 0, 0, FILE_END );
		ret = Write( file, data, c_data );
		Close(file);
	}
	return ret;
}

bool Append( const char* fileName, const char* s, int c_s )
{
	if( c_s < 0 ) c_s = Str::Len(s);
	return Append( fileName, (void*)s, c_s );
}

int Size( const char* fileName )
{
	int ret = -1;
	HANDLE file = Open( fileName, GENERIC_WRITE, CREATE_ALWAYS );
	if( file )
	{
		ret = API(KERNEL32, GetFileSize)( file, 0 );
		Close(file);
	}
	return ret;
}

bool IsExists( const char* fileName )
{
	DWORD val = API(KERNEL32, GetFileAttributesA)(fileName);
	if( val == INVALID_FILE_ATTRIBUTES )
		return false;
	return true;
}

bool Delete( const char* fileName )
{
	return API(KERNEL32, DeleteFileA)(fileName) != 0;
}

bool Copy( const char* src, const char* dst )
{
	if( API(KERNEL32, CopyFileA)( src, dst, FALSE ) )
		return true;
	return false;
}

bool SetAttributes( const char* fileName, DWORD attr )
{
	if( API(KERNEL32, SetFileAttributesA)( fileName, attr ) )
		return true;
	return false;
}

bool GetTempFile( char* fileName, const char* prefix )
{
	char folder[MAX_PATH];
	API(KERNEL32, GetTempPathA)( sizeof(folder), folder );
	API(KERNEL32, GetTempFileNameA)( folder, prefix, 0, fileName );
	return true;
}

int ListFiles( const StringBuilder& path, const char* mask, typeListFilesCallback func, int subdirs, void* tag )
{
	StringBuilder mask2, path2;
	Path::Combine( mask2, path, mask );
	FileInfo fi;
	int ret = 0;
	HANDLE finder = API(KERNEL32, FindFirstFileA)( mask2, &fi.fd ); //запускаем поиск
	if( finder != INVALID_HANDLE_VALUE ) 
	{
		do
		{
			bool callback = false;
			if( (fi.fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) //это папка?
			{
				//игнорируем '.' и '..'
				if( fi.fd.cFileName[0] != '.' || (fi.fd.cFileName[1] != 0 && fi.fd.cFileName[1] != '.') )
				{
					if( subdirs & 1 )
					{
						Path::Combine( path2, path, fi.fd.cFileName );
						ret = ListFiles( path2, mask, func, subdirs, tag ); //перечисление подпапки
						if( ret == 1 ) break;
					}
					if( subdirs & (2 | 4) ) callback = true;
				}
			}
			else
				if( (subdirs & 4) == 0 ) callback = true;
			if( callback )
			{
				Path::Combine( path2, path, fi.fd.cFileName );
				fi.path = &path;
				fi.fullName = &path2;
				if( !func( fi, tag ) ) 
				{
					ret = 1;
					break;
				}
			}
		} while( API(KERNEL32, FindNextFileA)( finder, &fi.fd ) );
	}
	else
	{
		ret = 2;
	}
	API(KERNEL32, FindClose)(finder);
	return ret;
}

}

MemFile::MemFile( int size ) : data(size), pos(0), linked(false)
{
}

MemFile::MemFile( const void* ptr, int c_ptr ) : data(c_ptr), pos(0), linked(false)
{
	data.Copy( ptr, c_ptr ); 
}

MemFile::MemFile( const char* fileName ) : pos(0), linked(false)
{
	File::ReadAll( fileName, data );
}

MemFile::MemFile( Mem::Data& _data ) : pos(0), linked(true)
{
	data.Link(_data);
}

MemFile::~MemFile()
{
	if( linked )
		data.Unlink();
}

int MemFile::Read( void* mem, int sz )
{
	if( pos + sz > data.Len() ) sz = data.Len() - pos;
	Mem::Copy( mem, (byte*)data.Ptr() + pos, sz );
	pos += sz;
	return sz;
}

int MemFile::Write( const void* mem, int sz )
{
	if( sz == 0 ) return 0;
	int res = data.Copy( pos, 0, sz, mem, sz );
	pos += res;
	return res;
}

int MemFile::Seek( int p, int moveMethod )
{
	switch( moveMethod )
	{
		case FILE_BEGIN:
			pos = p;
			break;
		case FILE_CURRENT:
			pos += p;
			break;
		case FILE_END:
			pos = data.Len() - p;
			break;
	}
	if( pos < 0 ) 
		pos = 0;
	else
		if( pos > data.Len() )
			pos = data.Len();
	return pos;
}

void MemFile::Clear()
{ 
	data.SetLen(0);
	pos = 0;
}
