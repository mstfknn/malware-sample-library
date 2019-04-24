#include "core\memory.h"
#include "core\vector.h"

VectorBase::VectorBase( int _size, int _szValue )
{
	count = 0;
	size = _size;
	szValue = _szValue;
	int szAlloc = _size * _szValue;
	data = Mem::Alloc(szAlloc);
	if( data )
	{
		Mem::Set( data, 0, szAlloc );
	}
	else
		size = 0;
}

VectorBase::VectorBase( VectorBase&& v )
{
	data = v.data;
	count = v.count;
	size = v.size;
	szValue = v.szValue;
	v.data = 0;
	v.count = 0;
	v.size = 0;
}

VectorBase::~VectorBase()
{
	if( data )
	{
		for( int i = 0; i < count; i++ )
			ReleaseValue( (byte*)data + i * szValue );
		Mem::Free(data);
	}
}

void VectorBase::ReleaseValue( void* val )
{
}

bool VectorBase::Realloc( int sz )
{
	if( sz <= count ) return false;
	int szAlloc = sz * szValue;
	void* newData = Mem::Realloc( data, szAlloc );
	if( newData )
	{
		data = newData;
		size = sz;
		Mem::Set( (byte*)data + size * szValue, 0, szAlloc - size * szValue );
		return true;
	}
	return false;
}

bool VectorBase::Grow()
{
	return Realloc( size + size / 2 );
}

void* VectorBase::Get( int index ) const
{
	return (byte*)data + index * szValue;
}

void VectorBase::Del( int i )
{
	if( i < 0 || i >= count ) return;
	byte* to = (byte*)data + i * szValue;
	ReleaseValue(to);
	byte* from = to + szValue;
	int moved = (count - i - 1) * szValue;
	Mem::Copy( to, from, moved );
	Mem::Set( to + moved, 0, szValue );
	count--;
}

void VectorBase::DelAll()
{
	for( int i = 0; i < count; i++ )
	{
		void* val = (byte*)data + i * szValue;
		ReleaseValue(val);
	}
	count = 0;
}
