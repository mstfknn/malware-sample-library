#include "core\cab.h"
#include "core\debug.h"

//содержит имя распакованного файла и его содержимое в памяти
struct ExtractInfo
{
	StringBuilderStack<MAX_PATH> fileName; //имя распакованного файла
	MemFile file; //содержимое файла
	const char* folder; //папка в которую распаковываем
};

//при распаковке либа 2 раза открывает исходный файл, поскольку исходный файл в памяти, то создаем 2 файла в памяти
//с одинаковым содержимым и подаем их последовательно
struct ExtractFiles
{
	MemFile file1;
	MemFile file2;
	int n;
	
	ExtractFiles( Mem::Data& data ) : file1(data), file2(data), n(0)
	{
	}

};

static LPVOID DIAMONDAPI FN_FCIALLOC( ULONG cb )
{
	return Mem::Alloc(cb);
}

static void DIAMONDAPI FN_FCIFREE( void HUGE *lpMem )
{
	Mem::Free(lpMem);
}

static INT_PTR DIAMONDAPI FN_FCIOPEN( char FAR *pszFile, int oflag, int pmode, int FAR *err, void FAR *pv )
{
	INT_PTR hf = Str::ToInt(pszFile);
	return hf;
}

static INT_PTR DIAMONDAPI FN_FDIOPEN( char FAR *pszFile, int oflag, int pmode )
{
	ExtractFiles* files = (ExtractFiles*)Str::ToInt(pszFile);
	INT_PTR hf;
	switch( files->n )
	{
		case 0: hf = (INT_PTR)&files->file1; break;
		case 1: hf = (INT_PTR)&files->file2; break;
		default: hf = (INT_PTR)-1;
	}
	files->n++;
	return hf;
}

static UINT DIAMONDAPI FN_FCIREAD( INT_PTR hf, void FAR *memory, UINT cb, int FAR *err, void FAR *pv )
{
	if( !hf ) return (UINT)-1;
	MemFile* file = (MemFile*)hf;
	*err = 0;
	int pos = file->pos;
	int ret = file->Read( memory, cb );
	return ret;
}

static UINT DIAMONDAPI FN_FDIREAD( INT_PTR hf, void FAR *memory, UINT cb )
{
	int err;
	return FN_FCIREAD( hf, memory, cb, &err, 0 );
}

static UINT DIAMONDAPI FN_FCIWRITE( INT_PTR hf, void FAR *memory, UINT cb, int FAR *err, void FAR *pv )
{
	if( !hf ) return (UINT)-1;
	MemFile* file = (MemFile*)hf;
	*err = 0;
	int ret = file->Write( memory, cb );
	return ret;
}

static UINT DIAMONDAPI FN_FDIWRITE( INT_PTR hf, void FAR *memory, UINT cb )
{
	int err;
	return FN_FCIWRITE( hf, memory, cb, &err, 0 );
}

static int DIAMONDAPI FN_FCICLOSE( INT_PTR hf, int FAR *err, void FAR *pv )
{
	MemFile* file = (MemFile*)hf;
	file->Seek( 0, FILE_BEGIN );
	*err = 0;
	return 0;
}

static int DIAMONDAPI FN_FDICLOSE( INT_PTR hf )
{
	int err;
	return FN_FCICLOSE( hf, &err, 0 );
}

static long DIAMONDAPI FN_FCISEEK( INT_PTR hf, long dist, int seektype, int FAR *err, void FAR *pv )
{
	MemFile* file = (MemFile*)hf;
	*err = 0;
	int ret = file->Seek( dist, seektype );
	return ret;
}

static long DIAMONDAPI FN_FDISEEK( INT_PTR hf, long dist, int seektype )
{
	int err;
    return FN_FCISEEK( hf, dist, seektype, &err, 0 );
}

static int DIAMONDAPI FN_FCIDELETE( char FAR *pszFile, int FAR *err, void FAR *pv )
{
	MemFile* file = (MemFile*)Str::ToInt(pszFile);
	delete file;
	*err = 0;
	return 0;
}

static long DIAMONDAPI FN_FCISTATUS( UINT typeStatus, ULONG  cb1, ULONG  cb2, void FAR *pv )
{
	return 0;
}

static int DIAMONDAPI FN_FCIFILEPLACED( PCCAB pccab, char *pszFile, long  cbFile, BOOL  fContinuation, void FAR *pv )
{
	return 0;
}

static INT_PTR DIAMONDAPI FN_FCIGETOPENINFO( char *pszName, USHORT *pdate, USHORT *ptime, USHORT *pattribs, int FAR *err, void FAR *pv )
{
	INT_PTR hf = Str::ToInt(pszName);
	*pdate = 1;
	*ptime = 1;
	*pattribs = 1;
    return hf;
}


static BOOL DIAMONDAPI FN_FCIGETTEMPFILE( char *pszTempName, int cbTempName, void FAR *pv )
{
	MemFile* file = new MemFile();
	Str::ToString( (uint)file, pszTempName );
	return TRUE;
}

static BOOL DIAMONDAPI FN_FCIGETNEXTCABINET( PCCAB pccab, ULONG cbPrevCab,	void FAR *pv )
{
	return FALSE;
}

