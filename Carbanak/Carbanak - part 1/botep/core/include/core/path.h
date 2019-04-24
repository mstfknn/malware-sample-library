#pragma once

#include "core.h"
#include "shlobj.h"

namespace Path
{

const char DirectorySeparatorChar = '\\';

bool GetSystemDirectore( StringBuilder& path );
bool GetStartupExe( StringBuilder& path ); //имя запущенного файла
//возвращает пути по их CSIDL (обертка для функции SHGetFolderPath)
//в path должно быть достаточно памяти, не меньше MAX_PATH
bool GetCSIDLPath( int csidl, StringBuilder& path );
//к папке csidl добавляет add
bool GetCSIDLPath( int csidl, StringBuilder& path, const char* add );
//возвращает имя временной папки, если указан add, то этот файл добавляется 
bool GetTempPath( StringBuilder& path, const char* add = 0 );
//папка юзерской автозагрузки 
inline bool GetStartupUser( StringBuilder& path ) 
{
	return GetCSIDLPath( CSIDL_STARTUP, path );
}
//возвращает имя файла с расширением (переменная path будет изменена)
StringBuilder& GetFileName( StringBuilder& path );
//возвращает указатель с которого начинается имя файла
char* GetFileName( const char* path );
//возвращает указатель с которого начинается расширение (указатель на точку)
char* GetFileExt( const char* path );
//изменяет в fileName расширение
StringBuilder& ChangeExt( StringBuilder& fileName, const char* newExt );
//возвращает путь (убирает имя файла)
StringBuilder& GetPathName( StringBuilder& path );

StringBuilder& Combine( StringBuilder& res, const StringBuilder& path1, const StringBuilder& path2 );
StringBuilder& Combine( StringBuilder& res, const StringBuilder& path1, const StringBuilder& path2, const StringBuilder& path3 );
StringBuilder& Combine( StringBuilder& res, const StringBuilder& path1, const StringBuilder& path2, const StringBuilder& path3, const StringBuilder& path4 );

StringBuilder& Combine( StringBuilder& res, const char* path1, const char* path2 );
StringBuilder& Combine( StringBuilder& res, const char* path1, const char* path2, const char* path3 );
StringBuilder& Combine( StringBuilder& res, const char* path1, const char* path2, const char* path3, const char* path4 );

//добавляет к пути файл
StringBuilder& AppendFile( StringBuilder& path, const char* file, int c_file = -1 );
inline StringBuilder& AppendFile( StringBuilder& path, const StringBuilder& fileName )
{
	return AppendFile( path, fileName.c_str(), fileName.Len() );
}
//создает указанную папку даже если нет какой-то поддиректории
bool CreateDirectory( const char* path );
//удаляет папку со всеми подпапками
bool DeleteDirectory( const char* path );
//если в конце path находится разделитель \ или /, то он убирается
void RemoveEndSeparator( StringBuilder& path );

}
