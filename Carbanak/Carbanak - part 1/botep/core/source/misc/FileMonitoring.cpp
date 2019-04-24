#include "core\FileTools.h"

namespace File
{

int Monitoring( const char* path, MonitoringStru* files, int c_files, typeCallbackMonitoring cb, int wait )
{
	HANDLE dir = API(KERNEL32, CreateFileA)( path, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, 0 );
	if( dir == INVALID_HANDLE_VALUE ) return -1;
	bool stop = false;
	void* buf = Mem::Alloc(2048);
	OVERLAPPED overlapped;
	Mem::Zero(overlapped);
	overlapped.hEvent = API(KERNEL32, CreateEventA)( 0, 0, 0, 0 ); //будем аснхронно следить, чтобы не висел поток
	StringBuilder fullName(MAX_PATH), fileName(MAX_PATH);
	int ret = -4;
	while( !stop )
	{
		//мониторим нужну папку
		DWORD size;
		if( !API(KERNEL32, ReadDirectoryChangesW)( dir, buf, 2048, FALSE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_SIZE, &size, &overlapped, 0 ) )
		{
			ret = -2;
			break;
		}
		//ждем события создания или изменения файла
		DWORD res = API(KERNEL32, WaitForSingleObject)( overlapped.hEvent, wait );
		if( res == WAIT_OBJECT_0 ) //что-то произошло
		{
			FILE_NOTIFY_INFORMATION* info = (FILE_NOTIFY_INFORMATION*)buf;
			for(;;)
			{
				size = WStr::ToWin1251( info->FileName, info->FileNameLength / 2, fileName.c_str(), fileName.Size() );
				fileName.SetLen(size);
				fileName.Lower();
				if( c_files > 0 )
				{
					for( int i = 0; i < c_files; i++ )
					{
						if( Str::Cmp( files[i].fileName, fileName ) == 0 )
						{
							Path::Combine( fullName, path, files[i].fileName );
							files[i].fullName = &fullName;
							files[i].action = info->Action;
							if( !cb( &files[i] ) ) 
							{
								stop = true;
								ret = 0;
								break;
							}
						}
					}
				}
				else
				{
					MonitoringStru m;
					Mem::Zero(m);
					Path::Combine( fullName, path, fileName );
					m.fullName = &fullName;
					m.action = info->Action;
					if( !cb( &m ) ) 
					{
						stop = true;
						ret = 0;
						break;
					}
				}
				if(	!info->NextEntryOffset ) break;
				info = (FILE_NOTIFY_INFORMATION*)((BYTE*)info + info->NextEntryOffset);
			}
		}
		else if( res == WAIT_TIMEOUT )
		{
			if( !cb(0) )
			{
				ret = 0;
				break;
			}
		}
		else
		{
			ret = -3;
			break;
		}
	}
	Mem::Free(buf);
	API(KERNEL32, CloseHandle)(overlapped.hEvent);
	return ret;
}

}
