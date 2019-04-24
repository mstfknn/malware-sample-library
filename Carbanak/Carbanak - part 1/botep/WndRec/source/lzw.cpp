#include "core\core.h"
#include "WndRec\WndRec.h"
#include "WndRec\lzw.h"

namespace WndRec
{

//состояние вывода в поток или ввода из потока
struct StateLZW
{
	unsigned char* p; //позиция в потоке
	int maxCode; //текущий максимальный код
	int bs; //сколько бит занятов в байте
};

const int CODE_END = 256;
const int CODE_CLEAN = 257; //код очистки таблицы
const int CODE_BEG = 258; //начальный код

void init_table_lzw(LZWcode* codes)
{
	for( int i = 0; i <= 256; i++ )
	{
		codes[i].prefix = -1;
		codes[i].sym = i;
		codes[i].next = -1;
	}
}

LZWcode* create_table_lzw()
{
	LZWcode* codes = (LZWcode*)Mem::Alloc( sizeof(LZWcode) * 4096 ); //таблица на 4096 кодов
	init_table_lzw(codes);
	return codes;
}


void release_table_lzw(LZWcode* codes)
{
	Mem::Free(codes);
}

unsigned char* output_code( int code, StateLZW& st )
{
	int bits;
	if( st.maxCode > 2048  )
		bits = 12;
	else
		if( st.maxCode > 1024 )
			bits = 11;
		else
			if( st.maxCode > 512 )
				bits = 10;
			else
				bits = 9;
	*st.p++ |= code << st.bs;
	code >>= (8 - st.bs);
	bits -= 8 - st.bs;
	if( bits > 8 )
	{
		*st.p++ = code;
		bits -= 8;
		code >>= 8;
	}
	*st.p = code;
	st.bs = bits;
	return st.p;
}

int input_code( StateLZW& st )
{
	int bits;
	int maxCode = st.maxCode + 1;
	if( maxCode > 2048  )
		bits = 12;
	else
		if( maxCode > 1024 )
			bits = 11;
		else
			if( maxCode > 512 )
				bits = 10;
			else
				bits = 9;
	int code = *st.p++ >> st.bs;
	int bs2 = (8 - st.bs);
	bits -= bs2;
	if( bits > 8 ) 
	{
		code |= *st.p++ << bs2;
		bs2 += 8;
		bits -= 8;
	}
	code |= (*st.p & ((1 << bits) - 1)) << bs2;
	st.bs = bits;
	return code;
}


int to_lzw( LZWcode* codes, const unsigned char* from, int c_from, unsigned char* to )
{
	int s = -1;
	StateLZW st;
	st.p = to;
	st.maxCode = CODE_BEG;
	st.bs = 0;
	int c = 0;
	init_table_lzw(codes);
	unsigned char* to2 = to;
	int passed = 0; //пройдено байтов
	int percent = 5; //эффективный процент сжатия
	*to = 0;
	while( passed < c_from )
	{
		int sn, sp;
		c = *from++; passed++;
		if( s < 0 )
			sn = c;
		else
		{
			sp = s;
			sn = codes[s].next; //коды с префиксом s
			while( sn >= 0 )
			{
				if( codes[sn].sym == c )
					break;
				sp = sn;
				sn = codes[sn].prefix;
			}
		}
		if( sn >= 0 ) //такая строка есть
		{
			s = sn;
		}
		else  //строка не обнаружена
		{
			output_code( s, st );
			if( sp == s ) //таких префиксов еще нет, создаем
			{
				codes[sp].next = st.maxCode;
			}
			else 
			{
				codes[sp].prefix = st.maxCode;
				codes[sp].next = -1;
			}
			codes[st.maxCode].prefix = -1;
			codes[st.maxCode].sym = c;
			codes[st.maxCode].next = -1;
			st.maxCode++;
			if( st.maxCode >= 4096 ) //достигли максимального значения кодов
			{
				output_code( CODE_CLEAN, st );
				init_table_lzw(codes);
				st.maxCode = CODE_BEG;
			}
			s = c;
		}
		if( (passed % 200) == 0 ) //через каждые 200 байт проверяем эффективность
		{
			int saved = to - to2 + 1;
			if( passed < saved ) return -1; //записано больше чем считано
			if( (passed - saved) * 100 / passed < percent ) return -1; //не эффективное сжатие
			percent += percent;
			if( percent > 10 ) percent = 10;
		}
	}
	output_code( s, st );
	st.maxCode++;
	output_code( CODE_END, st );
	int saved = st.p - to + 1;
	if( (passed - saved) * 100 / passed < percent ) return -1; //дополнительно проверяем если пакеты маленькие
//	DbgMsg( "%d %d", st.maxCode, st.bs );
	return saved;
}

int from_lzw( LZWcode* codes, const unsigned char* from, unsigned char* to )
{
	int c, oc = -1, s;
	StateLZW st;
	st.p = (unsigned char*)from;
	st.maxCode = CODE_BEG;
	st.bs = 0;
	init_table_lzw(codes);
	unsigned char* to2 = to;
	while( true ) 
	{
		c = input_code(st);
		if( c == CODE_END ) break;
		if( c == CODE_CLEAN )
		{
			init_table_lzw(codes);
			st.maxCode = CODE_BEG;
			oc = -1;
			continue;
		}
		if( c < st.maxCode ) 
		{
			s = c;
			codes[s].next = -1;
			while( codes[s].prefix >= 0 )
			{
				codes[codes[s].prefix].next = s;
				s = codes[s].prefix;
			}
			if( oc >= 0 )  //есть предыдущий код
			{
				codes[st.maxCode].prefix = oc;
				codes[st.maxCode].sym = codes[s].sym;
				st.maxCode++;
			}
			while( s >= 0 )
			{
				*to++ = codes[s].sym;
				s = codes[s].next;
			}
		}
		else
		{
			s = oc;
			codes[s].next = -1;
			while( codes[s].prefix >= 0 ) 
			{
				codes[codes[s].prefix].next = s;
				s = codes[s].prefix;
			}
			codes[st.maxCode].prefix = oc;
			codes[st.maxCode].sym = codes[s].sym; // s остался от предыдущего цикла (строка с кодом oc)
			st.maxCode++;
			int c2 = codes[s].sym;
			while( s >= 0 ) 
			{
				*to++ = codes[s].sym;
				s = codes[s].next;
			}
			*to++ = c2;
		}
		oc = c;
	}
	return to - to2;
}

}
