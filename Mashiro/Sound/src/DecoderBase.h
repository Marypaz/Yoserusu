#ifndef INCLUDE_SOUND_DECODER_BASE_H_
#define INCLUDE_SOUND_DECODER_BASE_H_

#include <XAudio2.h>

namespace Mashiro {
namespace Sound {

class DecoderBase {
public:
	virtual ~DecoderBase(){ }

	//�I�[�v��
	virtual bool open( const char* fileName, WAVEFORMATEX& format ) = 0;

	//�N���[�Y
	virtual void rlease() = 0;

	//�f�R�[�h
	virtual unsigned int decode( unsigned char* buffer, unsigned int size ) = 0;

	//�Đ��ʒu��ݒ�
	virtual void seek( int offset ) = 0;

	//�I�_�ł����H
	virtual bool checkStreamEnd() = 0;

	//�����ǂ񂭂炢�H
	virtual unsigned int getLength() = 0;
};

}
}

#endif