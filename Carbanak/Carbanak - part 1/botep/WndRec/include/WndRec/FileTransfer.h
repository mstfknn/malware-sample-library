#pragma once

#include "manager.h"

namespace WndRec
{

bool FT_Init();
void FT_Dir( ServerData* server, const byte* data, int c_data );
//копирование файла с сервера на бот
void FT_CopyServer2Bot( ServerData* server, const byte* data, int c_data );
//копирование файла с бота на сервер
void FT_CopyBot2Server( ServerData* server, const byte* data, int c_data );
void FT_StopCopy();
void FT_DelFile( ServerData* server, const byte* data, int c_data );
void FT_MkDir( ServerData* server, const byte* data, int c_data );

}
