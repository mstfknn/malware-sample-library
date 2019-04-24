#pragma once

#include "core\core.h"

//функции которые нужно реализовывать в конечных проектах,
//тут даются описание функций и варианты их реализации

namespace Abstract
{

//возвращает uid уникальный для компа
bool GetUid( StringBuilder& uid ); //эту функцию нужно реализовать
bool _GetUid1( StringBuilder& uid ); //вариант реализации (ид вычисляется на основе mac адреса), в uid должен вмещать не менее 10 символов

//массив байт используемый для шифрования в процессе обмена данными (например в pipe)
extern const byte* XorVector;
extern int SizeXorVector;

}
