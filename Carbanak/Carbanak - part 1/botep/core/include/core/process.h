#pragma once

#include "core.h"
#include "TlHelp32.h"

namespace Process
{

#define MAIN_USER ((char*)0xffffffff)

//если указа в options для функции Exec, то запускает процесс с правами текущего юзера
//const DWORD EXEC_USER = 0x8000000;
//при открытии процесса не показывать окно
const DWORD EXEC_NOWINDOW = 0x40000000;

//информация о процессе при перечислении процессов
struct ProcessInfo
{
	StringBuilderStack<MAX_PATH> fullPath; //полный путь к процессу в нижнем регистре
	StringBuilderStack<64> fileName; //имя файла процесса в нижнем регистре
	DWORD pid; 
	DWORD parentPid;
	uint hash; //хеш имени файла процесса
};

//вызывается функцией ListProcess, если возвращает true, то останавливает дальнейшее перечисление
//tag - данные передаваемые функцией запустившей ListProcess()
typedef bool (*typeListProcessCallback)( ProcessInfo&, void* tag );

//запуск процесса, options - это опции для функции CreateProcess, cmd - запускаемый процесс с аргументами
//возвращает pid процесса или 0, если запустить не удалось
//также через параметры возвращает хендл процесса, хендл потока hthread, код который вернул процесс exitCode, wait - сколько в миллисекундах ждать
//завершение процесса, если 0, то ждать не нужно
//nameUser - под каким юзером нужно запускать, если 0, то под тем под каким запущен бот, если = DefUser, то запускает под юзером у которого запущен проводник
DWORD Exec( DWORD options, const char* nameUser, HANDLE* hprocess, HANDLE* hthread, DWORD* exitCode, int wait, const char* cmd, ... );
//запуск процесса с ожиданием
DWORD Exec( DWORD* exitCode, int wait, const char* cmd, ... );
DWORD Exec( DWORD options, DWORD* exitCode, int wait, const char* cmd, ... );
//запуск процесса без ожидания
DWORD Exec( DWORD options, const char* nameUser, const char* cmd, ... );
DWORD Exec( const char* cmd, ... );
//PID текущего процесса
DWORD CurrentPID();
//хеш имени текущего процесса (имя в нижнем регистре)
uint CurrentHash();
//имя текущего процесса
StringBuilder& Name( StringBuilder& name );
//имя процесса по его pid, если full = true, то возвращает полный путь
StringBuilder& Name( DWORD pid, StringBuilder& name, bool full = false );
//возвращает PID запущенного процесса, если имя процесса полное (полное имя файла), то full должно быть равно true
DWORD GetPID( const char* nameProcess, bool full = false );
//возвращает PID процесса которому принадлежит окно
DWORD GetPID( HWND hwnd );
//возвращает PID процесса по хендлу его потока
DWORD GetPID( HANDLE hthread );
//перечисление всех запущенных процессов, возвращает количество перечисленных процессов
int ListProcess( typeListProcessCallback func, void* tag, DWORD typeSnap = TH32CS_SNAPPROCESS );
//возвращает PID процесса explorer
DWORD GetExplorerPID();
//закрывает процесс по его pid
bool Kill( DWORD pid, int wait );
//закрывает процесс по его имени
bool Kill( const char* nameProcess, int wait );
//запуск процесса из системного процесса под текущим пользователем
bool ExecAsCurrUser( const char* cmd );
//отсылает сообщение процессу окну hwnd которое принадлежит другому процессцу
bool SendMsg( HWND hwnd, DWORD msg, WPARAM wparam, LPARAM lparam );
bool SendMsg( HWND hwnd, MSG* msgs, int c_msgs );
//убивает процессы которые держат открытым указанный файл
void KillBlocking( const char* fileName );
//удаляет процессы которые загрузили указанную dll
void KillLoadedModule( const char* nameModule );
//возвращает true если процесс еще работает
bool IsAlive( HANDLE hprocess );
//возвращает командную строку с параметрами указанного процесса
bool GetCommandLine( uint pid, StringBuilder& s );

};

