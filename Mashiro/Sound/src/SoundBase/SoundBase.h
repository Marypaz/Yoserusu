#ifndef INCLUDE_SOUND_BASE_H_
#define INCLUDE_SOUND_BASE_H_

#include <XAudio2.h>

namespace Mashiro {
namespace Sound {

class SoundBase{
public:
	SoundBase();
	static SoundBase instance();

	//�f�o�C�X�Q�b�g
	IXAudio2*& getDevice();
	//�}�X�^�[�{�C�X�Q�b�g
	IXAudio2MasteringVoice*& getMasterVoice();

	//��������
	static void create();
	static void destroy();
};

}
}

#endif