static INT_PTR DIAMONDAPI ExtractCabNotify( FDINOTIFICATIONTYPE fdint, PFDINOTIFICATION pfdin )
{
	INT_PTR ret = 0;
	ExtractInfo* ei = (ExtractInfo*)pfdin->pv;
	switch( fdint )
	{
		case fdintCABINET_INFO:
			break;
		case fdintPARTIAL_FILE:
			break;
		case fdintCOPY_FILE:
			{
				Path::Combine( ei->fileName, ei->folder, pfdin->psz1 );
				ret = (INT_PTR)&ei->file;
			}
			break;
		case fdintCLOSE_FILE_INFO:
			{
				char* p = Path::GetFileName( ei->fileName.c_str() );
				int c = 0;
				if( p != ei->fileName.c_str() ) //есть папка
				{
					p--;
					char c = *p;
					*p = 0;
					Path::CreateDirectory(ei->fileName);
					*p = c;
				}
				File::Write( ei->fileName, ei->file.data );
				ei->file.Clear();
			}
			ret = TRUE;
			break;
		case fdintNEXT_CABINET:
			break;
		case fdintENUMERATE:
			break;	
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//поскольку все операции на нужно проводить в памяти (а либа расчитана только на работу с реальными файлами), то вместо
//имен файлов передаем указатели на MemFile в текстовом виде, которые конвертируются обратно при открытии

Cab::Cab()
{
	Mem::Zero(cab);
	Mem::Zero(erf);
	Str::ToString( (uint)&file, cab.szCab );
	cab.cb = 0x7FFFFFFF;
	cab.cbFolderThresh = 0x7FFFFFFF;
	hfci = API(CABINET, FCICreate)( &erf, FN_FCIFILEPLACED, FN_FCIALLOC, FN_FCIFREE, FN_FCIOPEN, FN_FCIREAD, FN_FCIWRITE, FN_FCICLOSE, FN_FCISEEK,
										FN_FCIDELETE, FN_FCIGETTEMPFILE, &cab, this );
}

Cab::~Cab()
{
	Close();
}

void Cab::Close()
{
	if( hfci )
	{
		API(CABINET, FCIFlushCabinet)( hfci, FALSE, FN_FCIGETNEXTCABINET, FN_FCISTATUS );
		API(CABINET, FCIDestroy)(hfci);
		hfci = 0;
	}
}

bool Cab::AddFile( const char* dstFile, MemFile& srcFile )
{
	char fileName[16];
	Str::ToString( (uint)&srcFile, fileName );
	if( API(CABINET, FCIAddFile)( hfci, fileName, (char*)dstFile, FALSE, FN_FCIGETNEXTCABINET, FN_FCISTATUS, FN_FCIGETOPENINFO, tcompTYPE_MSZIP ) )
		return true;
	return false;
}

bool Cab::AddFile( const char* dstFile, const char* srcFile )
{
	MemFile file(srcFile);
	return AddFile( dstFile, file );
}

bool Cab::AddFile( const char* dstFile, const void* data, int c_data )
{
	MemFile file( data, c_data );
	return AddFile( dstFile, file );
}

struct ForAddFile
{
	Cab* cab;
	const StringBuilder* srcFolder;
	const char* dstFolder;
	int lenDstFolder;
};

bool CallbackAddFile( File::FileInfo& fi, void* tag )
{
	ForAddFile* faf = (ForAddFile*)tag;
	StringBuilderStack<MAX_PATH> pathCab;
	fi.path->Substring( pathCab, faf->srcFolder->Len() );
	pathCab.Insert( 0, faf->dstFolder, faf->lenDstFolder );
	Path::AppendFile( pathCab, fi.fd.cFileName );
	faf->cab->AddFile( pathCab, fi.fullName->c_str() );
	return true;
}

bool Cab::AddFolder( const char* dstFolder, const char* srcFolder, bool subdirs )
{
	StringBuilderStack<MAX_PATH> pathSrc(srcFolder);
	Path::RemoveEndSeparator(pathSrc);
	ForAddFile faf;
	faf.cab = this;
	faf.srcFolder = &pathSrc;
	faf.dstFolder = dstFolder;
	faf.lenDstFolder = Str::Len(dstFolder);
	File::ListFiles( pathSrc, "*.*", CallbackAddFile, subdirs, &faf );
	return true;
}

void Cab::Save( const char* fileName )
{
	File::Write( fileName, file.data );
}

bool Cab::Extract( const Mem::Data& from, const char* to )
{
	bool ret = false;
	ERF erf;
	Mem::Zero(erf);
	HFDI hfdi = API(CABINET, FDICreate)( FN_FCIALLOC, FN_FCIFREE, FN_FDIOPEN, FN_FDIREAD, FN_FDIWRITE, FN_FDICLOSE, FN_FDISEEK, cpu80386, &erf );
	int err = 0;
	ExtractFiles files( (Mem::Data&)from );
	FDICABINETINFO cabInfo;
	BOOL isCab = API(CABINET, FDIIsCabinet)( hfdi, (INT_PTR)&files.file1, &cabInfo );
	if( isCab )
	{
		files.file1.Seek( 0, 0 ); //в начало файла
		ExtractInfo ei;
		ei.folder = to;
		char fileName[16];
		Str::ToString( (uint)&files, fileName );
		char cabPath[1]; cabPath[0] = 0;
		API(CABINET, FDICopy)( hfdi, fileName, cabPath, 0, ExtractCabNotify, 0, &ei );
		API(CABINET, FDIDestroy)(hfdi);
		ret = true;
	}
	return ret;

}
