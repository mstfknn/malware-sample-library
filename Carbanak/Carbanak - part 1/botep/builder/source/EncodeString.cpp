#include "stdafx.h"
#include "main.h"
#include <stdio.h>
#include "core\file.h"
#include "core\pe.h"

char EncodeTable[256]; //перекодировачная таблица

static uint GenRand( Config* cfg )
{
	cfg->R = (cfg->R * cfg->A + cfg->B) & 0xffff;
	return cfg->R;
}

static void Exchange( Config* cfg, int p1, int p2, int count )
{
	int dist = p2 - p1 + 1;
	for( int i = 0; i < count; i++ )
	{
		int i1 = (GenRand(cfg) % dist) + p1;
		int i2 = (GenRand(cfg) % dist) + p1;
		char t = EncodeTable[i1];
		EncodeTable[i1] = EncodeTable[i2];
		EncodeTable[i2] = t;
	}
}

void CreateEncodeTable( Config* cfg )
{
	for( int i = 0; i < 256; i++ )
		EncodeTable[i] = i;
	int exchanges = (cfg->R % 1000) + 128;
	//тасуем 3 разных диапазона символов, чтобы при кодировании сохранялась принадлежность символов одному диапазону
	Exchange( cfg, 1, 31, exchanges ); //тасование кодов меньше пробела
	Exchange( cfg, 32, 127, exchanges ); //тасование ascii кодов
	Exchange( cfg, 128, 255, exchanges ); //тасование русских символов
}

int EncodeString( const char* s, char* buf, int c_s )
{
	if( c_s < 0 ) c_s = Str::Len(s);
	int lenBlock = c_s / CountStringOpcode;
	int nb = 0; //номер блока
	int rb = 0; //сколько символов осталось в блоке
	int delta = 0;
	int n = 0;
	int i = 0;
	while( i < c_s )
	{
		int c = 0;
		if( rb == 0 )
		{
			nb++;
			if( nb <= CountStringOpcode )
			{
				delta = rand() % 28;
				c = delta + 'a';
			}
			rb = lenBlock;
		}
		else
			rb--;
		if( c == 0 )
		{
			c = s[i];
			int min, max;
			if( c < 32 )
			{
				min = 1; max = 31;
			}
			else if( c < 128 )
			{
				min = 32; max = 127;
			}
			else
			{
				min = 128; max = 255;
			}
			c += delta;
			if( c > max ) c = min + c - max;
			c = EncodeTable[c];
			i++;
		}
		buf[n++] = c;
	}
	buf[n] = 0;
	return n;
}

int EncodeAllStrings( Config* cfg, Mem::Data& data )
{
	int ret = 0;
	char* beg = BEG_ENCODE_STRING;
	char* end = END_ENCODE_STRING;
	int c_beg = Str::Len(beg);
	int c_end = Str::Len(end);
	int p = 0, p_end = 0;
	PIMAGE_NT_HEADERS headers = PE::GetNTHeaders( (HMODULE)data.Ptr() );
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(headers);
	//ищем секции data и только внутри них кодируем строки
	for( int i = 0; i < headers->FileHeader.NumberOfSections; i++, section++ )
	{
		if( Str::IndexOf( (char*)section->Name, "data" ) >= 0 )
		{
			if( p == 0 ) p = section->PointerToRawData;
			p_end = section->PointerToRawData + section->SizeOfRawData;
		}
	}
	for(;;)
	{
		int p2 = data.IndexOf( p, beg, c_beg );
		if( p2 < 0 ) break;
		int i = c_beg;
		bool finded = false;
		while( p2 + i < p_end - c_end + 1 )
		{
			if( Str::Cmp( (char*)data.Ptr() + p2 + i, end, c_end ) == 0 && ((char*)data.Ptr())[p2 + i + c_end] == 0 )
			{
				finded = true;
				break;
			}
			i++;
		}
		if( finded )
		{
			char* s = (char*)data.Ptr() + p2 + c_beg;
			int c_s = i - c_beg;
			int sz_buf = c_s + c_beg + c_end + 1;
			char* buf = Str::Alloc(sz_buf);
			EncodeString( s, buf, c_s );
			data.Copy( p2, buf, sz_buf );
			Str::Free(buf);
			p = p2 + sz_buf;
			ret++;
		}
		else
			p = p2 + c_beg;
	}
	return ret;

}
