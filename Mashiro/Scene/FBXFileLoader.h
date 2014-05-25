#ifndef MASHIRO_GRAPHICS_FBX_FILE_LOADER_H_
#define MASHIRO_GRAPHICS_FBX_FILE_LOADER_H_

namespace Mashiro{

namespace Graphics{
	class Texture;
}

namespace Scene {

class Model;

class FBXFileLoader{
public:
	//����
	static FBXFileLoader create();
	//FBX�t�@�C�����烂�f�������܂�
	Model createModel( const char* fileName );

	//���[�h�I���`�F�b�N
	bool isReady() const;

	//�ȉ����[�U�͋C�ɂ����
	FBXFileLoader();
	~FBXFileLoader();
	FBXFileLoader( const FBXFileLoader& );
	operator void*() const;
	FBXFileLoader& operator=( const FBXFileLoader& );
	void release();
	bool operator==( const FBXFileLoader& ) const;
	bool operator!=( const FBXFileLoader& ) const;
	class Impl;
	explicit FBXFileLoader( const Impl* );
private:
	Impl* mImpl;
};

}
}

#endif
