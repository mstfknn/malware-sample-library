#pragma once

#include "manager.h"
#include "stream.h"

namespace WndRec
{

bool SendFile( ServerData* server, const char* typeName, const char* fileName, const char* ext, const void* data, int c_data );
//отправляет запрос на сервер, чтобы он выслал указанный плагин, возвращает ид по которому через CmdExec можно определить присланный плагин
uint LoadPluginAsync( ServerData* server, const char* namePlugin);

}
