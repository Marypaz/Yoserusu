#include "SoundManager.h"
#include "Mashiro/Mashiro.h"
#include <sstream>
using namespace Mashiro;
using namespace Mashiro::Sound;


//�Ԃɍ���Ȃ��̂ł����ɂ����܂��@
namespace {
	const char* gBGM[] = {
		"res/music/Title.wav",
		"res/music/GameMain2.wav",
		"res/music/Ending2.wav",
		"res/music/Authe.wav",
	};
	const char* gSe[] = {
		"res/music/ok.wav",
		"res/music/open.wav",
		"res/music/false.wav",
		"res/music/countDown.wav",
		"res/music/start.wav",
		"res/music/warring.wav",
		"res/music/end.wav",
	};
}

SoundManager* SoundManager::mInstance = 0;

SoundManager* SoundManager::instance(){
	return mInstance;
}

void SoundManager::create(){
	ASSERT( !mInstance );
	mInstance = new SoundManager();
}

void SoundManager::destory(){
	ASSERT( mInstance );
	SAFE_DELETE( mInstance );
}

SoundManager::SoundManager() :
mSePlayerPos( 0 ){
	//�t�@�C�����[�h�J�n�B
	mBgmWaves = new Mashiro::Sound::MusicSource[ BGM_MAX ];
	mSeWaves = new Mashiro::Sound::MusicSource[ SE_MAX ];
	for( int i = 0; i < BGM_MAX; ++i ){
		mBgmWaves[ i ] = Mashiro::Sound::MusicSource::create( gBGM[ i ] );
	}
	for( int i = 0; i < SE_MAX; ++i ){
		mSeWaves[ i ] = Mashiro::Sound::MusicSource::create( gSe[ i ] );
	}
}

SoundManager::~SoundManager(){
	if( mBgmPlayer ){
		mBgmPlayer.stop();
	}
	SAFE_DELETE_ARRAY( mBgmWaves );
	SAFE_DELETE_ARRAY( mSeWaves );
}

void SoundManager::playBgm( Bgm bgm ){
	mBgmPlayer = Sound::Player::create( mBgmWaves[ bgm ] );
	mBgmPlayer.setVolum( 0.f );
	mBgmPlayer.start( 0, 0 ); //���[�v�Đ�
}

void SoundManager::stopBgm(){
	if ( mBgmPlayer ){
		mBgmPlayer.stop();
	}
}

void SoundManager::playSe( Se se ){
	mSePlayers[ mSePlayerPos ] = Sound::Player::create( mSeWaves[ se ] );
	mSePlayers[ mSePlayerPos ].start(); //�Đ�
	++mSePlayerPos;
	//�����߂�
	if ( mSePlayerPos == SE_PLAYER_MAX ){
		mSePlayerPos = 0;
	}
}

void SoundManager::setPitch( float p ){
	mBgmPlayer.setPitch( p );
}