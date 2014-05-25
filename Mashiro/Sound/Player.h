#ifndef INCLUDE_SOUND_MANAGER_H_
#define INCLUDE_SOUND_MANAGER_H_

#include "Mashiro/Sound/MusicSource.h"

namespace Mashiro {
namespace Sound{

class Wave;
class Mixer;

class Player {
public:
	//�쐬
	static Player create( MusicSource );
	//�Đ�
	void start();
	//�Đ����[�v
	void start( unsigned int roop, unsigned int lenght );
	//�X�g�b�v
	void stop();
	//�|�[�Y
	void pause();
	//����
	void setVolum( float v );
	//�s�b�`�ύX
	void setPitch( float s );

	//�ȉ����[�U�͈ӎ����Ȃ��֐��Q
	Player();
	Player( const Player& );
	~Player();
	operator void*() const;
	Player& operator=( const Player& );
	void release();
	bool operator==( const Player& ) const;
	bool operator!=( const Player& ) const;
private:
	//���g�͂��̂Ȃ�
	class Impl;
	Impl* mImpl;
};

};
};

#endif