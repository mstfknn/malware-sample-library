#include "core\winapi.h"
#include "core\string.h"
#include "core\memory.h"
#include "core\misc.h"
#include "core\debug.h"

#include <shlwapi.h>

namespace Str
{

bool Init()
{
	return true;
}

void Release()
{
}

int Len( const char* s )
{
	if( s == 0 ) return 0;
	int res = 0;
	while( *s++ ) res++;
	return res;
}

char* Copy( char* dst, const char* src, int len )
{
	if( dst )
	{
		if( len < 0 ) len = Len(src);
		Mem::Copy( dst, src, len );
		dst[len] = 0;
	}
	return dst;
}

char* Copy( char* dst, int sz_dst, const char* src, int len )
{
	if( len < 0 ) len = Str::Len(src);
	if( len >= sz_dst ) len = sz_dst - 1;
	return Copy( dst, src, len );
}

char* Duplication( const char* s, int len )
{
	if( s )
	{
		if( len < 0 ) len = Len(s);
		char* ret = (char*)Mem::Alloc( len + 1 );
		if( ret )
			return Copy( ret, s, len );
	}
	return 0;
}

char* Alloc( int len )
{
	if( len < 0 ) return 0;
	char* s = (char*)Mem::Alloc(len + 1);
	s[0] = 0;
	return s;
}

char* Join( const char* s, ... )
{
	int len[16];
	const char* str[16];
	int count = 0;
	int lenRes = 0;
	if( s )
	{
		//считаем конечную длину строки
		len[0] = Len(s); str[0] = s;
		lenRes += len[0];
		count++;
		va_list va;
		va_start( va, s );
		while( count < 16 )
		{
			const char* sn = va_arg( va, const char* );
			if( !sn ) break;
			len[count] = Len(sn);
			str[count] = sn;
			lenRes += len[count];
			count++;
		}
		va_end(va);
	}
	//создаем новую строку
	if( lenRes == 0 ) return 0;
	char* res = (char*)Mem::Alloc( lenRes + 1 );
	char* p = res;
	for( int i = 0; i < count; i++ )
	{
		Mem::Copy( p, str[i], len[i] );
		p += len[i];
	}
	res[lenRes] = 0;
	return res;
}

int FormatVA( char* buf, const char* format, va_list va )
{
	return API(USER32, wvsprintfA)( buf, format, va );
}

char* Format( int maxSize, const char* format, ... )
{
	char* res = (char*)Mem::Alloc(maxSize);
	if( res )
	{
		va_list va;
		va_start( va, format );
		FormatVA( res, format, va );
	}
	return res;
}

int Format( char* buf, const char* format, ... )
{
	va_list va;
	va_start( va, format );
	return FormatVA( buf, format, va );
}

uint Hash( const char* s, int c_s )
{
	if( c_s < 0 ) c_s = Len(s);
	return CalcHash( (byte*)s, c_s );
}

char* Upper( char* s )
{
	char* p = s;
	while( *p )
	{
		*p = Upper(*p);
		p++;
	}
	return s;
}

char Upper( char c )
{
	if( c >= 'a' && c <= 'z' )
		return  c - 'a' + 'A';
	else
		if( (byte)c >= byte('а') && (byte)c <= byte('€') ) //русские буквы в кодировке 1251
			return (byte)c - byte('а') + byte('ј');
	return c;
}

char* Lower( char* s )
{
	char* p = s;
	while( *p )
	{
		*p = Lower(*p);
		p++;
	}
	return s;
}

char Lower( char c )
{
	if( c >= 'A' && c <= 'Z' )
		return  c - 'A' + 'a';
	else
		if( (byte)c >= byte('ј') && (byte)c <= byte('я') ) //русские буквы в кодировке 1251
			return (byte)c - byte('ј') + byte('а');
	return c;
}


int Cmp( const char* s1, const char* s2, int max )
{
	if( s1 )
		if( s2 )
		{
			int n = 0; //количество пройденых символов
			for(;;)
			{
				if( n >= max ) return 0;
				if( *s1 )
					if( *s2 )
					{
						if( *s1 > *s2 )
							return 1;
						else if( *s1 < *s2 )
							return -1;
					}
					else
						return 1;
				else
					if( *s2 )
						return -1;
					else
						return 0;
				s1++; s2++; n++;
			}
		}
		else
			return 1;
	else
		if( s2 )
			return -1;
		else
			return 0;
}

int IndexOf( const char* s, const char* sub, int c_s, int c_sub )
{
	if( c_s < 0 ) c_s = Len(s);
	if( c_sub < 0 ) c_sub = Len(sub);
	return Mem::IndexOf( s, c_s, sub, c_sub );
}

int IndexOf( const char* s, char c )
{
	if( s )
	{
		const char* p = s;
		while( *p )
		{
			if( *p == c )
				return p - s;
			p++;
		}
	}
	return -1;
}

int ReplaceChars( char* s, const char* olds, const char* news )
{
	int ret = 0;
	char* from = s;
	char* to = s;
	int c_news = 0;
	if( news ) c_news = Len(news);
	while( *from )
	{
		int i = IndexOf( olds, *from );
		if( i >= 0 )
		{
			if( i < c_news )
				*to++ = news[i];
			else
				if( c_news > 0 )
					*to++ = news[c_news - 1];
		}
		else
			*to++ = *from;
		from++;
	}
	*to = 0;
	return to - s;
}

int ToInt( const char* s, bool hex )
{
	if( s == 0 ) return 0;
	char hex0x[16];
	const char* ptr = s;
	int flag = STIF_DEFAULT;
	if( hex )
	{
		//функци€ StrToIntEx требует, чтобы в начале было 0x
		flag = STIF_SUPPORT_HEX;
		if( s[0] != '0' || s[1] != 'x' || s[1] != 'X' )
		{
			hex0x[0] = '0';
			hex0x[1] = 'x';
			while( *s == ' ' ) s++; //игнорируем пробелы
			int len = Len(s);
			if( len > sizeof(hex0x) - 3 ) len = sizeof(hex0x) - 3;
			Copy( hex0x + 2, s, len );
			ptr = hex0x;
		}
	}
	int ret;
	if( API(SHLWAPI, StrToIntExA)( ptr, flag, &ret ) )
	{
		return ret;
	}
	return 0;
}

char DecToHex( int v )
{
	if( v >= 0 && v <= 9 )
		return '0' + v;
	else
		return 'A' + (v - 10);
}

//непосредственный перевод числа в текст
static int ToStringBegin( uint v, char* buf, int radix )
{
	int i = 0;
	do
	{
		int digit = v % radix;
		if( radix == 16 )
			buf[i] = DecToHex(digit);
		else
			buf[i] = digit + '0';
		i++;
		v = v / radix;
	} while( v );
	return i;
}

//переворачивает результат работы ToStringBegin
static void ToStringEnd( const char* buf, char* s, int len )
{
	while( --len >= 0 )
		*s++ = buf[len];
	*s = 0;
}

int ToString( int v, char* s, int radix )
{
	bool neg = false;
	if( v < 0 && radix == 10 )
	{
		neg = true;
		v = -v;
	}
	char buf[16];
	int len = ToStringBegin( v, buf, radix );
	int j = 0;
	if( neg ) s[j++] = '-';
	ToStringEnd( buf, s + j, len );
	return len + j;
}

int ToString( uint v, char* s, int radix )
{
	char buf[16];
	int len = ToStringBegin( v, buf, radix );
	ToStringEnd( buf, s, len );
	return len;
}

int LTrim( char* s, char c )
{
	if( s == 0 ) return 0;
	int i = 0;
	while( s[i] == c ) i++;
	int ret = i;
	if( i > 0 )
	{
		int j = 0;
		while( s[i] ) s[j++] = s[i++];
		s[j] = 0;
	}
	return ret;
}

int RTrim( char* s, int c, int c_s )
{
	if( s == 0 ) return 0;
	if( c_s < 0 ) c_s = Str::Len(s);
	int i = c_s - 1;
	while( i >= 0 )
	{
		if( s[i] != c ) break;
		i--;
	}
	s[i + 1] = 0;
	return c_s - i - 1;
}

int Trim( char* s, int c, int c_s )
{
	int ret = LTrim( s, c );
	if( c_s >= 0 ) c_s -= ret;
	ret += RTrim( s, c, c_s );
	return ret;
}

int ToWideChar( const char* src, int c_src, wchar_t* to, int size )
{
	return API(KERNEL32, MultiByteToWideChar)( CP_ACP, 0, src, c_src, to, size );
}

wchar_t* ToWideChar( const char* src, int c_src )
{
	if( c_src < 0 ) c_src = Str::Len(src);
	wchar_t* ret = WStr::Alloc(c_src);
	ToWideChar( src, c_src + 1, ret, c_src + 1 );
	return ret;
}

int Ignore( const char* s, int p, char c )
{
	if( s == 0 || p < 0 ) return -1;
	while( s[p] == c ) p++;
	return p;
}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace WStr
{

int Len( const wchar_t* s )
{
	if( s == 0 ) return 0;
	int res = 0;
	while( *s++ ) res++;
	return res;
}

int ToWin1251( const wchar_t* src, int c_src, char* to, int size )
{
	int len = API(KERNEL32, WideCharToMultiByte)( 1251, 0, src, c_src, to, size, 0, 0);
	return len;
}

//выдел€ет пам€ть дл€ строки указанной длины
wchar_t* Alloc( int len )
{
	if( len < 0 ) return 0;
	wchar_t* s = (wchar_t*)Mem::Alloc( sizeof(wchar_t) * (len + 1) );
	s[0] = 0;
	return s;
}

int IndexOf( const wchar_t* s, wchar_t c )
{
	wchar_t* p = Chr( s, c );
	if( p )
		return s - p;
	return -1;
}

wchar_t* Chr( const wchar_t* s, wchar_t c )
{
	if( s )
	{
		while( *s )
		{
			if( *s == c )
				return (wchar_t*)s;
			s++;
		}
	}
	return 0;
}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

StringBuilder::StringBuilder( int _size, const char* s, int len_s )
{
	ptr = Str::Alloc(_size);
	size = _size;
	alloc = true;
	len = 0;
	Cat( s, len_s );
}

StringBuilder::StringBuilder( char* _ptr, int _size, const char* s, int len_s )
{
	size = _size;
	alloc = false;
	if( _ptr )
	{
		if( size < 0 ) size = 0;
		ptr = _ptr;
		ptr[0] = 0;
		len = 0;
		Cat( s, len_s );
	}
	else
	{
		ptr = (char*)s;
		if( len_s < 0 ) len_s = Str::Len(s);
		len = len_s;
		if( size < 0 ) size = len + 1;
	}
}

StringBuilder::StringBuilder( Mem::Data& data, int start, int _len )
{
	if( data.Ptr() == 0 )
	{
		ptr = 0;
		len = 0;
		size = 0;
	}
	else
	{
		alloc = false;
		if( start < 0 ) 
			start = 0;
		else 
			if( start > data.Len() )
				start = data.Len();
		if( _len < 0 || _len + start > data.Len() ) _len = data.Len() - start;
		ptr = (char*)data.Ptr() + start;
		len = _len;
		//в конце блока data могут быть нули, поэтому корректируем длину строки
		while( len > 0 && ptr[len - 1] == 0 ) len--;
		ptr[len] = 0;
		size = data.Size() - start;
	}
}

StringBuilder::~StringBuilder()
{
	if( alloc )
		Str::Free(ptr);
}

bool StringBuilder::Grow( int addSize )
{
	if( len + addSize >= size - 1 )
	{
		if( alloc )
		{
			int addSize2 = size / 2;
			if( addSize2 > addSize ) addSize = addSize2;
			int newSize = size + addSize;
			char* newPtr = (char*)Mem::Realloc( ptr, newSize );
			if( newPtr ) //если пам€ть перевыделилась
			{
				ptr = newPtr;
				size = newSize;
				return true;
			}
		}
		else
			return false;
	}
	return true;
}

StringBuilder& StringBuilder::Cat( const char* s, int len_s )
{
	if( s ) 
	{
		if( len_s < 0 ) len_s = Str::Len(s);
		if( !Grow(len_s) )
			len_s = size - len - 1;
		Str::Copy( ptr + len, s, len_s );
		len += len_s;
	}
	return *this;
}

StringBuilder& StringBuilder::Cat( char c )
{
	Grow(1);
	if( len < size - 1 )
	{
		ptr[len] = c;
		len++;
		ptr[len] = 0;
	}
	return *this;
}

StringBuilder& StringBuilder::Cat( int v, int radix )
{
	char buf[16];
	int len = Str::ToString( v, buf, radix );
	return Cat( buf, len );
}

StringBuilder& StringBuilder::Cat( uint v, int radix )
{
	char buf[16];
	int len = Str::ToString( v, buf, radix );
	return Cat( buf, len );
}

StringBuilder& StringBuilder::Copy( const char* s, int len_s )
{
	len = 0;
	return Cat( s, len_s );
}

int StringBuilder::UpdateLen()
{
	len = Str::Len(ptr);
	return len;
}

int StringBuilder::SetLen( int newLen )
{
	int oldLen = len;
	if( newLen < size && newLen >= 0 ) //если нова€ меньше выделенной пам€ти
	{
		ptr[newLen] = 0;
		len = newLen;
	}
	return oldLen;
}

int StringBuilder::IndexOf( int start, const char* sub, int c_sub ) const
{
	if( start < 0 ) start = 0;
	int res = Str::IndexOf( ptr + start, sub, len - start, c_sub );
	if( res >= 0 )
		return start + res;
	return -1;
}

int StringBuilder::IndexOf( int start, char c ) const
{
	if( start < 0 ) start = 0;
	if( start >= len ) return -1;
	int res = Str::IndexOf( ptr + start, c );
	if( res >= 0 )
		return start + res;
	return -1;
}

StringBuilder& StringBuilder::Substring( int index, int _len )
{
	if( index >= 0 )
	{
		if( index < len )
		{
			if( _len > len - index ) _len = len - index;
			Str::Copy( ptr, ptr + index, _len );
			len = _len;
		}
		else
		{
			*ptr = 0;
			len = 0;
		}
	}
	return *this;
}

StringBuilder& StringBuilder::Substring( StringBuilder& s, int index, int _len ) const
{
	if( index >= 0 )
	{
		if( index < len )
		{
			if( _len > len - index ) _len = len - index;
			s.Copy( ptr + index, _len );
		}
		else
			s.SetEmpty();
	}
	return s;
}

StringBuilder& StringBuilder::Insert( int index, const char* s, int c_s )
{
	if( index >= 0 && index <= len )
	{
		if( c_s < 0 ) c_s = Str::Len(s);
		Grow(c_s);
		int newLen = len + c_s;
		int moved = len - index;
		if( newLen > size - 1 )
		{
			moved -= newLen - (size - 1);
			if( moved < 0 )
				c_s = size - 1 - index;
			newLen = size - 1;
		}
		if( c_s > 0 )
		{
			int end = newLen - 1;
			while( moved > 0 )
			{
				ptr[end] = ptr[end - c_s];
				moved--; end--;
			}
			Mem::Copy( ptr + index, s, c_s );
			ptr[newLen] = 0;
			len = newLen;
		}
	}
	return *this;
}

StringBuilder& StringBuilder::Insert( int index, char c )
{
	char buf[2];
	buf[0] = c;
	buf[1] = 0;
	return Insert( index, buf, 1 );
}

StringBuilder& StringBuilder::Replace( int index, int sz, const char* s, int c_s )
{
	if( index >= 0 && index <= len && sz >= 0 )
	{
		if( index + sz > len ) sz = len - index;
		if( c_s < 0 ) c_s = Str::Len(s);
		int newLen = len - sz + c_s;
		int moved = len - (index + sz);
		if( sz > c_s ) //убираем больше чем вставл€ем
		{
			Mem::Copy( ptr + index + c_s, ptr + index + sz, moved );
		}
		else //убираем меньше чем вставл€ем
		{
			Grow( c_s - sz );
			if( newLen > size - 1 )
			{
				moved -= newLen - (size - 1);
				if( moved < 0 )
					c_s = size - 1 - index;
				newLen = size - 1;
			}
			if( c_s > 0 )
			{
				int end = newLen - 1;
				char* src = &ptr[end - (c_s - sz)];
				char* dst = &ptr[end];
				while( moved > 0 )
				{
					*dst-- = *src--;
					moved--;
				}
			}
		}
		Mem::Copy( ptr + index, s, c_s );
		ptr[newLen] = 0;
		len = newLen;
	}
	return *this;
}

StringBuilder& StringBuilder::Replace( int start, int n, const char* oldStr, const char* newStr, int c_oldStr, int c_newStr )
{
	int p = start;
	if( c_oldStr < 0 ) c_oldStr = Str::Len(oldStr);
	if( c_newStr < 0 ) c_newStr = Str::Len(newStr);
	while( n-- )
	{
		int p2 = IndexOf( p, oldStr, c_oldStr );
		if( p2 >= 0 )
		{
			Replace( p2, c_oldStr, newStr, c_newStr );
			p = p2 + c_newStr;
		}
		else
			break;
	}
	return *this;
}

int StringBuilder::ReplaceChar( char oldc, char newc )
{
	char buf1[2]; buf1[0] = oldc; buf1[1] = 0;
	char buf2[2]; buf2[0] = newc; buf2[1] = 0;
	return ReplaceChars( buf1, buf2 );
}

StringArray StringBuilder::Split( char c ) const
{
	char buf[2];
	buf[0] = c;
	buf[1] = 0;
	return Split( buf, 1 );
}

StringArray StringBuilder::Split( const char* s, int c_s ) const
{
	StringArray ret;
	if( s )
	{
		if( c_s < 0 ) c_s = Str::Len(s);
		int p = 0;
		StringBuilder tmp;
		for(;;)
		{
			int pp = Str::IndexOf( ptr + p, s, len - p, c_s );
			if( pp < 0 ) 
				pp = len;
			else
				pp += p;
			tmp.Copy( ptr + p, pp - p );
			ret.Add(tmp);
			if( pp >= len ) break;
			p = pp + c_s;
		} 
	}
	return ret;
}

int StringBuilder::ToInt( int start, bool hex )
{
	if( start >= 0 && start < len )
		return Str::ToInt( ptr + start, hex );
	else
		return 0;
}

StringBuilder& StringBuilder::FillEndStr()
{
	if( size >= 3 )
	{
		ptr[0] = '\r';
		ptr[1] = '\n';
		ptr[2] = 0;
		len = 2;
	}
	else
		len = 0;
	return *this;
}

StringBuilder& StringBuilder::Set( char c, int count )
{
	if( count >= 0 )
	{
		if( count > size - 1 )
		{
			if( !Grow( count - size + 1 ) )
				count = size - 1;
		}
		Mem::Set( ptr, c, count );
		ptr[count] = 0;
		len = count;
	}
	return *this;
}

StringBuilder& StringBuilder::Right( int count )
{
	if( count < len && count >= 0 ) 
		return Substring( len - count );
	return *this;
}

StringBuilder& StringBuilder::ToWin1251( const wchar_t* from, int c_from )
{
	int needed = WStr::ToWin1251( from, c_from, 0, 0 );
	if( needed > 0 )
	{
		if( c_from > 0 ) needed++; //если c_from = -1, то в needed учтен завершающий нуль
		if( needed > size )
		{
			if( !Grow(needed - size ) )
				c_from = size - 1;
		}
		needed = WStr::ToWin1251( from, c_from, ptr, size );
		if( c_from < 0 ) needed--; 
	}
	SetLen(needed);
	return *this;
}
