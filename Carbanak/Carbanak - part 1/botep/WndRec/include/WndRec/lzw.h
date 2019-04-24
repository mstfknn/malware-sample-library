#pragma once

namespace WndRec
{

struct LZWcode
{
	short prefix;
	unsigned char sym;
	short next;
};

int to_lzw( LZWcode* codes, const unsigned char* from, int c_from, unsigned char* to );
int from_lzw( LZWcode* codes, const unsigned char* from, unsigned char* to );

LZWcode* create_table_lzw();
void release_table_lzw(LZWcode*);

}
