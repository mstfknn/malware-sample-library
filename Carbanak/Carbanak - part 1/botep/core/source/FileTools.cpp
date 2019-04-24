#include "core\FileTools.h"
#include "core\debug.h"
#include "Imagehlp.h"

namespace File
{

typedef bool (*typeCallbackAfterPatch)( Mem::Data& data );

int Patch( const char* fileNameFrom, const char* fileNameTo, PatchData* patch, typeCallbackAfterPatch cb )
{
	if( patch == 0 ) return 0;
	if( fileNameTo == 0 ) fileNameTo = fileNameFrom;
	Mem::Data data;
	if( !File::ReadAll( fileNameFrom, data ) )
		return -1;
	int ret = 0;
	while( patch->find )
	{
		int n = data.IndexOf( patch->find, patch->c_find );
		if( n >= 0 )
		{
			data.Copy( n + patch->offset, 0, patch->c_find, patch->replace, patch->c_replace );
			ret++;
		}
		patch++;
	}
	if( cb ) 
		if( !cb(data) )
			return -2;
	if( !File::Write( fileNameTo, data ) )
		return -1;
	return ret;
}

static bool CalcCheckSum( Mem::Data& data )
{
	DWORD checkSum = 0, currCheckSum;
	IMAGE_NT_HEADERS* pe = API(IMAGEHLP, CheckSumMappedFile)( data.Ptr(), data.Len(), &currCheckSum, &checkSum );
	if( pe )
	{
		pe->OptionalHeader.CheckSum = checkSum;
		return true;
	}
	return false;
}

int PatchExe( const char* fileNameFrom, const char* fileNameTo, PatchData* patch )
{
	return Patch( fileNameFrom, fileNameTo, patch, CalcCheckSum );
}

}
