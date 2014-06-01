#include "Authe.h"
#include "Mashiro/Mashiro.h"
#include "Mashiro/Kinect/KinectManager.h"
#include "Mashiro/Graphics/GraphicsManager.h"
#include "Mashiro/Graphics/SpriteManager.h"
#include "Mashiro/Math/Vector3.h"
#include "Util/DataBase.h"
#include "Util/DepthSingleton.h"
#include <sstream>

#include "Lua/LuaManager.h"

#include "Mashiro/Input/InputManager.h"
#include "Mashiro/Input/Keyboard.h"
#include "Mashiro/Input/Mouse.h"

using namespace std;

using namespace Mashiro;
using namespace Mashiro::Graphics;
using namespace Mashiro::Math;
using namespace Mashiro::Kinect;

Authe::Authe() : mHandCheckCount( 0 ), mHandCheck( false ), mDepth( 0 ), mCount( 0 ), mSum( 0 ),  mMoveSceneFlag( MODE_WAIT ){
	mMoveSceneFlag = MODE_WAIT;

	mBitmap[ 0 ] = Bitmap::create( "res/image/Auth.png" );
	mBitmap[ 1 ] = Bitmap::create( "res/image/Ok.png" );
	mBitmap[ 2 ] = Bitmap::create( "res/image/RedMaru.png" );
	mBitmap[ 3 ] = Bitmap::create( "res/image/BlueMaru.png" );  
	mBitmap[ 4 ] = Bitmap::create( "res/image/Title_Auth.png");
}

Authe::~Authe() {
}

void Authe::draw()  {
	Graphics::Manager m = Graphics::Manager::instance();
	Mashiro::Graphics::Sprite sp = Mashiro::Graphics::Sprite::instance();
	//認証範囲
	sp.setTrance( 1.f );
	sp.setBitmap( mBitmap[ 2 ] );
	sp.setBitmapRectangle( Vector2( 600, 100 ) );
	sp.draw();

	sp.setTrance( 1.f );
	sp.setBitmap( mBitmap[ 3 ] );
	sp.setBitmapRectangle( Vector2( 200, 100));	
	sp.draw();

	//認証状態
	switch( mMoveSceneFlag ){
		case MODE_WAIT:
			sp.setTrance( 1.f );
			sp.setBitmap( mBitmap[ 4 ] );
			sp.setBitmapRectangle( Vector2(130,400) );
			sp.draw();
			break;
		case MODE_NOW:
			sp.setTrance( 1.f );
			sp.setBitmap( mBitmap[ 4 ] );
			sp.setBitmapRectangle( Vector2(130,400) );
			sp.draw();
			if(check){
				sp.setTrance( 1.f );
				sp.setBitmap( mBitmap[ 0 ] );
				sp.setBitmapRectangle( Vector2( 200 , 530 ));	
				sp.draw();
			}
			break;
		case MODE_END:
			sp.setTrance( 1.f );
			sp.setBitmap( mBitmap[ 1 ] );
			sp.setBitmapRectangle( Vector2( 130 , 530 ));
			sp.draw();
			break;

	}
	Vector2 posR = Mashiro::Kinect::Manager::instance().skeletonPos( Mashiro::Kinect::SKELETON_INDEX_HAND_RIGHT );
	Vector2 posL = Mashiro::Kinect::Manager::instance().skeletonPos( Mashiro::Kinect::SKELETON_INDEX_HAND_LEFT );

	//手
	sp.setColor( Vector3( 1, 0, 0 ) );
	sp.setTrance( 1.f );
	sp.setFillEllipse( posR, Vector2( 30, 30 ) );
	sp.draw();

	sp.setColor( Vector3( 0, 0, 1 ) );
	sp.setTrance( 1.f );
	sp.setFillEllipse( posL, Vector2( 30, 30 ) );
	sp.draw();

}

bool Authe::handCheck()  {
	//枠ない収まってるかチェックしてます
	bool rCheck = HandRang( Mashiro::Kinect::Manager::instance().skeletonPos( Mashiro::Kinect::SKELETON_INDEX_HAND_RIGHT ), Vector2( 600, 100 ) );
	bool lCheck = HandRang( Mashiro::Kinect::Manager::instance().skeletonPos( Mashiro::Kinect::SKELETON_INDEX_HAND_LEFT ),  Vector2( 200, 100 ) );
	
	if ( rCheck && lCheck ){
		mMoveSceneFlag = MODE_NOW;
		mSum += Mashiro::Kinect::Manager::instance().depthSkeleton( Mashiro::Kinect::SKELETON_INDEX_HAND_RIGHT );
		++mCount;
		check = true;
	}else if(!rCheck && !lCheck){
		check = false;
		mCount = 0;
		mDepth = 0;
	}

	if( mCount > 20 ){
		return mHandCheck = true;
	}
	
	return mHandCheck = false;
}

void Authe::depthCheck()  {
	//深度値をうまく取ってこれるまでループさせて
	mDepth = mSum / mCount;
	if(mDepth < 15){
		mHandCheck = false;
		mSum = 0;
		mCount = 0;
		mMoveSceneFlag = MODE_NOW;
		return;
	}
	
	DepthSingleton::instance()->setDepthMin( static_cast< float >( mDepth - 3));
	DepthSingleton::instance()->setDepthMax(
		static_cast< float >(
		Mashiro::Kinect::Manager::instance().depthSkeleton(Mashiro::Kinect::SKELETON_INDEX_HEAD)) );
	
	if(DepthSingleton::instance()->getDepthMin() > DepthSingleton::instance()->getDepthMax() - DataBase::instance()->maxDepth() ){
		mHandCheck = false;
		mSum = 0;
		mCount = 0;
		mMoveSceneFlag = MODE_NOW;
		return;
	}
	if(DepthSingleton::instance()->getDepthMax()-DepthSingleton::instance()->getDepthMin() < DataBase::instance()->minDepth() ){
		mHandCheck = false;
		mSum = 0;
		mCount = 0;
		mMoveSceneFlag = MODE_NOW;
		return;
	}

	int sub = DepthSingleton::instance()->getDepthMax() - DepthSingleton::instance()->getDepthMin();
	if(abs(sub) < 15){
		mHandCheck = false;
		mSum = 0;
		mCount = 0;
		mMoveSceneFlag = MODE_NOW;
		return;
	}

	//モードをNow→END
	if(mHandCheck){
		mMoveSceneFlag = MODE_END;
	}
}

bool Authe::HandRang( const Vector2& kinectPos, const Vector2& checkPos1 ) {
	if(mHandCheckCount > 0) mHandCheckCount--;
	const float lb = checkPos1.x - 200;
	const float rb = checkPos1.x + 200;
	if( ( kinectPos.x < rb ) && ( kinectPos.x > lb ) ){
		const float tb = checkPos1.y - 200;
		const float bb = checkPos1.y + 200;
		if( ( kinectPos.y < bb ) && ( kinectPos.y > tb ) ){
			mHandCheckCount += 2 ;
			return true;
		}
	}
	return false;
}

AutheMode Authe::getMode() const {
	return mMoveSceneFlag;
}

bool Authe::handFlag() const {
	return mHandCheck;
}
