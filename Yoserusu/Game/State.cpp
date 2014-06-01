#include "Mashiro/Mashiro.h"
#include "Mashiro/Kinect/KinectManager.h"
#include "Mashiro/Graphics/GraphicsManager.h"
#include "Mashiro/WorkSpace/WorkSpace.h"
#include "Mashiro/Input/InputManager.h"
#include "Mashiro/Input/Keyboard.h"

#include "Game/State.h"
#include "Game/Ball.h"
#include "Game/BackGround.h"
#include "Game/Wall.h"
#include "Game/LockOn.h"

#include "Util/ModelLoader.h"
#include "Util/DepthSingleton.h"
#include "Util/DataBase.h"

#include <sstream>
using namespace Mashiro;
using namespace Mashiro::Graphics;
using namespace Mashiro::Kinect;
using namespace std;

State::State() : 
ball( 0 ), 
wall( 0 ), 
lArm( 0 ), 
rArm( 0 ), 
timer( 0 ), 
mBack( 0 ), 
mLockOn( 0 ){
	//オブジェクトの生成
	ball = NEW Ball();
	wall = NEW Wall();
	lArm= NEW ArmLeft();
	rArm = NEW ArmRight();
	timer = NEW Timer();
	mBack = NEW BackGround();
	mLockOn = NEW LockOn();
}

void State::init(){
	ball->Initi();
	lArm->Init();
	rArm->Init();
}

State::~State(){
	//deleteは生成した時の逆順でしよう
	SAFE_DELETE( mBack );
	SAFE_DELETE( mLockOn );
	SAFE_DELETE(timer);
	SAFE_DELETE(rArm);
	SAFE_DELETE(lArm);
	SAFE_DELETE(wall);
	SAFE_DELETE(ball);
}

void State::update(){

	//タイマー更新
	timer->update();
	
	//クラス更新
	lArm->Update(ball->Depth(),Mashiro::Kinect::SKELETON_INDEX_HAND_LEFT);
	rArm->Update(ball->Depth(),Mashiro::Kinect::SKELETON_INDEX_HAND_RIGHT);

	//タイマー回してる
	if( timer->isStart() && !timeUp() ){
		//ボールアップデート
		ball->Update( rArm, lArm );
		//ターゲット
		mLockOn->update( targetCheck() );
	}

	if( !timeUp() ){
		wall->Animation(ball);
	}
}

void State::draw(){
	//背景
	mBack->draw();
	//ボール描画
	ball->Draw( timer->draw_10(), timer->draw_01());
	//腕描画
	rArm->Draw();
	lArm->Draw();
	wall->Draw( timer->IsEnd() );
    mLockOn->draw();
	timer->startDraw();
	if( timer->time() <= 10 ){
		if( timer->time() > 0 ){
			timer->warrningDraw();
		}
	}
}

bool State::timeUp() const{
	//タイマーが切れればシーン移行へ
	if(timer->IsEnd()){
		return true;
	}
	return false;
}

bool State::countDown() const{
	if(!timer->isStart()){
		return false;
	}
	return true;
}

bool State::targetCheck() const {
	//ターゲット
	if(ball->IsMode() == Keep){
		return true;
	}
	return false;
}