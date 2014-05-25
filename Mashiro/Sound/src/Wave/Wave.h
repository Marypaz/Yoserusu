#ifndef INCLUDE_WAVE_H_
#define INCLUDE_WAVE_H_

#include <XAudio2.h>
#include <stdio.h>

#include "Mashiro/Sound/src/DecoderBase.h"

namespace Mashiro {
namespace Sound{

class Wave : public DecoderBase{
public:
	Wave();
	~Wave();

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
	bool readFormat( WAVEFORMATEX& format );
private:
	class Impl;
	Impl* mImpl;
	FILE* mFp;
	size_t mSoundSize;
	size_t mHeaderSize;
	size_t mBlockSize;
	size_t mLength;
};

};
};

#endif