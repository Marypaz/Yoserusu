#ifndef INCLUDE_SOUND_IMPL_H_
#define INCLUDE_SOUND_IMPL_H_

#include "GameLib/GameLib.h"
#include "GameLib/FileIO/Manager.h"
#include "GameLib/FileIO/InFile.h"
#include "GameLib/Base/Impl/ReferenceType.h"
#include "GameLib/Base/RefString.h"
#include <XAudio2.h>

#include "Sound/�w�b�_�[ �t�@�C��/Wave.h"
#include "Sound/�w�b�_�[ �t�@�C��/Mixer.h"
#include "Sound/�w�b�_�[ �t�@�C��/Player.h"
#include "DecoderBase.h"
#include "SoundDataCreate.h"
#include "SoundBase.h"

namespace Marisa {
namespace Sound{

//�\�[�X�{�C�X�����������ăv���C���[�ɓn���Ă����瑤�ōĐ�����
class Wave::Impl : public GameLib::ReferenceType {
public:
	Impl(){
	}
	~Impl(){
	}

};

};
};

#endif
