#include "core\core.h"
#include "WndRec\manager.h"

namespace WndRec
{

DWORD SendLog( ServerData* server, const char* name, int code, const char* text )
{
	int lenName = name ? Str::Len(name) : 0;
	int lenText = text ? Str::Len(text) : 0;
	SYSTEMTIME tm;
	API(KERNEL32, GetLocalTime)(&tm);
	Package* p = AllocPackage( server, 7 /*дата*/ + 2 /*длина имени*/ + lenName /*имя*/ + 2 /*код*/+ 2 /*длина текста*/ + lenText /*текст*/);
	char* data = (char*)p->data.Ptr();
	int i = 0;
	//формируем пакет
	*((short*)data + i) =  tm.wYear; i += 2;
	data[i] = (char)tm.wMonth; i++;
	data[i] = (char)tm.wDay; i++;
	data[i] = (char)tm.wHour; i++;
	data[i] = (char)tm.wMinute; i++;
	data[i] = (char)tm.wSecond; i++;
	*((short*)(data + i)) = lenName; i+= 2;
	Mem::Copy( data + i, name, lenName ); i += lenName;
	*((short*)(data + i)) = code; i += 2;
	*((short*)(data + i)) = lenText; i += 2;
	Mem::Copy( data + i, text, lenText ); i += lenText;
	p->data.SetLen(i);

	return WritePackage( server, ID_LOG, p );
}

}
