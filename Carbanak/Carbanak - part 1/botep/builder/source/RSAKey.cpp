#include "stdafx.h"
#include "main.h"

bool ExportKey( HCRYPTPROV hprov, HCRYPTKEY hkey, DWORD type, Mem::Data& key )
{
	DWORD size;
	bool ret = false;
	if( CryptExportKey( hkey, 0, type, 0, 0, &size ) )
	{
		if( key.MakeEnough( size - key.Len() ) )
		{
			if( CryptExportKey( hkey, 0, type, 0, (BYTE*)key.Ptr(), &size ) )
			{
				key.SetLen(size);
				ret = true;
			}
		}
	}
	return ret;
}

bool GenRSAKeys( Mem::Data& publicKey, Mem::Data& privateKey )
{
	bool ret = false;
	HCRYPTPROV hprov;
	if( CryptAcquireContextA( &hprov, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) )
	{
		HCRYPTKEY hkey;
		if( CryptGenKey( hprov, AT_KEYEXCHANGE, CRYPT_ARCHIVABLE | (RSA1024BIT_KEY >> 1), &hkey ) )
		{
			if( ExportKey( hprov, hkey, PUBLICKEYBLOB, publicKey ) &&
				ExportKey( hprov, hkey, PRIVATEKEYBLOB, privateKey ) )
			{
				ret = true;
			}
			CryptDestroyKey(hkey);
		}
		CryptReleaseContext(hprov, 0 );
	}
	return ret;
}

void EncodeRSAKey( Mem::Data& key, Mem::Data& res )
{
	res.MakeEnough( key.Len() + 4 );
	byte mask[2];
	mask[0] = rand() % 0xff;
	mask[1] = rand() % 0xff;
	byte* p = res.p_byte();
	p[0] = mask[0];
	p[1] = mask[1];
	p[2] = key.Len();
	Mem::Copy( p + 3, key.p_byte(), key.Len() );
	int xor = 0;
	for( int i = 0; i <= key.Len(); i++ )
	{
		p[2 + i] ^= mask[xor++];
		if( xor == sizeof(mask) ) xor = 0;
	}
	res.SetLen(key.Len() + 3);
}
