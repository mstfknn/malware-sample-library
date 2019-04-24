#include "WndRec\file.h"
#include "WndRec\manager.h"
#include "core\debug.h"

namespace WndRec
{

struct KeyBlobStru
{
	BLOBHEADER header;
    DWORD size;
	BYTE data[64];
};

bool EncryptRSA( const Mem::Data& key, Mem::Data& data )
{
	HCRYPTPROV hprov;
	bool ret = false;
	if( API(ADVAPI32, CryptAcquireContextA)( &hprov, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) )
	{
		HCRYPTKEY hkey;
		if( API(ADVAPI32, CryptImportKey)( hprov, (BYTE*)key.Ptr(), key.Len(), 0, 0, &hkey ) )
		{
			DWORD resSize = data.Len(); ;
			if( API(ADVAPI32, CryptEncrypt)( hkey, 0, TRUE, 0, 0, &resSize, 0 ) )
			{
				if( data.MakeEnough( resSize - data.Len() ) )
				{
					DWORD size = data.Len();
					API(ADVAPI32, CryptEncrypt)( hkey, 0, TRUE, 0, (BYTE*)data.Ptr(), &size, resSize );
					data.SetLen(resSize);
					ret = true;
				}
			}
			API(ADVAPI32, CryptDestroyKey)(hkey);
		}
		API(ADVAPI32, CryptReleaseContext)(hprov, 0 );
	}
	return ret;
}

bool EncryptAES256( const Mem::Data& key, Mem::Data& data )
{
	HCRYPTPROV hprov;
	bool ret = false;
	if( API(ADVAPI32, CryptAcquireContextA)( &hprov, 0, 0, PROV_RSA_AES, CRYPT_VERIFYCONTEXT ) )
	{
		HCRYPTKEY hkey;
		KeyBlobStru blob;
		blob.header.aiKeyAlg = CALG_AES_256;
		blob.header.bType = PLAINTEXTKEYBLOB;
		blob.header.bVersion = CUR_BLOB_VERSION;
		blob.size = key.Len();
		Mem::Copy( blob.data, key.Ptr(), key.Len() );
		if( API(ADVAPI32, CryptImportKey)( hprov, (BYTE*)&blob, sizeof(blob), 0, 0, &hkey ) )
		{
			DWORD resSize = data.Len(); ;
			if( API(ADVAPI32, CryptEncrypt)( hkey, 0, TRUE, 0, 0, &resSize, 0 ) )
			{
				if( data.MakeEnough( resSize - data.Len() ) )
				{
					DWORD size = data.Len();
					API(ADVAPI32, CryptEncrypt)( hkey, 0, TRUE, 0, (BYTE*)data.Ptr(), &size, resSize );
					data.SetLen(resSize);
					ret = true;
				}
			}
			API(ADVAPI32, CryptDestroyKey)(hkey);
		}
		API(ADVAPI32, CryptReleaseContext)(hprov, 0 );
	}
	return ret;
}

bool DecryptAES256( const Mem::Data& key, Mem::Data& data, int offset )
{
	HCRYPTPROV hprov;
	bool ret = false;
	if( API(ADVAPI32, CryptAcquireContextA)( &hprov, 0, 0, PROV_RSA_AES, CRYPT_VERIFYCONTEXT ) )
	{
		HCRYPTKEY hkey;
		KeyBlobStru blob;
		blob.header.aiKeyAlg = CALG_AES_256;
		blob.header.bType = PLAINTEXTKEYBLOB;
		blob.header.bVersion = CUR_BLOB_VERSION;
		blob.size = key.Len();
		Mem::Copy( blob.data, key.Ptr(), key.Len() );
		if( API(ADVAPI32, CryptImportKey)( hprov, (BYTE*)&blob, sizeof(blob), 0, 0, &hkey ) )
		{
			DWORD resSize = data.Len() - offset;
			if( API(ADVAPI32, CryptDecrypt)( hkey, 0, TRUE, 0, (BYTE*)data.Ptr() + offset, &resSize ) )
			{
				data.SetLen(resSize + offset);
				ret = true;
			}
			API(ADVAPI32, CryptDestroyKey)(hkey);
		}
		API(ADVAPI32, CryptReleaseContext)(hprov, 0 );
	}
	return ret;
}

bool GenAES256Key( Mem::Data& key )
{
	bool ret = false;
	HCRYPTPROV hprov;
	if( API(ADVAPI32, CryptAcquireContextA)( &hprov, 0, 0, PROV_RSA_AES, CRYPT_VERIFYCONTEXT ) )
	{
		HCRYPTKEY hkey;
		if( API(ADVAPI32, CryptGenKey)( hprov, CALG_AES_256, CRYPT_EXPORTABLE, &hkey ) )
		{
			DWORD size;
			if( API(ADVAPI32, CryptExportKey)( hkey, 0, PLAINTEXTKEYBLOB, 0, 0, &size ) )
			{
				KeyBlobStru blob;
				if( API(ADVAPI32, CryptExportKey)( hkey, 0, PLAINTEXTKEYBLOB, 0, (BYTE*)&blob, &size ) )
				{
					key.SetLen(0);
					if( key.Append( blob.data, blob.size ) )
					{
						ret = true;
					}
				}
			}
			API(ADVAPI32, CryptDestroyKey)(hkey);
		}
		API(ADVAPI32, CryptReleaseContext)(hprov, 0 );
	}
	return ret;
}

}
