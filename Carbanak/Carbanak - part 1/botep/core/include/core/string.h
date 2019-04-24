#pragma once

#include <stdarg.h>
#include "memory.h"
#include "type.h"
#include "misc.h"
#include "vector.h"

#define RN(x) x[0] = '\r', x[1] = '\n', x[2] = 0

namespace Str
{

bool Init();
void Release();

int Len( const char* s );
//копирование строки когда уверен, что в dst достаточно места
char* Copy( char* dst, const char* src, int len = -1 );
//копирование строки с проверкой наличия необходимого количество памяти, если не хватает, то строка обрезается
char* Copy( char* dst, int sz_dst, const char* src, int len = -1 );
char* Duplication( const char* s, int len = -1 );
//выделяет память для строки указанной длины и делает строку нулевой длины
char* Alloc( int len );
inline void Free( char* s ) //освобождает память строки
{
	Mem::Free(s);
}
//соединяет несколько строк в одну, последним аргументом должен быть 0
//функция расчитана максимум на объединение 16 строк
char* Join( const char* s, ... );
//выделяет указанное maxSize памяти и используя функцию wsprintf выводит туда форматированный текст
char* Format( int maxSize, const char* format, ... );
//выводит форматированный текст в buf, возращает длину полученной строки
int Format( char* buf, const char* format, ... );
int FormatVA( char* buf, const char* format, va_list va );
//вычисляет хеш строки
uint Hash( const char* buf, int c_buf = - 1 );
char* Upper( char* s ); //для русских букв в кодировке 1251
char Upper( char c );
char* Lower( char* s ); 
char Lower( char c );
//сравнивает строки, max - количество сравниваемых символов
int Cmp( const char* s1, const char* s2, int max = INT_MAX );
//поиск в строке s подстроки sub, возвращает индекс найденной подстроки,
//если возвращает -1, то подстрока не найдена
int IndexOf( const char* s, const char* sub, int c_s = -1, int c_sub = -1 );
//ищет в строке указанный символ, возвращает его позицию в строке, если -1, то символ не найден
int IndexOf( const char* s, char c );
//замена символов из olds на соотвествующие по индексам символы из news, если в news не будет хватать символов, то будет браться последний
//возвращает длину строки, если news == 0, то символы из olds удаляются, т. е. строка станет короче
int ReplaceChars( char* s, const char* olds, const char* news );
//преобразует строку в число, если hex = true, то указанная строка в 16-ричном виде (16-ричная строка может быть в формате 0xabd или abd)
//пробелы в начале строки игнорируются
int ToInt( const char* s, bool hex = false );
//конвертирует число от 0 до 15 в 16-ричное число
char DecToHex( int v );
//переводит знаковое число в текст, возвращает длину полученной строки
int ToString( int v, char* s, int radix = 10 );
//переводит безнаковое число в текст, возвращает длину полученной строки
int ToString( uint v, char* s, int radix = 10 );
//убирает указанные символы в начале строки, возвращает количество убранных символов
int LTrim( char* s, char c = ' ' );
//убирает указанные символы в конце строки, возвращает количество убранных символов
int RTrim( char* s, int c = ' ', int c_s = -1 );
//убирает указанные символы в начале и конце строки, возвращает количество убранных символов
int Trim( char* s, int c = ' ', int c_s = -1 );
//перекодировка строки char в  wchar_t
//если c_src = -1, то размер src считается функцией
//если to = 0 и size = 0, то возвращает необходимое количество памяти в символах для to
//функция в конце to не ставит сам 0, поэтому если указывается c_src, то указывать с учетом
//завершающего нуля, и size тоже указывать с учетом завершающего нуля
int ToWideChar( const char* src, int c_src, wchar_t* to, int size );
//перекодировка строки char в  wchar_t
//если c_src = -1, то размер src считается функцией
//возвращает перекодированную строку, которую потом нужно удалить
wchar_t* ToWideChar( const char* src, int c_src = -1 );
//игнорирует в строке s указанный символ, т. е. начиная с позиции p ищет первый символ который не равен c
int Ignore( const char* s, int p, char c );

}

namespace WStr
{

int Len( const wchar_t* s );
//перекодировка из wchar в char, size - размер памяти в to (включая место для завершающего нуля)
//c_src - длина src в символах, если -1, то функция считает сама
//если to = 0 и size = 0, то возращает необходимое количество памяти для выходной строки
//возвращает количество записанных байт в to (включая завершающий 0)
int ToWin1251( const wchar_t* src, int c_src, char* to, int size );
wchar_t* Alloc( int len );
inline void Free( wchar_t* s )
{
	Mem::Free(s);
}
//ищет в строке указанный символ, возвращает его позицию в строке, если -1, то символ не найден
int IndexOf( const wchar_t* s, wchar_t c );
wchar_t* Chr( const wchar_t* s, wchar_t c );

}

