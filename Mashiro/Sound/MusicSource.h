#ifndef INCLUDE_MUSIC_SOURCE_H_
#define INCLUDE_MUSIC_SOURCE_H_

namespace Mashiro {
namespace Sound {

class MusicSource {
public:
	//�����t�@�C������
	static MusicSource create( const char* fileName );
	//�o�b�t�@�[
	unsigned char* getBuffer();
	//�o�b�t�@�[�T�C�Y
	unsigned int getBufferSize();
	//�G�t�F�N�gOn
	void onEffect();
	//�G�t�F�N�goff
	void offEffect();

	//�ȉ����[�U�͈ӎ����Ȃ��֐��Q
	MusicSource();
	MusicSource( const MusicSource& );
	~MusicSource();
	operator void*() const;
	MusicSource& operator=( const MusicSource& );
	void release();
	bool operator==( const MusicSource& ) const;
	bool operator!=( const MusicSource& ) const;
	class Impl;
private:
	friend class Player;
	friend class AudioEffect;
	Impl* mImpl;
};

}
}

#endif
