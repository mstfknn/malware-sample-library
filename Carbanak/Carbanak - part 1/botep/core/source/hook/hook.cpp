#include "core\hook.h"
#include "x86\zdisasm.h"
#include "core\debug.h"

namespace Hook
{

const int SizeOfJump = 5;

HookInfoEx* tableHooksEx = 0; //таблица расширенных хуков
const int SizeTableHooksEx = sizeof(HookInfoEx) * MaxHooksEx; //размер таблицы в байтах
HookInfo* tableHooks = 0; //таблица простых хуков
int freeNumHook = 0; //свободна€ позици€ дл€ простого хука
const int SizeTableHooks = sizeof(HookInfo) * MaxHooksEx;

bool Init()
{
	tableHooksEx = (HookInfoEx*)API(KERNEL32, VirtualAlloc)( 0, SizeTableHooksEx, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE );
	if( tableHooksEx )
	{
		Mem::Set( tableHooksEx, 0, SizeTableHooksEx );
		tableHooks = 0;
		freeNumHook = 0;
		return true;
	}
	return false;
}

void Release()
{
	API(KERNEL32, VirtualFree)( tableHooksEx, 0, MEM_RELEASE );
	if( tableHooks ) API(KERNEL32, VirtualFree)( tableHooks, 0, MEM_RELEASE );
}

bool GetAsmLen( void* code, int* len )
{
	DWORD size = 0;
	*len = 0;
	GetInstLenght( (DWORD*)code, &size );
	if( size == C_ERROR )
		return false;
	*len = size;
	return true;
}

static bool Set( HookInfo* item, void* realFunc, void* hookFunc )
{
	int modifedSize = 0; //размер кода который будет перенесен и на место которого будет сто€ть jump
	item->init = false;
	byte* modifedSpace = (byte*)realFunc;
    do
	{
		int size;
		GetAsmLen( modifedSpace, &size );
		modifedSpace += size;
		modifedSize += size;
	} while( modifedSize < SizeOfJump );

	int sizeCode = modifedSize + SizeOfJump;
	if( sizeCode < sizeof(item->code) )
	{
		//копируем начало апи функции
		Mem::Copy( item->code, realFunc, modifedSize );
		item->c_code = modifedSize;
		//делаем переход на оставшую часть функции
		item->code[modifedSize] = 0xe9; //команда jump с длинным переходом
		ADDR rel = (ADDR)realFunc -  (ADDR)(item->code + SizeOfJump); //смещение дл€ перехода по команде jump
		*((ADDR*)(item->code + modifedSize + 1)) = rel;
		Mem::Set( item->code + sizeCode, 0x90, sizeof(item->code) - sizeCode ); //заполн€ем оставшее пространство командами nop
		//модифицируем начало апи функции
		DWORD oldProtect;
		if( API(KERNEL32, VirtualProtect)( realFunc, modifedSize, PAGE_EXECUTE_READWRITE, &oldProtect ) )
		{
			//выставл€ем переход на нашу функцию
			byte* ptr = (byte*)realFunc;
			*ptr = 0xe9;
			rel = (byte*)hookFunc - (ptr + SizeOfJump);
			*((ADDR*)(ptr + 1)) = rel;
			Mem::Set( ptr + SizeOfJump, 0x90, modifedSize - SizeOfJump ); //выставл€ем nop после перехода, если нужно
			API(KERNEL32, VirtualProtect)( realFunc, modifedSize, oldProtect, &oldProtect ); //восстанавливаем защиту страницы
			item->real = (ADDR)realFunc;
			item->proxy = (ADDR)item->code;
			item->init = true;
		}
	}
	return item->init;
}

bool Set( int idFunc, void* realFunc, void* hookFunc )
{
	if( idFunc < 0 || idFunc >= MaxHooksEx ) return false;
	HookInfoEx* item = &tableHooksEx[idFunc];
	if( !item->init )
	{
		Set( item, realFunc, hookFunc );
	}
	return item->init;
}

void* Set( void* realFunc, void* hookFunc )
{
	if( !realFunc || !hookFunc ) return nullptr;
	void* ret = nullptr;
	if( tableHooks == 0 )
	{
		tableHooks = (HookInfo*)API(KERNEL32, VirtualAlloc)( 0, SizeTableHooks, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE );
		if( tableHooks )
		{
			Mem::Set( tableHooks, 0, SizeTableHooks );
			freeNumHook = 0;
		}
		else
			return nullptr;
	}
	if( Set( &tableHooks[freeNumHook], realFunc, hookFunc ) )
	{
		ret = (void*)tableHooks[freeNumHook].proxy;
		freeNumHook++;
	}
	return ret;
}

void* Set( HMODULE dll, uint hashFunc, void* hookFunc )
{
	void* realFunc = WinAPI::GetApiAddr( dll, hashFunc );
	return Set( realFunc, hookFunc );
}

void* GetHook( void* realFunc )
{
	if( tableHooks )
	{
		for( int i = 0; i < freeNumHook; i++ )
			if( tableHooks[i].real == (ADDR)realFunc )
				return (void*)tableHooks[i].proxy;
	}
	return nullptr;
}

bool Join( int idFunc, void* hookFunc, void* tag, bool before )
{
	if( idFunc < 0 || idFunc >= MaxHooksEx ) return false;
	HookInfoEx* item = &tableHooksEx[idFunc];
	if( !item->init ) return false;
	ADDR* join =  before ? item->joinBefore : item->joinAfter;
	void** tags = before ? item->tagBefore : item->tagAfter;
	//ищем €чейку где сохраним адрес функции
	for( int i = 0; i < MaxJoinHooks; i++ )
		if( join[i] == 0 ) //свободна€ €чейка
		{
			join[i] = (ADDR)hookFunc;
			tags[i] = tag;
			return true;
		}
	return false;
}

ADDR GetJoinFunc( int idFunc, int& n, void*& tag, bool before )
{
	ADDR ret = 0;
	HookInfoEx* item = &tableHooksEx[idFunc];
	ADDR* join =  before ? item->joinBefore : item->joinAfter;
	void** tags = before ? item->tagBefore : item->tagAfter;
	for( ; n < MaxJoinHooks; n++ )
	{
		if( join[n] )
		{
			ret = join[n];
			tag = tags[n];
			n++;
			break;
		}
	}
	return ret;
}

ADDR GetRealFunc( int idFunc )
{
	return tableHooksEx[idFunc].real;
}

ADDR GetProxyFunc( int idFunc )
{
	return tableHooksEx[idFunc].proxy;
}

bool Join_Func( int idFunc, void* realFunc, void* hookFunc, void* myFunc, void* tag, bool before )
{
	if( idFunc < 0 || idFunc >= MaxHooksEx ) return false;
	HookInfo* item = &tableHooksEx[idFunc];
	if( !item->init )
	{
		if( !Set( idFunc, realFunc, hookFunc ) )
			return false;
	}
	return Join( idFunc, myFunc, tag, before ); 
}

}
