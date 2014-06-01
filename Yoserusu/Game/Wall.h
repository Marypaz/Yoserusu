#ifndef WALL_H
#define WALL_H

#include "Mashiro/Scene/Model.h"
#include "Mashiro/Graphics/Texture.h"
#include "Mashiro/Math/Vector3.h"
#include "Mashiro/Math/Random.h"
#include "Game/Enum.h"
#include "Util/SoundManager.h"

#include <utility>

class Ball;

class Wall
{
public:
	Wall();

	~Wall();

	void Animation(Ball* ball);

	void Draw( bool timer );

	WallColor GetWallColor();

	bool ResetFlag();

private:
	
	void MoveWall();	//初期配置
	void DelWall(int color,Ball* ball);		//壁を離す
	void ResetWall();	//再配置

	std::pair< 
		Mashiro::Graphics::Texture, 
		Mashiro::Math::Vector3 > mTextureCol[ 3 ];
	Mashiro::Scene::Model			mModel[4];
	Mashiro::Math::Vector3			mPos[4];
	Mashiro::Math::Vector3			mVec[4];
	Mashiro::Math::Random				mRand;

	//2D
	int mCheck;
	WallState mWallState;

	int count;
	int seed;
	int rand;
	bool setflg;	//初期配置フラグ
	bool delflg;
	bool resetflg;  //再配置フラグ
	WallColor nowColor;
};
#endif