class StringBuilder;

//массив строк
typedef Vector< MovedPtr<StringBuilder> > StringArray;

class StringBuilder
{
	protected:

		char* ptr; //выделенная памяти для строки
		int len; //длина строки (без завершающего нуля)
		int size; //размер выделенной памяти
		bool alloc; //true - если в ptr выделенная этим классом память, будет удалятся в деструкторе, если false, то память была передана из вне и поэтому удаляться не будет

		StringBuilder( const StringBuilder& );

	public:

		//создает пустую строку и выделяет указанное количество памяти, копирует указанную строку
		explicit StringBuilder( int _size = 128, const char* s = 0, int len_s = -1 ); 
		//создает строку хранимую в переданной памяти, в деструкторе эта память не будет удалятся
		//если _ptr = 0, то будет браться s, т. е. создается объект без копирования строки, 
		//и если _size < 0, то оно будет равно длине строке + 1 (завершающий нуль)
		explicit StringBuilder( char* _ptr, int _size, const char* s = 0, int len_s = -1 );
		//создает строку на основе переданного блока памяти, начало строки с start, если len = -1, то длина строки будет равна длине заполненной памяти
		//в блоке памяти обязательно должно быть место для завершающего нуля, так как конструктор его будет устанавливать.
		//количество выделенной памяти будет равно количеству выделенной памяти в data. 
		//Внимание! data не должно уничтожиться раньше чем данная строка
		StringBuilder( Mem::Data& data, int start = 0, int _len = -1 );
		~StringBuilder();

		//увеличивает размер выделенной памяти для строки, если нужно
		bool Grow( int addSize );
		int Len() const
		{
			return len;
		}

		int Size() const
		{
			return size;
		}

		char* c_str() const
		{
			return ptr;
		}

		operator char*() const
		{
			return c_str();
		}

		char& operator[]( int index )
		{
			return ptr[index];
		}

		StringBuilder& Cat( const char* s, int len_s = -1 );
		StringBuilder& Cat( char c );
		StringBuilder& Cat( int v, int radix = 10 );
		StringBuilder& Cat( uint v, int radix = 10 );
		StringBuilder& Copy( const char* s, int len_s = -1 );

		StringBuilder& operator+=( const char* s )
		{
			return Cat(s);
		}
		StringBuilder& operator+=( const StringBuilder& sb )
		{
			return Cat( sb.ptr, sb.len );
		}
		StringBuilder& operator+=( char c )
		{
			return Cat(c);
		}
		StringBuilder& operator+=( int v )
		{
			return Cat(v);
		}
		StringBuilder& operator+=( uint v )
		{
			return Cat(v);
		}

		StringBuilder& operator=( const char* s )
		{
			return Copy(s);
		}
		StringBuilder& operator=( const StringBuilder& sb )
		{
			return Copy( sb.ptr, sb. len );
		}

		//вычисляет длину строки, используется после использования WINAPI функций
		int UpdateLen();
		//устанавливает новую длину строки и возвращает старую
		int SetLen( int newLen );
		//делает строку пустой
		void SetEmpty()
		{
			SetLen(0);
		}
		StringBuilder& Upper()
		{
			Str::Upper(ptr);
			return *this;
		}
		StringBuilder& Lower()
		{
			Str::Lower(ptr);
			return *this;
		}
		uint Hash() const
		{
			return Str::Hash( ptr, len );
		}

		StringBuilder& Format( const char* format, ... )
		{
			va_list va;
			va_start( va, format );
			len = Str::FormatVA( ptr, format, va );
			return *this;
		}
		int Cmp( const char* s, int max = INT_MAX ) const
		{
			return Str::Cmp( ptr, s, max );
		}
		bool operator==( const char* s ) const
		{
			return Cmp(s) == 0;
		}
		bool operator==( const StringBuilder& s ) const
		{
			return Cmp( s.c_str() ) == 0;
		}
		bool operator!=( const char* s ) const
		{
			return Cmp(s) != 0;
		}
		bool operator!=( const StringBuilder& s ) const
		{
			return Cmp( s.c_str() ) != 0;
		}
		int IndexOf( int start, const char* sub, int c_sub = -1 ) const;
		int IndexOf( int start, const StringBuilder& sub ) const
		{
			return IndexOf( start, sub.ptr, sub.len );
		}
		int IndexOf( const char* sub, int c_sub = -1 ) const
		{
			return IndexOf( 0, sub, c_sub );
		}
		int IndexOf( const StringBuilder& sub ) const
		{
			return IndexOf( 0, sub );
		}
		int IndexOf( int start, char c ) const;
		int IndexOf( char c ) const
		{
			return IndexOf( 0, c );
		}

