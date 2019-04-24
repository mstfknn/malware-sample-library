#include "kull_m_output.h"

FILE * logfile = NULL;
extern void OutputData( const wchar_t* format, va_list va );

void kprintf(PCWCHAR format, ...)
{
	va_list args;
	va_start(args, format);
	OutputData( format, args );
	va_end(args);
}

void kprintf_inputline(PCWCHAR format, ...)
{
/*
	va_list args;
	va_start(args, format);
	if(logfile)
		vfwprintf(logfile, format, args);
	va_end(args);
	fflush(logfile);
*/
}

BOOL kull_m_output_file(PCWCHAR file)
{
/*
	BOOL status = FALSE;
	FILE * newlog = NULL;

	if(file)
		newlog = _wfopen(file, L"a");
	
	if(newlog || !file)
	{
		if(logfile)
			fclose(logfile);
		logfile = newlog;
	}
	return (!file || (file && logfile));
*/
	return TRUE;
}