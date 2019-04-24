#pragma once

#include "core\core.h"

//генератор случайных чисел реализован методом ‘ибоначчи с запаздыванием, его параметры (17,5)
namespace Rand
{

void Init();
uint Gen( int min, int max );
inline uint Gen( int max )
{
	return Gen( 0, max );
}
StringBuilder& Gen( StringBuilder& s, int min, int max );
inline StringBuilder& Gen( StringBuilder& s, int len )
{
	return Gen( s, len, len );
}
//генерирует size случайных чисел в интервале 0 - 255
void Gen( byte* buf, int size );

//‘ункци€ обспечивает выбор на основе указанной веро€тности
//генерирует число от 0 до 100 и если оно меньше v, то возвращает true, иначе false
bool Condition( int v );

}
