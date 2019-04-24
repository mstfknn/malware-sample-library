#pragma once

#include "winapi.h"

//вычисление хеша
uint CalcHash( const byte* ptr, int c_ptr );

//умный указатель, автоматом удаляет указатель
template< class T >
class AutoPtr
{
		mutable T* ptr;

	public:

		AutoPtr( const T& val );
		AutoPtr( T* val ) : ptr(val)
		{
		}
		AutoPtr( const AutoPtr& p )
		{
			ptr = p.ptr;
		}
		~AutoPtr();
		operator T&()
		{
			return *ptr;
		}
		operator T*()
		{
			return ptr;
		}
		T* operator->()
		{
			return ptr;
		}
};

//тоже самое, что и AutoPtr, только присванивании или копировании объекта, указатель который копируется очищается (присваивается 0)
//этот специальный класс и используется в контейнерах для работы со сложными объектами
template< class T >
class MovedPtr
{
		mutable T* ptr;

	public:

		MovedPtr( const T& val );
		MovedPtr( T* val )
		{
			ptr = val;
		}
		MovedPtr( const MovedPtr& p )
		{
			ptr = p.ptr;
			p.ptr = 0;
		}
		~MovedPtr();
		operator T&()
		{
			return *ptr;
		}
		operator const T&() const
		{
			return *ptr;
		}
		operator T*()
		{
			return ptr;
		}
		T* operator->()
		{
			return ptr;
		}
		const T* operator->() const
		{
			return ptr;
		}
		T& operator=( const MovedPtr& p )
		{
			ptr = p.ptr;
			p.ptr = 0;
			return *this;
		}
		bool IsValid() const
		{
			return ptr != nullptr;
		}
};


template< class T >
AutoPtr<T>::AutoPtr( const T& val )
{
	ptr = new T();
	*ptr = val;
}

template< class T >
AutoPtr<T>::~AutoPtr()
{
	if( ptr ) delete ptr;
}

template< class T >
MovedPtr<T>::MovedPtr( const T& val )
{
	ptr = new T();
	*ptr = val;
}

template< class T >
MovedPtr<T>::~MovedPtr()
{
	delete ptr;
}

//Возвращает мак адрес. Адрес сохраняется в mac (место должно быть достаточно, не меньше 8-ми байт)
//Функция коичество байт записанных в mac, если 0, то ошибка
int GetMacAddress( byte* mac );
//запуск потока
bool RunThread( typeFuncThread func, void* data );
//запуск потока и возврат его хендла
HANDLE RunThread( typeFuncThread func, void* data, DWORD* threadId );
//задержка в миллисекундах (обертка для Sleep)
void Delay( uint v );

//работа с критическими секциями
class CriticalSection
{
		CRITICAL_SECTION& cs;
	
	public:

		static void Init( CRITICAL_SECTION& cs );
		static void Enter( CRITICAL_SECTION& cs );
		static void Leave( CRITICAL_SECTION& cs );

		CriticalSection( CRITICAL_SECTION& _cs ) : cs(_cs)
		{
			Enter(cs);
		}
		~CriticalSection()
		{
			Leave(cs);
		}
};

//из строки ip:port выделяет ip и порт
bool ExtractIpPort( const char* s, char* ip, int& port );
