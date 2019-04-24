#pragma once

//Работа с памятью

#include "misc.h"
#include "type.h"

class StringBuilder;
typedef MovedPtr<byte> AutoMem; //память с а втоматическим освобождением памяти

namespace Mem
{

bool Init();
void Release();

void* Alloc( int size );
void Free( void* ptr );
void* Realloc( void* ptr, int newSize );

void* Copy( void* dst, const void* src, int size );
void* Duplication( const void* src, int size );
void* Set( void* ptr, int c, int count );
//поиск в памяти указанной последовательности, возвращает позицию где найдена или -1, если ничего ненайдено
int IndexOf( const void* ptr, int c_ptr, const void* what, int c_what );
int IndexOf( const void* ptr, char c, int c_ptr );
//поиск указанного символа, возвращает указатель на него, или 0 если не найден
void* Find( const void* ptr, char c, int c_ptr );
int Cmp( const void* mem1, const void* mem2, size_t n ); 

//очистка структур
template<class T> void Zero( T& ptr )
{
	Set( &ptr, 0, sizeof(T) );
}

//выделенная память с данными
class Data
{
		void* data; //выделенная память
		int c_data, sz_data; //длина записанных данных и размер выделенной памяти

		Data( const Data& );

	public:

		Data( int sz = 0 );
		~Data();
		bool Realloc( int sz );
		//проверяет достаточно ли памяти для добавления sz данных, если нет, то перевыделяет чтобы хватало
		bool MakeEnough( int sz );
		void* Ptr() const
		{
			return data;
		}
		byte* p_byte() const
		{
			return (byte*)data;
		}
		char* p_char() const
		{
			return (char*)data;
		}
		int Len() const
		{
			return c_data;
		}
		int SetLen( int sz );
		int AddLen( int add );

		int Size() const
		{
			return sz_data;
		}
		bool Append( const void* ptr, int c_ptr ); //добавляет новые данные, если выделенной памяти не будет хватать, то перевыделится сколько нужно
		bool AppendStr( const char* s, int c_s = - 1 ); //добавляет строку, если s = 0, то добавляет 0 (пустая строка)
		bool IsEnough( int sz ) //достачно ли памяти для записи sz байт
		{
			if( sz < sz_data - c_data )
				return true;
			return false;
		}
		void* PtrWrite() //начало свободной памяти  куда можно писать данные
		{
			return (byte*)data + c_data;
		}
		void Clear()
		{
			c_data = 0;
		}
		//заполнение выделенной памяти указаным байтом
		void SetFull( char c )
		{
			Mem::Set( data, c, sz_data );
		}
		int IndexOf( int start, const void* what, int c_what );
		int IndexOf( int start, char c );
		int IndexOf( const void* what, int c_what )
		{
			return Mem::IndexOf( data, c_data, what, c_what );
		}
		//копирует массив байт в начало памяти начиная с index и длиною len, т. е. байт с 0 до index - 1 убираются
		void Submem( int index, int len = INT_MAX );
		//копирует байт начиная с index в mem
		void Submem( Data& mem, int index, int len = INT_MAX );
		//всталяет в указанную позицию массив ptr длиною c_ptr, возвращает true если удалось вставить
		bool Insert( int index, const void* ptr, int c_ptr );
		bool Insert( int index, const Data& data )
		{
			return Insert( index, data.Ptr(), data.Len() );
		}
		//убирает из массива байты начиная с позиции i и длиною len
		void Remove( int index, int len );
		void Left( int n ) //оставляет начальные n байт
		{
			Submem( 0, n );
		}
		void Right( int n ) //оставляет конечные n байт
		{
			Submem( c_data - n, n );
		}
		void DelLeft( int n ) //удаляет начальные n байт
		{
			return Right( c_data - n );
		}
		void DelRight( int n ) //удаляет конечные n байт
		{
			return Left( c_data - n );
		}
		//копирует блок памяти, возвращает сколько байт скопировано
		int Copy( const void* src, int c_src ); //тут длина заполненной памяти будет равна c_src
		int Copy( const Data& src )
		{
			return Copy( src.data, src.c_data );
		}
		int Copy( int dst_start, int src_start, int count, const void* src, int c_src ); //тут длина заполненной памяти останется прежней либо увеличится если dst_start + c_src будет больше
		int Copy( int start, const void* src, int c_src )
		{
			return Copy( start, 0, c_src, src, c_src );
		}
		int Copy( int dst_start, int src_start, int count, const Data& src )
		{
			return Copy( dst_start, src_start, count, src.data, src.c_data );
		}
		int Copy( int dst_start, int src_start, const Data& src )
		{
			return Copy( dst_start, src_start, src.c_data - src_start, src );
		}
		StringBuilder& ToString( StringBuilder& s );
		//отвязывает от этого объекта выделенную память, чтобы в деструкторе она не была удалена. Возвращает указатель на отвязанную память
		void* Unlink();
		//подключает данные из другой Data, обязательно после нее нужно сделать Unlink, иначе будет двойное удаление массива data
		//текущая Data должна быть пустой (сразу после конструктора Data() или после Unlink), иначе возможна утечка памяти
		//также не в коем случае не должно быть перевыделения памяти.
		//Link нужно использовать только если привязываемые данные не изменяются
		void Link( Data& src );
		void Link( void* _data, int _c_data );
		//заменяет oldv на newv, возвращает количество замен
		int Replace( void* oldv, int c_oldv, void* newv, int c_newv );
};

}

//переопределяем стандартные операции C++ для выделения и удаления памяти
inline void* operator new( size_t size )
{
	return Mem::Alloc(size);
}

inline void* operator new[]( size_t size)
{
	return Mem::Alloc(size);
}

inline void operator delete( void* ptr )
{
	Mem::Free(ptr);
}

inline void operator delete[]( void* ptr )
{
	Mem::Free(ptr);
}
