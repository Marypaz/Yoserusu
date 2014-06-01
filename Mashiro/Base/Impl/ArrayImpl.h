#ifndef INCLIUDE_MASHIRO_BASE_ARRAY_IMPL_H_
#define INCLIUDE_MASHIRO_BASE_ARRAY_IMPL_H_

#include "Mashiro/Mashiro.h"
#include "Mashiro/Base/Array.h"

namespace Mashiro{

template< class T > inline Array< T >::Array() :
mElements( 0 ),
mSize( 0 ){
}

template< class T > inline Array< T >::Array( int size ) :
mElements( 0 ),
mSize( size ){
	if ( size > 0 ){
		mElements = static_cast< T* >( OPERATOR_NEW( sizeof( T ) * mSize ) );
		for ( int i = 0; i < mSize; ++i ){
			NEW ( &mElements[ i ] ) T;
		}
	}
}

template< class T > inline Array< T >::Array( int size, const T& a ) :
mElements( 0 ),
mSize( size ){
	if ( size > 0 ){
		mElements = static_cast< T* >( OPERATOR_NEW( sizeof( T ) * mSize ) );
		for ( int i = 0; i < mSize; ++i ){
			NEW ( &mElements[ i ] ) T( a );
		}
	}
}

template< class T > inline Array< T >::~Array(){
	clear();
}

template< class T > inline void Array< T >::setSize( int size ){
	//空か、縮小方向なら受け付ける
	if ( !mElements ){ //空
		mElements = static_cast< T* >( OPERATOR_NEW( sizeof( T ) * size ) );
		for ( int i = 0; i < size; ++i ){
			NEW ( &mElements[ i ] ) T;
		}
	}else{
		STRONG_ASSERT( ( size <= mSize ) && "Array::setSize() : extension is not supperted." );
		//縮小分デストラクト
		for ( int i = size; i < mSize; ++i ){
			mElements[ i ].~T();
		}
	}
	mSize = size;
}

template< class T > inline int Array< T >::size() const{
	return mSize;
}

template< class T > inline void Array< T >::clear(){
	for ( int i = 0; i < mSize; ++i ){
		mElements[ i ].~T();
	}
	operator delete( mElements );
	mSize = 0;
}

template< class T > inline const T& Array< T >::operator[]( int i ) const {
	STRONG_ASSERT( i >= 0 && i < mSize && "range error." );
	return mElements[ i ];
}

template< class T > inline T& Array< T >::operator[]( int i ){
	STRONG_ASSERT( i >= 0 && i < mSize && "range error." );
	return mElements[ i ];
}

}

#endif
