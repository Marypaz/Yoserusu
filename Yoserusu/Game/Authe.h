#ifndef AUTHE_H
#define AUTHE_H

#include "Mashiro/Math/Vector2.h"
#include "Mashiro/Graphics/Bitmap.h"
#include "Enum.h"
using namespace Mashiro::Math;

class Authe {
public:
	Authe();
	~Authe();

	//�r�`�F�b�N
	bool HandRang( const Vector2& kinectPos, const Vector2& checkPos1 );

	bool handCheck();
	
	void depthCheck();

	void draw() ;

	AutheMode getMode() const;
	bool handFlag() const;
private: 
	//�F�؊���
	 bool mHandCheck;
	//�[�x�l
	int mDepth;
	int mCount;
	int mSum;
	int mHandCheckCount;

	bool check;

	//�V�[���|�b�v�t���O
	AutheMode mMoveSceneFlag;

	Mashiro::Graphics::Bitmap mBitmap[ 5 ];
};

#endif