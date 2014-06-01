#include "Sequence/AutheScene.h"
#include "Sequence/Parent.h"

#include "Mashiro/Mashiro.h"
#include "Mashiro/Math/Matrix.h"
#include "Mashiro/Math/Vector3.h"
#include "Mashiro/Math/Vector4.h"
#include "Mashiro/Graphics/GraphicsManager.h"
#include "Mashiro/WorkSpace/WorkSpace.h"
#include "Mashiro/Kinect/KinectManager.h"
#include "Mashiro/Input/InputManager.h"
#include "Mashiro/Input/Keyboard.h"
#include "Mashiro/Input/Mouse.h"

#include "Game/BackGround.h"
#include "Game/Authe.h"
#include "Util/SoundManager.h"
#include "Util/DepthSingleton.h"

namespace Sequence{

AutheScene::AutheScene() : 
mAuthe( 0 ),
mMoveSceneCount( 0 ){
	SoundManager::instance()->playBgm(SoundManager::BGM_AUTHE);
	Graphics::Manager m = Graphics::Manager::instance();

	Matrix world,view,proj;
	int width, height;
	m.getViewPort(0,0,&width, &height);
	world.setTranslation(Vector3(0));
	proj.setPerspectiveFovLH(45.f, (float)WorkSpace::instance().width(),(float)WorkSpace::instance().height(),1.0f,10000.f);

	view.setIdentity();
	view.setLookAtLH( Vector3(0,0,-30),Vector3(0.f),Vector3(0,1,0));
	
	Matrix mat;
	mat.setIdentity();
	mat.setMul( mat, view );
	mat.setMul( mat, proj );
	Matrix inv;
	inv.setInverse( mat );
	Vector4 v( -0.57f, -0.57f, -0.57f, 1.f );
	v = mat.translation( v, inv );

	m.setLight( Vector4( 0.7f,0.7f, 0.6f, 2.f ) );
	m.setEyePos( v );
	//m.setShader( Graphics::SHADER_COCTRANS );

	m.setViewMatrix(view);
	m.setWorldMatrix(world);
	m.setProjectionMatrix( proj);

	mAuthe = NEW Authe();
	mBack = NEW BackGround();
}

AutheScene::~AutheScene(){
	SoundManager::instance()->stopBgm();
	SAFE_DELETE( mAuthe );
	SAFE_DELETE( mBack );
}

void AutheScene::update( Parent* parent ){

#if _DEBUG
	if( Mashiro::Input::Manager::instance().mouse().isTriggered( Input::Mouse::BUTTON_LEFT ) ){
		parent->moveTo( Parent::NEXT_TITLE );
	}
#endif

	autheUpdate( parent );

	autheDraw();
}

void AutheScene::autheDraw(){
	mBack->draw();
	mAuthe->draw();
}

void AutheScene::autheUpdate( Parent* parent ){
	if( mAuthe->getMode() == MODE_END ){
		if( moveScene() ){
			parent->moveTo( Parent::NEXT_TITLE );
		}
	}
	if( !mAuthe->handFlag() ){
		if( mAuthe->handCheck() ){
			//認証が完了したら深度値取得
			mAuthe->depthCheck();
		}
	}
}

bool AutheScene::moveScene(){
	if( mMoveSceneCount == 90 ){
		//認証が完了した 放置してからシーンをポップする
		SoundManager::instance()->playSe(SoundManager::SE_OK);
	}
	if( mMoveSceneCount == 100 ){
		return true;
	}
	++mMoveSceneCount;
	return false;
}

} //namespace Sequence