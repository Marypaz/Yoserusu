#ifndef MASHIRO_GRAPHICS_SIN_FILE_LOADER_H_
#define MASHIRO_GRAPHICS_SIN_FILE_LOADER_H_

namespace Mashiro{

namespace Scene {

class AnimationModel;

class SINFileLoader{
public:
	//����
	static SINFileLoader create( const char* fileName );

	//���[�h�I���`�F�b�N
	bool isReady() const;

	//�ȉ����[�U�͋C�ɂ����
	SINFileLoader();
	~SINFileLoader();
	SINFileLoader( const SINFileLoader& );
	operator void*() const;
	SINFileLoader& operator=( const SINFileLoader& );
	void release();
	bool operator==( const SINFileLoader& ) const;
	bool operator!=( const SINFileLoader& ) const;
	class Impl;
	explicit SINFileLoader( const Impl* );
private:
	friend class AnimationModel;
	Impl* mImpl;
};

}
}

#endif
