#pragma once

#include "manager.h"

namespace WndRec
{

//сокет и его ид 
struct SocketId
{
	SOCKET sc;
	uint id;
	char ip[24];
	int port;
	byte type; //0 - данные по протоколу (передаются специальной функции), 1 - простые данные
	DWORD val; //какая-то информация в зависимости от протокола
};

//специфическая функция обработки данных по определенному протоколу, возвращает
//0 - если обработка успешна, 1 - ошибка (закрыть туннель), 2 - добавить новый туннель
typedef int (*ProtocolHandler)( ServerData* server, SocketId*, char* data, int szData, char* createData, int szCreate );

int FromSocketsToServer( ServerData* server, SocketId* sockets, int c_sockets, int cmdServer, int wait );
int FromServerToSockets( ServerData* server, SocketId* sockets, int& c_sockets, int cmdServer, ProtocolHandler );

//отсылает данные по протоколу серверу 
//вызываться должна из ProtocolHandler для ответа серверу согласно протокола
void SendProtocolData( ServerData* server, SocketId* si, int cmdServer, char* data, int szData );
//закрывает все открытые сокеты
void CloseAllSockets( ServerData* server, SocketId* sockets, int c_sockets, int cmdServer );
//после закрытия сокетов их становится меньше, функция уменьшает их количество
//функцию желательно вызывать после операций чтения и записи
int CompactSockets( SocketId* sockets, int c_sockets );

void InitTunnels();
bool StartTunnel( ServerData* server, int cmd, ProtocolHandler pfunc );
void StopTunnel( ServerData* server, int cmd );

}