		//выделяет подстроку начиная с позиции index и длиною len
		StringBuilder& Substring( int index, int _len = INT_MAX );
		//выделяет подстроку начиная с позиции index и длиною len и ложит в s
		//возвращает s (выделенную строку) 
		StringBuilder& Substring( StringBuilder& s, int index, int _len = INT_MAX ) const;
		//вставляет указанную позицию строку, если индекс больше длины строки, то вставка не будет проводится
		StringBuilder& Insert( int index, const char* s, int c_s = -1 );
		StringBuilder& Insert( int index, const StringBuilder& s )
		{
			return Insert( index, s.c_str(), s.Len() );
		}
		StringBuilder& Insert( int index, char c );
		//заменяет в строке с позиции index и sz символов на строку s
		StringBuilder& Replace( int index, int sz, const char* s, int c_s = -1 );
		StringBuilder& Replace( int index, int sz, const StringBuilder& s )
		{
			return Replace( index, sz, s.c_str(), s.Len() );
		}
		//заменить строку oldStr на newStr начиная с позиции start и n раз
		//если не указывать newStr, т. е. передавать 0, то будет удаляться строки oldStr, но только вместо 0, лучше ставить nullptr, так как из-за
		//перегружаемых функций может вызваться нета
		StringBuilder& Replace( int start, int n, const char* oldStr, const char* newStr, int c_oldStr = -1, int c_newStr = -1 );
		StringBuilder& Replace( int start, const char* oldStr, const char* newStr, int c_oldStr = -1, int c_newStr = -1 )
		{
			return Replace( start, INT_MAX, oldStr, newStr, c_oldStr, c_newStr );
		}
		StringBuilder& Replace( const char* oldStr, const char* newStr, int c_oldStr = -1, int c_newStr = -1 )
		{
			return Replace( 0, INT_MAX, oldStr, newStr, c_oldStr, c_newStr );
		}
		StringBuilder& Replace( int start, int n, const StringBuilder& oldStr, const StringBuilder& newStr )
		{
			return Replace( start, n, oldStr.c_str(), newStr.c_str(), oldStr.Len(), newStr.Len() );
		}
		StringBuilder& Replace( int start, const StringBuilder& oldStr, const StringBuilder& newStr )
		{
			return Replace( start, INT_MAX, oldStr.c_str(), newStr.c_str(), oldStr.Len(), newStr.Len() );
		}
		StringBuilder& Replace( const StringBuilder& oldStr, const StringBuilder& newStr )
		{
			return Replace( 0, INT_MAX, oldStr.c_str(), newStr.c_str(), oldStr.Len(), newStr.Len() );
		}

		int ReplaceChars( const char* olds, const char* news )
		{
			return len = Str::ReplaceChars( ptr, olds, news );
		}

		int ReplaceChar( char oldc, char newc );

		bool IsEmpty() const
		{
			if( len == 0 ) return true;
			return false;
		}
		StringArray Split( char c ) const;
		StringArray Split( const char* s, int c_s = -1 ) const;
		int ToInt( bool hex = false )
		{
			return Str::ToInt( ptr, hex );
		}
		int ToInt( int start, bool hex = false );
		StringBuilder& LTrim( char c = ' ' )
		{
			len -= Str::LTrim( ptr, c );
			return *this;
		}
		StringBuilder& RTrim( char c = ' ' )
		{
			len -= Str::RTrim( ptr, c, len );
			return *this;
		}
		StringBuilder& Trim( char c = ' ' )
		{
			len -= Str::Trim( ptr, c, len );
			return *this;
		}
		//сформировать конец строки (\r\n)
		StringBuilder& FillEndStr();
		//заполнить строку символами c, строка станет длиною count
		StringBuilder& Set( char c, int count );
		//оставляет count символов слева
		StringBuilder& Left( int count )
		{
			SetLen(len);
			return *this;
		}
		//оставляет count символов справа
		StringBuilder& Right( int count );
		//конвертирует строки unicode в win1251
		StringBuilder& ToWin1251( const wchar_t* from, int c_from = -1 );

		int Ignore( int p, char c )
		{
			return Str::Ignore( ptr, p, c );
		}
};

//создание строки на стеке, для быстрого выполнения строковых операций
template<int size_on_stack>
class StringBuilderStack : public StringBuilder
{
		char buf[size_on_stack];

		StringBuilderStack( const StringBuilderStack& );

	public:

		StringBuilderStack( const char* s = 0, int len = - 1 ) : StringBuilder( buf, size_on_stack, s, len )
		{
		}
		StringBuilder& operator=( const char* s )
		{
			return Copy(s);
		}
		StringBuilder& operator=( const StringBuilder& sb )
		{
			return Copy( sb.c_str(), sb.Len() );
		}
		int Size() const
		{
			return size_on_stack;
		}

};

//массив строк
typedef Vector< MovedPtr<StringBuilder> > StringArray;

#include "string_crypt.h"
