#ifndef INCLUDE_SPLITE_H_
#define INCLUDE_SPLITE_H_

#include "Mashiro/Graphics/Bitmap.h"

/*
* @file Splite.h
* @brif 2D描画クラス 一枚一クラス
*/
class SpriteUtil{
public:
	SpriteUtil(){}
	/*
	* @brif コンストラクタ bitmapロード
	*/
	SpriteUtil( const char* fileName );
	/*
	* @brif デストラクタ
	*/
	~SpriteUtil();
	/*
	* @brif 2D描画
	* @param int x - x座標
	* @param int y - y座標
	*/
	void draw( int x, int y );
	/*
	* @brif 2D描画
	*/
	void draw( const Mashiro::Graphics::Bitmap& bitmap );
private:
	Mashiro::Graphics::Bitmap mBitmap;
};

#endif