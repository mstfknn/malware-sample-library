#pragma once

#include "core.h"

namespace File
{

//инфорация о файле при перечислении файлов в папке
struct FileInfo
{
	WIN32_FIND_DATAA fd;
	const StringBuilder* path; //папка в которой находится файл
	StringBuilder* fullName; //полное имя файла
};

//вызывается функцией ListFiles, если возвращает false, то останавливает дальнейшее перечисление
//tag - данные передаваемые функцией запустившей ListFiles()
typedef bool (*typeListFilesCallback)( FileInfo&, void* tag );

//открывает файл, используется если нужно работать с хендлом файла
HANDLE Open( const char* fileName, DWORD access, DWORD creation, DWORD flags = 0 );
bool Write( HANDLE file, const void* data, int c_data );
uint Read( HANDLE file, void* data, int c_data );
//чтение всего файла 
bool ReadAll( const char* fileName, Mem::Data& data );
//запись данных в файл
bool WriteAll( const char* fileName, const void* data, int c_data );
inline bool Write( const char* fileName, const StringBuilder& s )
{
	return WriteAll( fileName, s.c_str(), s.Len() );
}
inline bool Write( const char* fileName, const Mem::Data& data )
{
	return WriteAll( fileName, data.Ptr(), data.Len() );
}
//запись данных в конец файла
bool Append( const char* fileName, const void* data, int c_data );
inline bool Append( const char* fileName, const Mem::Data& data )
{
	return Append( fileName, data.Ptr(), data.Len() );
}
inline bool Append( const char* fileName, const StringBuilder& s )
{
	return Append( fileName, s.c_str(), s.Len() );
}
bool Append( const char* fileName, const char* s, int c_s = - 1 );

//возвращает размер файла, если возвращает -1, то ошибка
int Size( const char* fileName );
//проверяет наличие файла
bool IsExists( const char* fileName );
bool Delete( const char* fileName );
void Close( HANDLE file );
//копирует файл из src в dst
bool Copy( const char* src, const char* dst );
//устанавливает аттрибуты файлу
bool SetAttributes( const char* fileName, DWORD attr );
//возвращает имя временного файла, размер fileName должно быть не меньше MAX_PATH
bool GetTempFile( char* fileName, const char* prefix = 0 );
//перечисляет все файлы в папке path по маске mask
//если subdirs = true, то и перечисляются файлы в подпапках
//возвращает 0, если все папки перечислены, 1 - если поиск был остановлен callback функцией, 2 - если нет доступа к папке
int ListFiles( const StringBuilder& path, const char* mask, typeListFilesCallback func, int subdirs, void* tag = 0 );
//записывает в содержимое файла одни 0xff и потом удаляет файл
bool DeleteHard( const char* fileName );

}

class MemFile
{
	public:

		Mem::Data data;
		int pos; //позиция в файле

	private:
	
		bool linked;

	public:

		MemFile( int size = 0 );
		MemFile( const void* ptr, int c_ptr );
		MemFile( const char* fileName );
		MemFile( Mem::Data& _data );
		~MemFile();

		int Read( void* mem, int sz );
		int Write( const void* mem, int sz );
		int Seek( int p, int moveMethod );
		void Clear();
};
