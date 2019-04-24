#include <windows.h>

#define C_ERROR         0xFFFFFFFF
#define C_PREFIX        0x00000001
#define C_66            0x00000002
#define C_67            0x00000004
#define C_DATA66        0x00000008
#define C_DATA1         0x00000010
#define C_DATA2         0x00000020
#define C_DATA4         0x00000040
#define C_MEM67         0x00000080
#define C_MEM1          0x00000100
#define C_MEM2          0x00000200
#define C_MEM4          0x00000400
#define C_MODRM         0x00000800
#define C_DATAW0        0x00001000
#define C_FUCKINGTEST   0x00002000
#define C_TABLE_0F      0x00004000

void GetInstLenght(DWORD* myiptr0, DWORD* osizeptr);
