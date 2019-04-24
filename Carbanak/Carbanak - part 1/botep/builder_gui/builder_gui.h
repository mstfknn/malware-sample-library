
// builder_gui.h : главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// Cbuilder_guiApp:
// О реализации данного класса см. builder_gui.cpp
//

class Cbuilder_guiApp : public CWinApp
{
public:
	Cbuilder_guiApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern Cbuilder_guiApp theApp;