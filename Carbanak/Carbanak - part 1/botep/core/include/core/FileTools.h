#pragma once

#include "File.h"

namespace File
{

struct PatchData
{
	byte* find; //последовательность которую нужно найти
	int c_find; //количество байт в find
	byte* replace; //что нужно вставить
	int c_replace; //количество байт в replcace
	int offset; //смещение от позиции в которой найден find, с этого смещения и вставляется replace
};

//если возвращает true, то файл сохранится, если false, то ошибка и PatchExe() вернет -2
typedef bool (*typeCallbackAfterPatch)( Mem::Data& data );

//патчит указанный файл, возвращает количество найденных данных из patch
//конец пачинга определяется по patch->find = 0
//если возвращает 0, то ничего не найдено, -1 - если такого файла нет или его нельзя изменить, -2 - cb вернула false
int Patch( const char* fileNameFrom, const char* fileNameTo, PatchData* patch, typeCallbackAfterPatch cb = 0 );
//Патчит exe или dll файл при этом устанавливает новую контрольную сумму в PE заголовке
int PatchExe( const char* fileNameFrom, const char* fileNameTo, PatchData* patch );

struct MonitoringStru
{
	const char* fileName; //файл который мониторится
	StringBuilder* fullName; //полное имя файла, устанавливается функцией Monitoring()
	DWORD tag; //какая-то дополнительная информация
	DWORD action; //FILE_NOTIFY_INFORMATION.Action
};

//если возвращает false, то мониторинг останавливается
typedef bool (*typeCallbackMonitoring)( MonitoringStru* );

//возвращает 0 - функция cb возвратила false (остановка мониторинга), <0 - какая-то ошибка
int Monitoring( const char* path, MonitoringStru* files, int c_files, typeCallbackMonitoring cb, int wait = INFINITE );

}
