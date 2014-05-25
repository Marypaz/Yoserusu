#ifndef INCLUDE_MASHIRO_GRAPHICS_TEXTURE_H_
#define INCLUDE_MASHIRO_GRAPHICS_TEXTURE_H_

namespace Mashiro{

namespace Graphics {

class Texture {
public:
	//���O���琶��
	static Texture create( const char* fileName );
	//��̃e�N�X�`���[�����I
	static Texture create( int w, int h );
	//���O�Q�b�g
	const char* name() const;

	//���擾
	int width() const;
	int height() const;
	//���O�Ńe�N�X�`���[�����܂�
	unsigned char* lockChar();
	///�������݊֐�
	void lock( unsigned** address, int* pitch, int mipLevel = 0 );
	///�����������݊֐�
	void lock( 
		unsigned** address, 
		int* pitch, 
		int x,
		int y,
		int width,
		int height,
		int mipLevel = 0 );
	void unlock();

	//�ȉ����[�U�͈ӎ����Ȃ��֐��Q
	Texture();
	Texture( const Texture& );
	~Texture();
	operator void*() const;
	Texture& operator=( const Texture& );
	void release();
	bool operator==( const Texture& ) const;
	bool operator!=( const Texture& ) const;
	bool operator<( const Texture& ) const;

	class Impl;
	Texture( const Impl* );
private:
	friend class Manager;
	Impl* mImpl;
};

}
}


#endif
