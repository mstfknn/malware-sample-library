#include "core\process.h"
#include "core\file.h"
#include "core\debug.h"

namespace Process
{

#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)

static void* GetInfoTable( DWORD dwTableType )
{
	int size = 0x8000;
	NTSTATUS status = 0;
	void* ptr = 0;
	for(;;)
	{
		ptr = Mem::Alloc(size);
		if( !ptr ) break;
		status = API(NTDLL, ZwQuerySystemInformation)( (SYSTEMINFOCLASS) dwTableType, ptr, size, NULL );
		if( status != STATUS_INFO_LENGTH_MISMATCH ) break;
		Mem::Free(ptr);
		size *= 2;
	};
	if( status == STATUS_SUCCESS )
		return ptr;
	Mem::Free(ptr);
	return 0;
}

static UCHAR GetFileHandleType()
{
	UCHAR result = 0;
	HANDLE hfile = File::Open( _CS_("NUL"), GENERIC_READ, OPEN_EXISTING );
	if( hfile )
	{
		PSYSTEM_HANDLE_INFORMATION info = (PSYSTEM_HANDLE_INFORMATION)GetInfoTable(SystemHandleInformation);
		if( info )
		{
			DWORD currPid = CurrentPID();
			for( uint r = 0; r < info->uCount; r++ )
			{
				if( info->aSH[r].Handle == (USHORT)hfile && info->aSH[r].uIdProcess == currPid )
				{
					result = info->aSH[r].ObjectType;
					break;
				}
			}
		    Mem::Free(info);
		}
		API(KERNEL32, CloseHandle)(hfile);
	}
	return result;
}

struct QueryInformationFile
{
	HANDLE hfile;
	FILE_NAME_INFORMATION fni;
	wchar_t buf[MAX_PATH];
};

static DWORD WINAPI ZwQueryInformationFileThread( void* data )
{
	IO_STATUS_BLOCK ioStatus;
	QueryInformationFile* qif = (QueryInformationFile*)data;
	qif->fni.FileName[0] = 0;
	if( API(NTDLL, ZwQueryInformationFile)( qif->hfile, &ioStatus, &qif->fni, sizeof(QueryInformationFile) - sizeof(HANDLE), FileNameInformation ) != STATUS_SUCCESS )
		qif->fni.FileName[0] = 0;
	return 0;
}

static int FindBlockingProcesses( const char* fileName, uint* PIDs, int c_PIDs )
{
	int procCount = 0;
	UCHAR obFileType = GetFileHandleType();
	PSYSTEM_HANDLE_INFORMATION info = (PSYSTEM_HANDLE_INFORMATION)GetInfoTable(SystemHandleInformation);
	if( info )
	{
		int c_fileName = Str::Len(fileName);
		HANDLE currProcess = API(KERNEL32, GetCurrentProcess)();
		QueryInformationFile* qif = (QueryInformationFile*)Mem::Alloc( sizeof(QueryInformationFile) );
		StringBuilder fileNameHandle;
		for( uint r = 0; r < info->uCount; r++)
		{
			if( info->aSH[r].ObjectType == obFileType )
			{
				HANDLE hprocess = API(KERNEL32, OpenProcess)( PROCESS_DUP_HANDLE, FALSE, info->aSH[r].uIdProcess );
				if( hprocess )
				{
					HANDLE hfile;
					if( API(KERNEL32, DuplicateHandle)( hprocess, (HANDLE)info->aSH[r].Handle, currProcess, &hfile, 0, FALSE, DUPLICATE_SAME_ACCESS) )
					{
						qif->hfile = hfile;
						HANDLE thread = RunThread( ZwQueryInformationFileThread, qif, 0 );
						if( API(KERNEL32, WaitForSingleObject)( thread, 300 ) == WAIT_TIMEOUT ) 
						{
							API(KERNEL32, TerminateThread)( thread, 0 );
							qif->fni.FileName[0] = 0;
						}
						API(KERNEL32, CloseHandle)(thread);
						if( qif->fni.FileName[0] )
						{
							fileNameHandle.ToWin1251( qif->fni.FileName, qif->fni.FileNameLength / 2 );
							fileNameHandle.Lower();
							if( fileNameHandle.IndexOf( fileName, c_fileName ) >= 0 )
							{
								PIDs[procCount++] = info->aSH[r].uIdProcess;
								if( procCount >= c_PIDs ) break;
							}
						}
						API(KERNEL32, CloseHandle)(hfile);
					}
					API(KERNEL32, CloseHandle)(hprocess);
				}
			}
		}
		Mem::Free(qif);
		Mem::Free(info);
	}
	return procCount;
}

//уничтожает процессы которые держат указанный файл
void KillBlocking( const char* fileName )
{
	uint pids[10];

	int counter = 0; //счетчик попыток уничтожения процессов
	int countProcess;
	while( ( countProcess = FindBlockingProcesses( fileName, pids, 10 ) ) && counter < 3 )
	{
		counter++;
		while( countProcess-- )
		{
			Kill( pids[countProcess], 1000 );
		}
	};	
}

}
