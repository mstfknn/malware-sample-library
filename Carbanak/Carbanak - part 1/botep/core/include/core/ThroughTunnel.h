#pragma once

#include "core.h"
#include "misc.h"

//сквозной туннель из сокетов
//пропускает трафик через себ€

class ThroughTunnel
{
		int portIn; //порт подключени€

	protected:

		char ipOut[16]; //адрес куда подключаемс€
		int portOut; //порт куда подключаемс€

		//вызываетс€ когда приходит вход€щие соединени€ и до того как будет сделано соединени€ в ipPort
		//возвращает исход€щий сокет, если 0, то осуществл€етс€ соединение с ipPort
		virtual int Connected( int sc );

	private:

		void Loop();

	public:

		ThroughTunnel( int _portIn, const char* _ipOut, int _portOut );
		~ThroughTunnel();

		bool Start();
		bool StartAsync();

};

