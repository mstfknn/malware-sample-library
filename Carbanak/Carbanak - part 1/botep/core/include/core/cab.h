#pragma once

#include "core.h"
#include "File.h"
#include <fdi.h>
#include <fci.h>

class Cab 
{
		CCAB cab;
		HFCI hfci;
		ERF erf;
		MemFile file;
		
	public:

		Cab();
		~Cab();
		void Close();
		bool AddFile( const char* dstFile, MemFile& srcFile );
		bool AddFile( const char* dstFile, const char* srcFile );
		bool AddFile( const char* dstFile, const void* data, int c_data );
		bool AddFile( const char* dstFile, const Mem::Data& data )
		{
			return AddFile( dstFile, data.Ptr(), data.Len() );
		}
		bool AddFile( const char* dstFile, const StringBuilder& s )
		{
			return AddFile( dstFile, s.c_str(), s.Len() );
		}
		bool AddFolder( const char* dstFolder, const char* srcFolder, bool subdirs = true );
		void Save( const char* fileName );
		Mem::Data& GetData()
		{
			return file.data;
		}

		static bool Extract( const Mem::Data& from, const char* to );
};
