#pragma once

#include "WndRec\manager.h"
#include "WndRec\WndRec.h"

namespace WndRec
{

typedef DWORD (WINAPI *typeCallbackCmd)( ServerData* server, DWORD cmd, uint id, char* inData, int lenInData, char* outData, int szOutData, DWORD* lenOutData );

bool RunCmdExec( ServerData* server, typeCallbackCmd func );

}
