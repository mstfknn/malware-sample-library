#pragma once

class VectorBase
{
	protected:
	
		void* data; //указатель на данные
		int count; //количество элементов
		int size; //на сколько элементов выделено памяти
		int szValue; //размер одного элемента

		VectorBase( int _size, int _szValue );
		~VectorBase();

		//на сколько элементов выделить памяти
		bool Realloc( int sz );
		//увеличить на 50% количество памяти для элементов
		bool Grow();
		//вызывается при удаления вектора для каждого элемента
		virtual void ReleaseValue( void* val );
		void* Get(int index ) const;

		VectorBase( VectorBase&& );

	public:

		int Count() const
		{
			return count;
		}
		int Size() const
		{
			return size;
		}
		//удаляет элемент по индексу
		void Del( int i );
		//удаляет все элементы
		void DelAll();
};

template< class T >
class Vector : public VectorBase
{
		
		virtual void ReleaseValue( void* val );

		Vector( const Vector& );
		Vector& operator=(const Vector&);

	public:

		Vector( int size = 32 );
		Vector( Vector&& v ) : VectorBase(v)
		{
		}
		T* Add( const T& val );
		T* Add();
		T& Get( int index ) const;
		T& operator[]( int index )
		{
			return Get(index);
		}
		const T& operator[]( int index ) const
		{
			return Get(index);
		}

};

template< class T >
Vector<T>::Vector( int size ) : VectorBase( size, sizeof(T) )
{
}

template< class T >
void Vector<T>::ReleaseValue( void* val )
{
	((T*)val)->~T();
}

template< class T >
T* Vector<T>::Add( const T& val )
{
	if( count >= size )
		if( !Grow() )
			return 0;
	T& ret = Get(count);
	ret = val;
	count++;
	return &ret;
}

template< class T >
T* Vector<T>::Add()
{
	if( count >= size )
		if( !Grow() )
			return 0;
	T& ret = Get(count);
	ret = T();
	count++;
	return &ret;
}

template< class T >
T& Vector<T>::Get( int index ) const
{
	return *((T*)VectorBase::Get(index));
}
