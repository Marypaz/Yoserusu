#ifndef MASHIRO_GRAPHICS_MRS_FILE_LOADER_H_
#define MASHIRO_GRAPHICS_MRS_FILE_LOADER_H_

namespace Mashiro{

namespace Scene {

class Model;
class AnimationModel;

class MRSFileLoader{
public:
	//����
	static MRSFileLoader create();
	//X�t�@�C�����烂�f�������܂�
	AnimationModel createModel( const char* fileName );
	//���f��
	Model createStaticModel( const char* filename );

	//���[�h�I���`�F�b�N
	bool isReady() const;

	//�ȉ����[�U�͋C�ɂ����
	MRSFileLoader();
	~MRSFileLoader();
	MRSFileLoader( const MRSFileLoader& );
	operator void*() const;
	MRSFileLoader& operator=( const MRSFileLoader& );
	void release();
	bool operator==( const MRSFileLoader& ) const;
	bool operator!=( const MRSFileLoader& ) const;
	class Impl;
	explicit MRSFileLoader( const Impl* );
private:
	friend class AnimationModel;
	Impl* mImpl;
};

}
}

#endif
