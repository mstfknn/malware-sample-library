#pragma once

#include "core.h"

namespace Crypt
{

bool Init();
void Release();

//конвертирование в base64
bool ToBase64( const void* data, int c_data, StringBuilder& s );
inline bool ToBase64( const Mem::Data& data, StringBuilder& s )
{
	return ToBase64( data.Ptr(), data.Len(), s );
}
//конвертирование из base64
bool FromBase64( const StringBuilder& s, Mem::Data& data );
//конвертирует им€ в случайный набор символов (шифрует), т. е. на им€ накладываетс€ xor маска и результат конвертируетс€ в base64
//таким образом формируем шифруем им€ уникальное дл€ данного компа
StringBuilder& Name( const char* name, const char* xor, StringBuilder& s );
//шифрует им€ файла, при этом путь и расширение остаютс€ прежними
StringBuilder& FileName( const char* xor, StringBuilder& fileName );
//шифрование алгоритмом RC2 данных из data
bool EncodeRC2( const char* psw, const char* IV, Mem::Data& data );
//рсшифровка алгоритмом RC2 данных из data
bool DecodeRC2( const char* psw, const char* IV, Mem::Data& data );

//кодирует декодирует данные использу€ Abstract::XorVector
void EncodeDecodeXorVector( void* data, int c_data );
inline void EncodeDecodeXorVector( Mem::Data& data )
{
	EncodeDecodeXorVector( data.Ptr(), data.Len() );
}

}
