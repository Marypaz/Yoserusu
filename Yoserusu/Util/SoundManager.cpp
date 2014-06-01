#include "SoundManager.h"
#include "Mashiro/Mashiro.h"
#include <sstream>
using namespace Mashiro;
using namespace Mashiro::Sound;


//間に合わないのでここにおきます　
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
	mInstance = NEW SoundManager();
}

void SoundManager::destory(){
	ASSERT( mInstance );
	SAFE_DELETE( mInstance );
}

SoundManager::SoundManager() :
mSePlayerPos( 0 ){
	//ファイルロード開始。
	mBgmWaves = NEW Mashiro::Sound::MusicSource[ BGM_MAX ];
	mSeWaves = NEW Mashiro::Sound::MusicSource[ SE_MAX ];
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
#if _DEBUG
	mBgmPlayer.setVolum( 0.f );
#else
	mBgmPlayer.setVolum( 1.f );
#endif
	mBgmPlayer.start( 0, 0 ); //ループ再生
}

void SoundManager::stopBgm(){
	if ( mBgmPlayer ){
		mBgmPlayer.stop();
	}
}

void SoundManager::playSe( Se se ){
	mSePlayers[ mSePlayerPos ] = Sound::Player::create( mSeWaves[ se ] );
#if _DEBUG
	mSePlayers[ mSePlayerPos ].setVolum( 0.f );
#else
	mSePlayers[ mSePlayerPos ].setVolum( 1.f );
#endif
	mSePlayers[ mSePlayerPos ].start(); //再生
	++mSePlayerPos;
	//巻き戻し
	if ( mSePlayerPos == SE_PLAYER_MAX ){
		mSePlayerPos = 0;
	}
}

void SoundManager::setPitch( float p ){
	mBgmPlayer.setPitch( p );
}