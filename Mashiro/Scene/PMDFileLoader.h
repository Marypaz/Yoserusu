#ifndef MASHIRO_GRAPHICS_PMD_FILE_LOADER_H_
#define MASHIRO_GRAPHICS_PMD_FILE_LOADER_H_

namespace Mashiro{

namespace Scene {

class Model;

class PMDFileLoader{
public:
	//����
	static PMDFileLoader create();
	//X�t�@�C�����烂�f�������܂�
	Model createModel( const char* fileName );

	//���[�h�I���`�F�b�N
	bool isReady() const;

	//�ȉ����[�U�͋C�ɂ����
	PMDFileLoader();
	~PMDFileLoader();
	PMDFileLoader( const PMDFileLoader& );
	operator void*() const;
	PMDFileLoader& operator=( const PMDFileLoader& );
	void release();
	bool operator==( const PMDFileLoader& ) const;
	bool operator!=( const PMDFileLoader& ) const;
	class Impl;
	explicit PMDFileLoader( const Impl* );
private:
	friend class CollisionMesh;
	Impl* mImpl;
};

}
}

#endif
