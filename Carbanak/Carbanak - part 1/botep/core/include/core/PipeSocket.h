#pragma once

#include "pipe.h"

class PipeSocketClient;

class PipeSocketServer : public PipeServer
{
		virtual int Handler( Pipe::Msg* msgIn, void** msgOut );
		virtual void Disconnect();

		#pragma warning ( disable : 4200 )
		struct DataConnect
		{
			char ip[64];
			int port;
		};

		struct DataRecv
		{
			int sc;
			int maxRead;
			int wait;
		};

		struct DataSend
		{
			int sc;
			int c_data;
			byte data[0];
		};

		struct DataRecvAnswer
		{
			int res;
			int c_data;
			byte data[0];
		};

		char ip[48]; //куда коннектиться по умолчанию
		int port;
		Mem::Data result;

	public:

		enum
		{
			CmdConnect, 
			CmdRecv,
			CmdSend,
			CmdClose
		};

		PipeSocketServer( const char* pipeName, const char* _ip, int _port );
		~PipeSocketServer();

	friend class PipeSocketClient;
};

class PipeSocketClient : public PipeClient
{
	public:

		PipeSocketClient( const char* pipeName, const char* user, const char* password );
		//функции аналогичны из socket.h, их описание там
		int ConnectIP( const char* ip, int port );
		int Write( int sc, const void* data, int c_data );
		int Read( int sc, Mem::Data& data, int max_read, int wait = 0 ); 
		void Close( int sc );
};
