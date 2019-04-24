#pragma once

//запуск приложения или dll в памяти

#include "core.h"

namespace RunInMem
{

//запуск exe файла в памяти
bool RunExe( const void* module, int szModule );
//запуск dll в памяти, возвращает хендл этой длл для получения адресов функций
HMODULE RunDll( const void* module, int szModule, void* thirdParam = 0 );
void FreeDll( HMODULE module );

}
