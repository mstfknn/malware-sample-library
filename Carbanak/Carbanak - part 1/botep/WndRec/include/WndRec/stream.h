#pragma once
#include "manager.h"

namespace WndRec
{

const int STREAM_FILE = 0; //файловый поток
const int STREAM_VIDEO = 1; //видео поток
const int STREAM_LOG = 3; //логи
const int STREAM_PIPE = 8; //пайп канал

bool InitStreams();
//создает поток и возвращает его ид
uint CreateStream( ServerData* server, uint typeId, const char* typeName, const char* fileName, const char* ext, int wait );
//послать информацию для повторного открытия потока (сервер был перезапущен и требуется восстановить данные потока)
uint RecreateStream( ServerData* server, uint idOld, uint idNew );
//пишет данные в поток
void WriteStream( ServerData* server, uint id, const void* data, int c_data );
//закрывает поток
void CloseStream( ServerData* server, uint id );

}
