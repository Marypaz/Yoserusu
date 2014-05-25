#ifndef INCLUDE_MASHIRO_OGG_H_
#define INCLUDE_MASHIRO_OGG_H_

#include <XAudio2.h>
#include <stdio.h>
#include "vorbis/vorbisfile.h"
#include "Mashiro/Sound/src/DecoderBase.h"

namespace Mashiro {
namespace Sound{

class Ogg : public DecoderBase{
public:
	Ogg();
	~Ogg();

	//�t�@�C���I�[�v��
	bool open( const char* path, WAVEFORMATEX& format );
	//�t�@�C���N���[�Y
	void close();
	//���
	void rlease();
	//�f�R�[�h
	unsigned int decode( unsigned char* buffer, unsigned int size );
	//�Đ��ʒu��ݒ�
	void seek( int offset );
	//�T�E���h�t�@�C���̏I�[�`�F�b�N
	bool checkStreamEnd();
	//�������擾
	unsigned int getLength();


	//�t�H�[�}�b�g��ǂݎ��
	bool readFormat( FILE* fp, WAVEFORMATEX& format );
private:
	OggVorbis_File mFp;
	bool mOpen;
};

};
};

#endif