#ifndef INCLUDE_MASHIRO_AUTOPTR_H_
#define INCLUDE_MASHIRO_AUTOPTR_H_

namespace Mashiro {

template< class T > class AutoPtr{
public:
	AutoPtr( T* p = 0 );
	~AutoPtr();
	//���:���g������Α��폜(����𗘗p���āA=0�ōD���ȃ^�C�~���O�ŏ�����)
	AutoPtr& operator=( T* p );
	T& operator*();
	const T& operator*() const;
	T* operator->();
	const T* operator->() const;
	operator void*() const; //����̂�������if ( p ){ �Ə�����B
private:
	void operator=( AutoPtr< T >& ); //����͋֎~�B�Ǘ��̈ڏ��͂킩��ɂ����B
	AutoPtr( AutoPtr< T >& ); //�R�s�[�R���X�g���N�^���֎~�B�Ǘ��̈ڏ��͂킩��ɂ����B

	T* mPointer;
};

}

#include "Mashiro/Base/Impl/AutoPtrImpl.h"

#endif
