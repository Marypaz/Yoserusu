#include "Game/Wall.h"
#include "Mashiro/Mashiro.h"
#include "Mashiro/WorkSpace/WorkSpace.h"
#include "Mashiro/Graphics/Texture.h"
#include "Mashiro/Math/Vector3.h"
#include "Mashiro/Graphics/SpriteManager.h"
#include "Mashiro/Graphics/GraphicsManager.h"
#include "Mashiro/Math/Functions.h"
#include "Game/Ball.h"
#include "Game/Score.h"

#include "Util/ModelLoader.h"

#include "Shader/include/CocTrans.h"

#define PI 3.14159265f

using namespace Mashiro;
using namespace Mashiro::Graphics;
using namespace Mashiro::Math;

namespace {
	Mashiro::Math::Vector3 lPos[ 4 ];
};

Wall::Wall()
{
	mTextureCol[ 0 ].first = Graphics::Texture::create( "res/image/gato_color_R_UPZ_R.png" );
	mTextureCol[ 0 ].second = Vector3( 1, 0, 0 );
	mTextureCol[ 1 ].first = Graphics::Texture::create( "res/image/gato_color_R_UPZ_B.png" );
	mTextureCol[ 1 ].second = Vector3( 0, 0, 1 );
	mTextureCol[ 2 ].first = Graphics::Texture::create( "res/image/gato_color_R_UPZ_G.png" );
	mTextureCol[ 2 ].second = Vector3( 0, 1, 0 );

	lPos[ 0 ] = Vector3(-30,30,40);
	lPos[ 1 ] = Vector3(40,40,40);
    lPos[ 2 ] = Vector3(-50,-50,40);
	lPos[ 3 ] = Vector3(60,-60,40);

	ModelLoader* loader = ModelLoader::instance();

	mModel[0] = loader->createModel("res/model/WallLeftTop.pmd");
	mModel[1] = loader->createModel("res/model/WallRightTop.pmd");
	mModel[2] = loader->createModel("res/model/WallLeftBottom.pmd");
	mModel[3] = loader->createModel("res/model/WallRightBottom.pmd");

	for(int i=0; i<4; i++){
		mPos[i] = lPos[i];
		mModel[i].setAngle(Vector3(0));
		mModel[i].setScale(Vector3(1.f));
		mVec[i] = Vector3(0,0,0);
		mVec[i]-= mPos[i];
	}

	count = 0;
	seed = WorkSpace::instance().time();
	mRand = Random::create(seed*-1);
	rand = 0;
	nowColor = (WallColor)rand;

	setflg = true;
	delflg = false;
	resetflg = false;

	mCheck = 0;
	mWallState = WALL_NONE;
}

Wall::~Wall()
{
}

void Wall::Animation(Ball* ball)
{
	
	Wall::MoveWall();
	Wall::DelWall(ball->Color(),ball);
	Wall::ResetWall();
}

void Wall::Draw( bool timer )
{
	Graphics::Sprite sp = Graphics::Sprite::instance();

	CocTrans* coc = CocTrans::instance();
	CocTrans::ConstantBuffer* cb = NULL;
	if( coc->lock( (void**)&cb ) ){
		cb->mDrawType = CocTrans::TYPE_WALL;
		coc->unLock();
	}
	cb = nullptr;
	Graphics::Manager().setShader( CocTrans::instance()->shader() );

	for(int i = 0; i < 4; i++)
	{
		mModel[i].setPosition(mPos[i]);
		mModel[ i ].setTexture( mTextureCol[ rand ].first );
		mModel[ i ].setColor( mTextureCol[ rand ].second );

		Vector4 light = coc->instance()->worldLight( mModel[ i ].worldMatrix() );	
		Graphics::Manager().setLight( light );

		mModel[i].draw();
	}

	//タイマー終了
	if( !timer ){
		if( mWallState == WALL_FALSE ){
			sp.setColor( Vector3( 1, 0, 0 ) );
			sp.setTrance(sin(((double)mCheck/30)+3.141592)/2+0.3f);
			sp.setFillRectangle( Vector2( 0, 0 ), Vector2( 1024, 800 ) );
			sp.draw();
		}
	}
}

//		壁初期配置関数
void Wall::MoveWall()
{
	if(!setflg)
	{
		return;
	}
	//壁初期配置
	for(int i = 0; i < 4; i++)
	{
		Vector3 temp = Vector3(0,0,0);
		temp -= mPos[i];
		if(temp.length() < 40.9f)continue;
		
		mPos[i] += Vector3(mVec[i].x / 120 * 1.3f, mVec[i].y / 120 * 1.3f, 0);
	}
}

//壁消える関数
void Wall::DelWall(int color,Ball* ball)
{
	//壁ステート
	if( mWallState == WALL_FALSE ){
		++mCheck;
	}
	if( mCheck == 10 ){
		mWallState = WALL_NONE;
		mCheck = 0;
	}

	if(color != rand&& ball->IsMode() == Release)
	{
		mWallState = WALL_FALSE;
		SoundManager::instance()->playSe(SoundManager::SE_FALSE);
	}

	if(color == rand && !resetflg && ball->IsMode() == Release)
	{
		setflg = false;
		delflg = true;
		SoundManager::instance()->playSe(SoundManager::SE_OPEN);
		
	}

	if(!delflg)
	{
		return;
	}

	for(int i = 0; i < 4; i++)
	{
		Vector3 temp = Vector3(0,0,40);
		temp -= mPos[i];
		if(temp.length() < 75){
				mPos[i] -= Vector3(mVec[i].x / 120 * 1.3f, mVec[i].y / 120 * 1.3f, 0);
		}
		else
		{
			Score::instance()->Add();
			rand = mRand.getInt(0,3);
			nowColor = (WallColor)rand;
			if(Score::instance()->getScore()==1){
				rand = 1;
			}
			if(Score::instance()->getScore()==2){
				rand = 2;
			}
			mPos[0] = Vector3(-5.5f,5.5f,-30);
			mPos[1] = Vector3(5.5f,5.5f,-30);
			mPos[2] = Vector3(-5.5f,-5.5f,-30);
			mPos[3] = Vector3(5.5f,-5.5f,-30);
			delflg = false;
			resetflg = true;
			
			break;
		}
	}
}

//		壁再配置関数
void Wall::ResetWall()
{
	if(!resetflg)
	{
		return;
	}

	for(int i = 0; i < 4; i++)
	{
		if(mPos[i].z < 40)
		{
				mPos[i].z -= (mPos[i].z - (Math::sin(-PI/2 + PI/15 * count) + 90) / 2) / 20;
		}
		else
		{
			mPos[i].z = 40;
		}
	}

	if(mPos[3].z == 40)
	{
		resetflg = false;
	}
}

WallColor Wall::GetWallColor()
{
	return nowColor;
}

bool Wall::ResetFlag(){
	return resetflg;
}