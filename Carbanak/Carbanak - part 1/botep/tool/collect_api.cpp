#include "core\core.h"
#include "core\file.h"
#include <stdio.h>

struct CollectData
{
	StringArray dlls;
	StringArray funcs;
};

int FindItem( const StringArray& sa, const StringBuilder& s )
{
	for( int i = 0; i < sa.Count(); i++ )
		if( sa[i]->Cmp(s) == 0 )
			return i;
	return -1;
}

bool C_Cpp_ListCallback( File::FileInfo& fi, void* tag )
{
	StringBuilderStack<16> ext = Path::GetFileExt( fi.fullName->c_str() );
	ext.Lower();
	if( ext != ".cpp" && ext != ".c" ) return true;
	printf( "%s\n", fi.fullName->c_str() );
	Mem::Data data;
	File::ReadAll( *fi.fullName, data );
	StringBuilder s(data);
	StringArray sa = s.Split( "\r\n", 2 );
	CollectData* cd = (CollectData*)tag;
	for( int i = 0; i < sa.Count(); i++ )
	{
		StringBuilder& r = sa[i];
		int index = 0;
		for(;;)
		{
			int p = r.IndexOf( index, "API(" );
			if( p < 0 ) break;
			p += 4;
			if( r[p] == '"' ) break; //если попали на строку которая подобна предыдущей ( "API(" ), то игнорим
			int p2 = r.IndexOf( p, ',' );
			if( p2 < 0 )
			{
				printf( "ERROR: wrong row %d in %s\n%s\n", i + 1, fi.fullName->c_str(), r.c_str() );
				return false;
			}
			int p3 = r.IndexOf( p2, ')' );
			if( p3 < 0 )
			{
				printf( "ERROR: wrong row %d in %s\n%s\n", i + 1, fi.fullName->c_str(), r.c_str() );
				return false;
			}
			StringBuilderStack<16> dllName;
			r.Substring( dllName, p, p2 - p );
			StringBuilderStack<64> funcName;
			r.Substring( funcName, p2 + 1, p3 - p2 - 1 );
			dllName.Trim();
			funcName.Trim();
			int n = FindItem( cd->funcs, funcName );
			if( n >= 0 )
			{
				if( *cd->dlls[n] == funcName )
				{
					printf( "ERROR: function %s have %s and %s, file %s, row %d\n", funcName.c_str(), dllName.c_str(), cd->dlls[n]->c_str(), fi.fullName->c_str(), i + 1 );
					return false;
				}
			}
			else
			{
				cd->funcs.Add(funcName);
				cd->dlls.Add(dllName);
			}
			index = p3 + 1;
		}
	}
	return true;
}

void CmdCollectApi( int argc, char* argv[] )
{
	if( argc != 1 )
	{
		printf( "ERROR: for command 'collectapi' needed 1 parameter (path)\n" );
		return;
	}
	CollectData cd;
	StringBuilder path( nullptr, -1, argv[0] );
	File::ListFiles( path, "*.*", C_Cpp_ListCallback, true, &cd );
	FILE* file = fopen( "api_funcs_hash.h", "w" );
	fprintf( file, "#pragma once\n\n" );
	fprintf( file, "// auto generation: tool.exe collectapi\n\n" );
	for( int i = 0; i < cd.funcs.Count(); i++ )
	{
		fprintf( file, "const unsigned int hash%s = 0x%08x;\n", cd.funcs[i]->c_str(), cd.funcs[i]->Hash() );
	}
	fclose(file);
	
	file = fopen( "api_funcs_type.h", "w" );
	fprintf( file, "#pragma once\n\n" );
	fprintf( file, "// auto generation: tool.exe collectapi\n\n#include <windows.h>\n\n" );
	for( int i = 0; i < cd.funcs.Count(); i++ )
	{
		fprintf( file, "typedef __ ( WINAPI *type%s)( __ ); //%s\n", cd.funcs[i]->c_str(), cd.dlls[i]->c_str() );
	}
	fclose(file);
}

