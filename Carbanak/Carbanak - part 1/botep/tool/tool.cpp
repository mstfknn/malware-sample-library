#include "core\core.h"
#include "core\misc.h"
#include "core\file.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool CmdHash( int argc, char* argv[] );
bool CmdUrl( int argc, char* argv[] );
void TextToUrl( const char* psw, const char* text, StringBuilder& url );
void CmdCollectApi( int argc, char* argv[] );

int main(int argc, char* argv[])
{
	WinAPI::Init();
	Core::Init();
	if( argc < 2 )
	{
		printf( "ERROR: not command\n" );
	}
	else
	{
		if( strcmp( argv[1], "hash" ) ==  0 )
		{
			CmdHash( argc - 2, argv + 2 );
		}
		else if( strcmp( argv[1], "url" ) == 0 )
		{
			CmdUrl( argc - 2, argv + 2 );
		}
		else if( strcmp( argv[1], "collectapi" ) == 0 )
		{
			CmdCollectApi( argc - 2, argv + 2 );
		}
		else
			printf( "ERROR: unknow command '%s'\n", argv[1] );
	}
	return 0;
}

bool CmdHash( int argc, char* argv[] )
{
	if( argc != 1 )
	{
		printf( "ERROR: for command 'hash' needed 1 paramaters" );
		return false;
	}
	if( argv[0][0] == '@' ) //передан файл
	{
		Mem::Data data;
		File::ReadAll( argv[0] + 1, data );
		StringBuilder s(data);
		StringArray ss = s.Split("\r\n");
		FILE* file = fopen( "out.txt", "w" );
		if( file )
		{
			for( int i = 0; i < ss.Count(); i++ )
			{
				ss[i]->Trim();
				if( !ss[i]->IsEmpty() )
				{
					uint hash = ss[i]->Hash();
					fprintf( file, "%s -> 0x%08x %d\n", ss[i]->c_str(), hash, hash );
				}
			}
			fclose(file);
		}
	}
	else
	{
		uint hash = CalcHash( (byte*)argv[0], strlen(argv[0]) );
		printf( "%s -> 0x%08x %d\n", argv[0], hash, hash );
	}
	return true;
}

bool CmdUrl( int argc, char* argv[] )
{
	if( argc != 2 )
	{
		printf( "ERROR: for command 'url' needed 2 parameters: [psw] [text]" );
		return false;
	}
	StringBuilder url;
	TextToUrl( argv[0], argv[1], url );
	printf( "%s", url.c_str() );
	return true;
}

#ifdef ON_CODE_STRING
//определяем функцию для декодирования зашифрованных строк
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
