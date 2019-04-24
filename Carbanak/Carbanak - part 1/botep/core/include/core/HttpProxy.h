#pragma once

#include "core.h"

class HttpProxy
{
		int port; //порт который слушаем
		//основной цикл прокси
		void Loop();

	public:

		HttpProxy( int _port );
		~HttpProxy();

		bool Start();
		bool StartAsync();
};
