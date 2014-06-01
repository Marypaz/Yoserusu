#ifndef INCLUDE_MASHIRO_GRAPHICS_VERTEX_H_
#define INCLUDE_MASHIRO_GRAPHICS_VERTEX_H_

#include "Mashiro/Math/Matrix.h"
#include "Mashiro/Math/Vector4.h"
#include "Mashiro/Math/Vector3.h"
#include "Mashiro/Math/Vector2.h"

namespace Mashiro{

namespace Graphics{

using namespace Mashiro::Math;

class SkinMeshVertex{
public:
	Mashiro::Math::Vector3 mPosition;		//	ポジション
	Mashiro::Math::Vector3 mNormal;		//	法線
	Mashiro::Math::Vector2 mUV;			//	テクスチャ座標
	unsigned int mBoneIndex[4];		//	ボーンの番号	
	Mashiro::Math::Vector4 mBoneWeight;	//	ボーンの重み
};

class Vertex {
public:
	Mashiro::Math::Vector3 mPosition; //位置
	Mashiro::Math::Vector3 mNormal; //法線
	Mashiro::Math::Vector4 mColor; //頂点色
	Mashiro::Math::Vector2 mUV; //UV
};

struct BasicConstantBuffer{
	Matrix mWorld;
	Matrix mView;
	Matrix mProj;
	Vector4 mLight;
	Vector4 mDiffuse;
	Vector4 mAmbient;
	Vector4 mEye;
	Vector4 mSpecular;
};

struct FogConstant{
	Vector4 mFog;
	Vector4 mFogColor;
	Vector4 mHeight;
};

struct FbxSkinMeshConstant
{
	Matrix mBoneData[255];		//	ボーン行列
};

struct DefferdPostConstant{
	Vector4 mLightPos[ 4 ];
};

struct shadowConstantBuffer {
	Matrix worldInv; // ワールド逆行列 	
	Vector4 offset; // シャドウマップアクセス用のオフセット位置 
	Matrix vlp; // 光源に対しての変換行列
};

struct circleShadowConstantBuffer{
	unsigned int gCount[ 4 ];
	Vector4 mModelToLight[ 50 ]; //影を落とすモデルとライトを結ぶベクトル
	Vector4 mModelPos[ 50 ]; //モデルの位置
};

struct myConstantBuffer{

	//とりあえず外部から欲しいものを宣言
	int mPart; //描画するものごとに分岐したいから
	int mDummy;
};

}
}

#endif
