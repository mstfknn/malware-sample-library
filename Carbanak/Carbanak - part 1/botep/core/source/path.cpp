#include "core\path.h"
#include "shlwapi.h"
#include "core\debug.h"

namespace Path
{

bool GetSystemDirectore( StringBuilder& path )
{
	if( API(KERNEL32, GetSystemDirectoryA)( path.c_str(), path.Size() ) > 0 )
	{
		path.UpdateLen();
		return true;
	}
	return false;
}

bool GetStartupExe( StringBuilder& path )
{
	int len = API(KERNEL32, GetModuleFileNameA)( 0, path.c_str(), path.Size() );
	if( len > 0 )
	{
		path.SetLen(len);
		return true;
	}
	return false;
}

bool GetCSIDLPath( int csidl, StringBuilder& path )
{
	if( API(SHELL32, SHGetFolderPathA)( 0, csidl, 0, SHGFP_TYPE_CURRENT, path.c_str() ) == S_OK )
	{
		path.UpdateLen();
		return true;
	}
	path.SetEmpty();
	return false;
}

bool GetCSIDLPath( int csidl, StringBuilder& path, const char* add )
{
	if( GetCSIDLPath( csidl, path ) )
	{
		AppendFile( path, add );
		return true;
	}
	return false;
}

bool GetTempPath( StringBuilder& path, const char* add )
{
	int len = API(KERNEL32, GetTempPathA)( path.Size(), path );
	if( len > 0 )
	{
		path.SetLen(len);
		if( add ) AppendFile( path, add );
		return true;
	}
	return false;
}

StringBuilder& GetFileName( StringBuilder& path )
{
	char* p = GetFileName( path.c_str() );
	path.Substring( p - path.c_str() );
	return path;
}

char* GetFileName( const char* path )
{
	return API(SHLWAPI, PathFindFileNameA)(path);
}

char* GetFileExt( const char* path )
{
	return API(SHLWAPI, PathFindExtensionA)(path);
}

StringBuilder& ChangeExt( StringBuilder& fileName, const char* newExt )
{
	char* p = GetFileExt(fileName);
	if( p )
	{
		p[1] = 0;
		fileName.SetLen( p - fileName.c_str() + 1 );
		fileName += newExt;
	}
	return fileName;
}

StringBuilder& GetPathName( StringBuilder& path )
{
	char* p = GetFileName( path.c_str() );
	int len = p - path.c_str() - 1; //-1 убираем символ /
	path.Substring( 0, len );
	return path;
}

struct data_combine
{
	const char* s;
	int len;
};

static StringBuilder& Combine( StringBuilder& res, const data_combine* paths, int count )
{
	res.Copy( paths[0].s, paths[0].len );
	for( int i = 1; i < count; i++ )
	{
		res.Cat(DirectorySeparatorChar);
		res.Cat( paths[i].s, paths[i].len );
	}
	return res;
}

StringBuilder& Combine( StringBuilder& res, const StringBuilder& path1, const StringBuilder& path2 )
{
	data_combine paths[2];
	paths[0].s = path1.c_str(); paths[0].len = path1.Len();
	paths[1].s = path2.c_str(); paths[1].len = path2.Len();
	return Combine( res, paths, 2 );
}

StringBuilder& Combine( StringBuilder& res, const StringBuilder& path1, const StringBuilder& path2, const StringBuilder& path3 )
{
	data_combine paths[3];
	paths[0].s = path1.c_str(); paths[0].len = path1.Len();
	paths[1].s = path2.c_str(); paths[1].len = path2.Len();
	paths[2].s = path3.c_str(); paths[2].len = path3.Len();
	return Combine( res, paths, 3 );
}

StringBuilder& Combine( StringBuilder& res, const StringBuilder& path1, const StringBuilder& path2, const StringBuilder& path3, const StringBuilder& path4 )
{
	data_combine paths[4];
	paths[0].s = path1.c_str(); paths[0].len = path1.Len();
	paths[1].s = path2.c_str(); paths[1].len = path2.Len();
	paths[2].s = path3.c_str(); paths[2].len = path3.Len();
	paths[3].s = path4.c_str(); paths[3].len = path4.Len();
	return Combine( res, paths, 4 );
}

StringBuilder& Combine( StringBuilder& res, const char* path1, const char* path2 )
{
	data_combine paths[2];
	paths[0].s = path1; paths[0].len = -1; 
	paths[1].s = path2; paths[1].len = -1;
	return Combine( res, paths, 2 );
}

StringBuilder& Combine( StringBuilder& res, const char* path1, const char* path2, const char* path3 )
{
	data_combine paths[3];
	paths[0].s = path1; paths[0].len = -1; 
	paths[1].s = path2; paths[1].len = -1;
	paths[2].s = path3; paths[2].len = -1;
	return Combine( res, paths, 3 );
}

StringBuilder& Combine( StringBuilder& res, const char* path1, const char* path2, const char* path3, const char* path4 )
{
	data_combine paths[4];
	paths[0].s = path1; paths[0].len = -1; 
	paths[1].s = path2; paths[1].len = -1;
	paths[2].s = path3; paths[2].len = -1;
	paths[3].s = path4; paths[3].len = -1;
	return Combine( res, paths, 4 );
}

StringBuilder& AppendFile( StringBuilder& path, const char* file, int c_file )
{
	if( path.Len() > 0 )
	{
		//смотрим стоит ли в конце разделитель директории
		char lastc = path[path.Len() - 1];
		if( lastc == '\\' || lastc == '/' )
			path.SetLen( path.Len() - 1 ); //убираем его
	}
	path.Cat( DirectorySeparatorChar );
	return path.Cat( file, c_file );
}

bool CreateDirectory( const char* path )
{
	int res =  API(SHELL32, SHCreateDirectoryExA)( 0, path, 0 );
	if( res == ERROR_ALREADY_EXISTS || res == ERROR_SUCCESS )
		return true;
	return false;
}

bool DeleteDirectory( const char* path )
{
	SHFILEOPSTRUCTA fo;
	Mem::Zero(fo);
	fo.wFunc = FO_DELETE;
	fo.fFlags =  FOF_NOCONFIRMATION | FOF_SILENT;
	fo.fAnyOperationsAborted = FALSE;
	StringBuilderStack<MAX_PATH> path2(path);
	path2[ path2.Len() + 1 ] = 0; //в конце имени папки должно обязательно стоять дна нуля
	fo.pFrom = path2;

	if( API(SHELL32, SHFileOperationA)( &fo ) )
		return false;
	return true;
}

void RemoveEndSeparator( StringBuilder& path )
{
	int i = 1;
	for( int i = 1; i < path.Len(); i++ )
	{
		int p = path.Len() - i;
		if( path[p] != '\\' && path[p] != '/' )
		{
			path.SetLen(p + 1);
			break;
		}
	}
}

}
