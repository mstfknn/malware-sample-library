#include "builder.h" //настройки бота для билдера
#include "core\core.h"

struct Config
{
	StringBuilder prefix;
	StringArray hosts;
	StringBuilder proxy;
	StringArray hosts_az;
	StringBuilder user_az;
	StringArray video_hosts;
	bool video_svchost;
	bool video_run; 
	int video_timeout;
	bool autorun;
	bool sployty;
	bool check_dublication;
	bool plugins_server;
	bool not_used_svchost;

	StringBuilder password;
	int period;
	byte RandVector[MaxSizeRandVector];
	uint A, B, R; //A B коэффициенты генератора случайных чисел, R текущее значение случайного числа
	Mem::Data publicKey, privateKey, encodedKey;
	StringBuilder dateWork;
};

Config* ReadConfig( const char* fileName );
bool SetConfig( Config* cfg, Mem::Data& data );
//генерация таблицы перекодировки, используется для кодирования строк
void CreateEncodeTable( Config* cfg );
//кодирование строк заключенных символами CS...ES, возвращает количество закодированных строк
int EncodeAllStrings( Config* cfg, Mem::Data& data );
//кодирование строки s, результат помещается в buf, размер buf должен быть достаточным (равен длине строки + CountStringOpcode + \0)
int EncodeString( const char* s, char* buf, int c_s = -1 );
void SetRandVector( Config* cfg, Mem::Data& data );
bool GenRSAKeys( Mem::Data& publicKey, Mem::Data& privateKey );
void EncodeRSAKey( Mem::Data& key, Mem::Data& res );