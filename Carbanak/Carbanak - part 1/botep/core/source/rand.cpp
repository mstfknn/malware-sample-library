#include "core\rand.h"

namespace Rand
{

const int A = 17, B = 5;
//массив случайных чисел, найдя в exe его по заданным значениям нужно заменить эти числа на свои случайные 
uint randVals[A] = { 1000001, 1000002, 1000003, 1000004, 1000005, 1000006, 1000007, 1000008, 1000009, 1000010, 1000011, 1000012, 1000013, 1000014, 1000015, 1000016, 1000017 };
uint N = 0; //номер случайного числа

void Init()
{
	N = 0;
	uint x = API(KERNEL32, GetTickCount)();
	//налаживаем случайное число на массив исходных
	for( int i = 0; i < A; i++ )
		randVals[i] ^= x;
}

//генерируем следующее случайное число
static uint Next()
{
	uint n1 = (N - A) % A;
	uint n2 = (N - B) % A;
	uint v = randVals[n1] + randVals[n2];
	uint n = N % A;
	randVals[n] = v;
	N++;
	return v;
}

uint Gen( int min, int max )
{
	uint v = Next();
	if( min > max )
	{
		int t = min;
		min = max;
		max = t;
	}
	return (v % (max - min + 1)) + min;
}

StringBuilder& Gen( StringBuilder& s, int min, int max )
{
	s.SetEmpty();
	char buf[16];
	int n = 0;
	int len = Gen( min, max );
	while( n < len )
	{
		int sz = sizeof(buf) - 1;
		if( sz > len - n ) sz = len - n;
		for( int i = 0; i < sz; i++ )
		{
			int c = Gen( 2 * ('z' - 'a' + 1) - 1 );
			if( c <= 'z' - 'a' )
				c += 'a';
			else
				c = c - ('z' - 'a' + 1) + 'A';
			buf[i] = c;
		}
		buf[sz] = 0;
		s.Cat( buf, sz );
		n += sz;
	}
	return s;
}

void Gen( byte* buf, int size )
{
	for( int i = 0; i < size; i++ )
		buf[i] = Gen(255);
}

bool Condition( int v )
{
	return (int)Gen( 0, 100 ) < v;
}

}
