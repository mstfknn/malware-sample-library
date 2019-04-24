#include "core\winapi.h"
#include "core\memory.h"
#include "core\string.h"
#include "core\debug.h"

namespace Mem
{

static HANDLE globalHeap = 0; //хендл памяти 

//низкоуровневые функции работы с кучей
static HANDLE InitHeap()
{
	return API(KERNEL32, HeapCreate)( 0, 0, 0 );
}

static void ReleaseHeap( HANDLE heap )
{
	API(KERNEL32, HeapDestroy)(heap);
}

static void* AllocHeap( HANDLE heap, int size )
{
	return API(KERNEL32, HeapAlloc)( heap, 0, size );
}

static void* ReallocHeap( HANDLE heap, void* ptr, int newSize )
{
	if( ptr == 0 )
		return AllocHeap( heap, newSize );
	return API(KERNEL32, HeapReAlloc)( heap, 0, ptr, newSize );
}

static void FreeHeap( HANDLE heap, void* ptr )
{
	API(KERNEL32, HeapFree)( heap, 0, ptr );
}

//функции выделения и освобождения памяти в глобальной куче
bool Init()
{
	globalHeap = InitHeap();
	if( globalHeap )
		return true;
	else
		return false;
}

void Release()
{
	ReleaseHeap(globalHeap);
}

void* Alloc( int size )
{
	return AllocHeap( globalHeap, size );
}

void* Realloc( void* ptr, int newSize )
{
	return ReallocHeap( globalHeap, ptr, newSize );
}

void Free( void* ptr )
{
	FreeHeap( globalHeap, ptr );
}

void* Copy( void* dst, const void* src, int size )
{
	if( dst && src && size > 0 )
	{
		byte* to = (byte*)dst;
		byte* from = (byte*)src;
		while( size-- ) *to++ = *from++;
	}
	return dst;
}

void* Duplication( const void* src, int size )
{
	if( src == nullptr || size <= 0 ) return nullptr;
	void* ret = Mem::Alloc(size);
	if( ret )
	{
		Copy( ret, src, size );
	}
	return ret;
}

void* Set( void* ptr, int c, int count )
{
	if( ptr && count > 0 )
	{
		volatile byte* p = (byte*)ptr; //ключевое слово volatile ставим из-за того чтобы компилер данный цикл не заменил на функцию memset
		for( int i = 0; i < count; i++, p++ )
			*p = c;
	}
	return ptr;
}

int IndexOf( const void* ptr, int c_ptr, const void* what, int c_what )
{
	if( ptr == 0 || c_ptr <= 0 || what == 0 || c_what <= 0 ) return -1;
	int index = -1;
	byte* s = (byte*)ptr;
	byte* sub = (byte*)what;
	for( int i = 0; i <= c_ptr - c_what; i++ )
	{
		int j = 0;
		for( ; j < c_what; j++ )
			if( s[i + j] != sub[j] ) break;
		if( j >= c_what )
		{
			index = i;
			break;
		}
	}
	return index;
}

int IndexOf( const void* ptr, char c, int c_ptr )
{
	char* p = (char*)Find( ptr, c, c_ptr );
	if( p )
		return p - (char*)ptr;
	return -1;
}

void* Find( const void* ptr, char c, int c_ptr )
{
	if( ptr == 0 || c_ptr <= 0 ) return 0;
	char* p = (char*)ptr;
	while( c_ptr-- )
	{
		if( *p == c )
			return p;
		p++;
	}
	return 0;
}

int Cmp( const void* mem1, const void* mem2, size_t n )
{
	if( mem1 == 0 )
		if( mem2 )
			return -1;
		else
			return 0;
	else
		if( mem2 == 0 )
			return 0;
	const byte* p1 = (const byte*)mem1;
	const byte* p2 = (const byte*)mem2;
	while( n-- )
	{
		if( *p1 != *p2 )
			return *p1 - *p2;
		p1++; p2++;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////

Data::Data( int sz )
{
	if( sz > 0 )
	{
		data = Alloc(sz);
		if( data )
			sz_data = sz;
		else
			sz_data = 0;
	}
	else
	{
		data = 0;
		sz_data = 0;
	}
	c_data = 0;
}

Data::~Data()
{
	Free(data);
}

bool Data::Realloc( int sz )
{
	if( sz < c_data ) return false;
	void* newData = Mem::Realloc( data, sz );
	if( newData )
	{
		data = newData;
		sz_data = sz;
		return true;
	}
	return false;
}

bool Data::MakeEnough( int sz )
{
	if( sz_data - c_data >= sz ) return true;
	int addSize = sz_data / 2;
	if( addSize < sz ) addSize = sz;
	return Realloc( sz_data + addSize );
}

int Data::SetLen( int sz )
{
	if( sz < 0 ) 
		c_data = 0;
	else
		if( sz > sz_data )
			c_data = sz_data;
		else
			c_data = sz;
	return c_data;
}

int Data::AddLen( int add )
{
	c_data += add;
	if( c_data < 0 ) 
		c_data = 0;
	else if( c_data > sz_data )
		c_data = sz_data;
	return c_data;
}

bool Data::Append( const void* ptr, int c_ptr )
{
	if( MakeEnough(c_ptr ) )
	{
		Mem::Copy( (byte*)data + c_data, ptr, c_ptr );
		c_data += c_ptr;
		return true;
	}
	return false;
}

bool Data::AppendStr( const char* s, int c_s )
{
	if( c_s < 0 ) c_s = Str::Len(s);
	if( s )
		return Append( s, c_s + 1 );
	else
	{
		char empty[1]; empty[0] = 0;
		return Append( empty, 1 );
	}
}

void Data::Submem( int index, int len )
{
	if( index >= 0 )
	{
		if( index < c_data )
		{
			if( len > c_data - index ) len = c_data - index;
			Mem::Copy( data, (byte*)data + index, len );
			c_data = len;
		}
		else
		{
			c_data = 0;
		}
	}
}

void Data::Submem( Data& mem, int index, int len )
{
	if( index >= 0 )
	{
		if( index < c_data )
		{
			if( len > c_data - index ) len = c_data - index;
			mem.MakeEnough(len);
			Mem::Copy( mem.data, (byte*)data + index, len );
			mem.c_data = len;
		}
		else
		{
			mem.c_data = 0;
		}
	}
}

bool Data::Insert( int index, const void* ptr, int c_ptr )
{
	if( index < 0 || index > c_data || ptr == 0 || c_ptr < 0 ) return false;
	if( !MakeEnough(c_data + c_ptr) ) return false;
	byte* pd = (byte*)data;
	for( int from = c_data - 1, to = c_data + c_ptr - 1; from >= index; from--, to-- )
		pd[to] = pd[from];
	Mem::Copy( pd + index, ptr, c_ptr );
	c_data += c_ptr;
	return true;
}

void Data::Remove( int index, int len )
{
	if( index < 0 || index >= c_data) return;
	if( index + len > c_data ) len = c_data - index;
	Mem::Copy( (byte*)data + index, (byte*)data + index + len, c_data - (index + len) );
	c_data -= len;
}

int Data::IndexOf( int start, const void* what, int c_what )
{
	if( start < 0 ) start = 0;
	int res = Mem::IndexOf( (byte*)data + start, c_data - start, what, c_what );
	if( res >= 0 )
		res += start;
	return res;
}

int Data::IndexOf( int start, char c )
{
	if( start < 0 ) start = 0;
	int res = Mem::IndexOf( (byte*)data + start, c, c_data - start );
	if( res >= 0 )
		res += start;
	return res;
}

int Data::Copy( const void* src, int c_src )
{
	if( !src || c_src <= 0 ) return 0;
	if( MakeEnough(c_src) )
	{
		Mem::Copy( data, src, c_src );
		c_data = c_src;
		return c_src;
	}
	return 0;
}

int Data::Copy( int dst_start, int src_start, int count, const void* src, int c_src )
{
	if( dst_start < 0 ) dst_start = 0;
	if( src_start < 0 ) src_start = 0;
	if( count < 0 ) count = 0;
	if( src_start + count > c_src ) count = c_src - src_start;
	if( MakeEnough( dst_start + count ) )
	{
		Mem::Copy( (byte*)data + dst_start, (byte*)src + src_start, count );
		if( dst_start + count > c_data ) c_data = dst_start + count;
		return count;
	}
	return 0;
}

StringBuilder& Data::ToString( StringBuilder& s )
{
	//в конце блока data могут быть нули, поэтому корректируем длину строки
	while( c_data > 0 && ((byte*)data)[c_data - 1] == 0 ) c_data--;
	return s.Copy( (char*)data, c_data );
}

void* Data::Unlink()
{
	void* ret = data;
	data = nullptr;
	c_data = 0;
	sz_data = 0;
	return ret;
}

void Data::Link( Data& src )
{
	data = src.data;
	c_data = src.c_data;
	sz_data = src.sz_data;
}

void Data::Link( void* _data, int _c_data )
{
	data = _data;
	c_data = _c_data;
	sz_data = _c_data;
}

int Data::Replace( void* oldv, int c_oldv, void* newv, int c_newv )
{
	int p = 0, ret = 0;
	for(;;)
	{
		p = IndexOf( p, oldv, c_oldv );
		if( p < 0 ) break;
		ret++;
		if( c_oldv >= c_newv )
		{
			Mem::Copy( (byte*)data + p, newv, c_newv );
			p += c_newv;
			Remove( p, c_oldv - c_newv );
		}
		else
		{
			Mem::Copy( (byte*)data + p, newv, c_oldv );
			Insert( p + c_oldv, (byte*)newv + c_oldv, c_newv - c_oldv );
			p += c_newv;
		}
	}
	return ret;
}

}

//заменяем стандартные операции с памятью
#pragma function(memset)
void *memset(void *dst, int c, size_t n)
{
	return Mem::Set( dst, c, n );
}

#pragma function(memcpy)
void* memcpy( void* dst, const void* src, size_t n )
{
	return Mem::Copy( dst, src, n );
}

#pragma function(memcmp)
int memcmp( const void* s1, const void* s2, size_t n )
{
	return Mem::Cmp( s1, s2, n );
}
