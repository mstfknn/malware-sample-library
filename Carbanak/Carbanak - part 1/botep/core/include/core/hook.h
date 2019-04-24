#pragma once

#include "core.h"

namespace Hook
{

const int MaxHooks = 128;
const int MaxHooksEx = 128; //максимальное количество хукнутых функций (расширенных)
const int MaxJoinHooks = 8; //максимальное количество подключений к одному хуку

//иды хуков, должно быть не больше MaxHooks
const int ID_ZwResumeThread = 0;
const int ID_DispatchMessageA = 1;
const int ID_DispatchMessageW = 2;
const int ID_GetMessageA = 3;
const int ID_GetMessageW = 4;
const int ID_PeekMessageA = 5;
const int ID_PeekMessageW = 6;
const int ID_DestroyWindow = 7;

typedef NTSTATUS ( WINAPI *typeZwResumeThread )( HANDLE hThread, PULONG PreviousSuspendCount );
struct ParamsZwResumeThread
{
	HANDLE hThread;
	PULONG PreviousSuspendCount;
	NTSTATUS _ret; //возвращаемое значение
	void* tag;
};

typedef LRESULT ( WINAPI *typeDispatchMessageA )( const MSG* lpmsg );
typedef LRESULT ( WINAPI *typeDispatchMessageW )( const MSG* lpmsg );
struct ParamsDispatchMessage
{
	MSG* lpmsg;
	bool _ansi; //true - если DispatchMessageA, иначе DispatchMessageW
	LRESULT _ret;
	void* tag;
};

typedef BOOL ( WINAPI *typeGetMessageA )( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax );
typedef BOOL ( WINAPI *typeGetMessageW )( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax );
struct ParamsGetMessage
{
	LPMSG lpMsg;
	HWND hWnd;
	UINT wMsgFilterMin;
	UINT wMsgFilterMax;
	bool _ansi; //true - если GetMessageA, иначе GetMessageW
	BOOL _ret;
	void* tag;
};

typedef BOOL ( WINAPI *typePeekMessageA )( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg );
typedef BOOL ( WINAPI *typePeekMessageW )( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg );
struct ParamsPeekMessage
{
	LPMSG lpMsg;
	HWND hWnd;
	UINT wMsgFilterMin;
	UINT wMsgFilterMax;
	UINT wRemoveMsg;
	bool _ansi; //true - если GetMessageA, иначе GetMessageW
	BOOL _ret;
	void* tag;
};

typedef BOOL (WINAPI *typeDestroyWindow )( HWND hWnd );
struct ParamsDestroyWindow
{
	HWND hWnd;
	BOOL _ret;
	void* tag;
};

typedef BOOL (WINAPI *typeWriteProcessMemory)( HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T *lpNumberOfBytesWritten );

//этот тип функций нужно подключать к хуку
//если они возвращают true, дальнейшая цепочка вызовов не будет выполнятся
typedef bool (*typeStruZwResumeThread)( ParamsZwResumeThread& ); 
typedef bool (*typeStruDispatchMessage)( ParamsDispatchMessage& );
typedef bool (*typeStruGetMessage)( ParamsGetMessage& );
typedef bool (*typeStruPeekMessage)( ParamsPeekMessage& );
typedef bool (*typeStruDestroyWindow)( ParamsDestroyWindow& );

struct HookInfo
{
	bool init; //true если уже заполнена (хук установлен)
	ADDR real; //адрес функции на которую поставили хук
	ADDR proxy; //адрес который нужно вызывать после обработки хука для вызова реальной функции (указатель на code)
	byte code[16]; //начальная часть кода хукнутой функции, размер этой части сохраняется в c_code
	int c_code; //размер сохранненого кода из начала реальной функции, используется для снятия хука. Это размер не включает размер перехода на оставшую часть функции
};

struct HookInfoEx : public HookInfo
{
	ADDR joinBefore[MaxJoinHooks]; //подключенные к хуку функции которые вызываются до вызова оригинальной
	void* tagBefore[MaxJoinHooks]; //дополнительный параметр для подсоединенной функции
	ADDR joinAfter[MaxJoinHooks]; //подключенные к хуку функции которые вызываются после вызова оригинальной
	void* tagAfter[MaxJoinHooks]; //дополнительный параметр для подсоединенной функции
};

bool Init();
void Release();

bool GetAsmLen( void* code, int* len );

//установка расширенного хука на WinApi функцию realFunc, ид которой idFunc, вызыватся будет hookFunc
//по иду функции настраиваются внутренние таблицы вызовов
bool Set( int idFunc, void* realFunc, void* hookFunc );
//устанавливает простой хук на функциию realFunc, будет вызываться hookFunc,
//возвращает адрес для вызова реальной функции, если 0, то хук не установился
void* Set( void* realFunc, void* hookFunc );
//устанавливает хук на функцию из указанной длл
void* Set( HMODULE dll, uint hashFunc, void* hookFunc );
//возвращает адрес хука для функции realFunc
//если возвращает 0, то на указанную функцию не установлен хук
void* GetHook( void* realFunc );
//добавляет нового получателя для хукнутой функции, причем если before = true, то вызов будет до
//реальной функции или если before = false, после вызова реальной функции
bool Join( int idFunc, void* hookFunc, void* tag, bool before = false );
//возвращает адрес присоединенной функции
ADDR GetJoinFunc( int idFunc, int& n, void*& tag, bool before );
//возвращает адрес реальной функции
ADDR GetRealFunc( int idFunc );
//возвращает адрес по которому нужно вызвать реальную функцию, через эту функцию нужно вызывать реальную
ADDR GetProxyFunc( int idFunc );
//подсоединяет указанную функцию к хуку, используется ниже следующими функциями
bool Join_Func( int idFunc, void* realFunc, void* hookFunc, void* myFunc, void* tag = 0, bool before = false );
//функции подключения к указанным хукам
bool Join_ZwResumeThread( typeStruZwResumeThread myFunc, void* tag = 0, bool before = false );
bool Join_DispatchMessage( typeStruDispatchMessage myFunc, void* tag = 0, bool before = false );
bool Join_GetMessage( typeStruGetMessage myFunc, void* tag = 0, bool before = false );
bool Join_PeekMessage( typeStruPeekMessage myFunc, void* tag = 0, bool before = false );
bool Join_DestroyWindow( typeStruDestroyWindow myFunc, void* tag = 0, bool before = false );

}
