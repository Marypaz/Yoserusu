#ifndef INCLUDE_MASHIRO_COLLISIONDETECTOR_H_
#define INCLUDE_MASHIRO_COLLISIONDETECTOR_H_

namespace Mashiro{
namespace Math {
	class Vector3;
};

namespace Scene {
using namespace Math;

class PMDFileLoader;

class CollisionMesh {
public:
	//PMD�t�@�C������R���X�g���N�g
	static CollisionMesh create( const PMDFileLoader&, const Vector3& vec );
	///���[�h�I�����m�B�t�@�C�������������ꍇ��true���Ԃ邱�Ƃ��m�F���Ă���g�����ƁB
	bool isReady();
	///�����ƌ�������B
	bool isIntersect( const Vector3& lineOrigin, const Vector3& lineVector ) const;
	///�����ƌ�������B�ŏ��ɓ�������������timeOut�ɓ����B�������true��Ԃ��B
	bool getIntersection( 
		float* timeOut, //0�|�C���^�Ȃ�Ԃ��Ȃ�
		Vector3* normalOut, //0�|�C���^�Ȃ�Ԃ��Ȃ�
		const Vector3& lineOrigin, 
		const Vector3& lineVector ) const;

	CollisionMesh();
	~CollisionMesh();
	CollisionMesh( const CollisionMesh& );
	operator void*() const;
	CollisionMesh& operator=( const CollisionMesh& );
	void release();
	bool operator==( const CollisionMesh& ) const;
	bool operator!=( const CollisionMesh& ) const;
private:
	class Impl;
	explicit CollisionMesh( Impl* );
	Impl* mImpl;

};

}
}

#endif