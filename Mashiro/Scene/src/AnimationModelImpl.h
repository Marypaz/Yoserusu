#ifndef INCLUDE_MASHIRO_GRAPHICS_ANIMATION_MODELIMPL_H_
#define INCLUDE_MASHIRO_GRAPHICS_ANIMATION_MODELIMPL_H_

#include "Mashiro/Mashiro.h"
#include "Mashiro/WorkSpace/WorkSpace.h"
#include "Mashiro/Base/Impl/ReferenceType.h"
#include "Mashiro/Math/Functions.h"
#include "Mashiro/Math/Vector3.h"
#include "Mashiro/Math/Matrix.h"
#include "Mashiro/Graphics/GraphicsManager.h"
#include "Mashiro/Scene/AnimationModel.h"
#include "Mashiro/Scene/src/FBXSkinMeshLoaderImpl.h"
#include "Mashiro/Scene/src/MRSFileLoaderImpl.h"
#include "Mashiro/Scene/src/SINFileLoaderImpl.h"
#include "Mashiro/WorkSpace/WorkSpace.h"
#include "Mashiro/Graphics/IndexBuffer.h"
#include "Mashiro/Graphics/VertexBuffer.h"
#include "Mashiro/Graphics/Texture.h"
#include "Mashiro/Graphics/RenderTarget.h"
#include "Mashiro/Graphics/Shader.h"
#include "Mashiro/Graphics/Vertex.h"
#include <vector>
using namespace std;

namespace Mashiro{

using namespace Mashiro::Graphics;

namespace Scene {
class AnimationModel::Impl : public Mashiro::ReferenceType {
public:
	Impl( FBXSkinMeshLoader::Impl* data ) : 
	mPosition( 0.f ),
	mAngle( 0.f ),
	mScale( 1.f ),
	mColor( 1.f ),
	mMaterialCount( data->mNumMaterial ),
	mTrans( 1.f ),
	mTexMaterialNumber( -1 ),
	mAnimaFrame( 0 ),
	mStartAnimaFrame( 0 ),
	mStopAnimaFrame( 0 ),
	mBoneNum( data->mBoneCount ),
	mMaterialColorNumber( -1 ),
	mRenderTexMaterialNumber( -1 ){
		//データを移していく
		mVertexBuffer = data->mVertex;
		mIndexBuffer = NEW IndexBuffer[ mMaterialCount ];
		mMaterial = NEW Material[ mMaterialCount ];
		for( unsigned long i = 0; i < mMaterialCount; ++i ){
			mIndexBuffer[ i ] = data->mIndex[ i ];
			mMaterial[ i ].mAmbient = data->mMeshMaterial[ i ].mAmbient;
			mMaterial[ i ].mDiffuse = data->mMeshMaterial[ i ].mDiffuse;
			mMaterial[ i ].mSpecular = data->mMeshMaterial[ i ].mSpecular;
//			mMaterial[ i ].mTexture = data->mMaterial[ i ].mTexture;
			mMaterial[ i ].mNumFace = data->mMeshMaterial[ i ].mNumFace;
		}
	}
	Impl( MRSFileLoader::Impl* data ) : 
	mPosition( 0.f ),
	mAngle( 0.f ),
	mScale( 1.f ),
	mColor( 1.f ),
	mMaterialCount( data->mMaterialCount ),
	mTrans( 1.f ),
	mTexMaterialNumber( -1 ),
	mAnimaFrame( 0 ),
	mStartAnimaFrame( 0 ),
	mStopAnimaFrame( 0 ),
	mBoneNum( data->mBoneCount ),
	mMaterialColorNumber( -1 ),
	mRenderTexMaterialNumber( -1 ){
		//データを移していく
		mVertexBuffer = data->mVertexBuffer;
		mIndexBuffer = NEW IndexBuffer[ mMaterialCount ];
		mMaterial = NEW Material[ mMaterialCount ];
		for( unsigned long i = 0; i < mMaterialCount; ++i ){
			mIndexBuffer[ i ] = data->mIndexBuffer[ i ];
			mMaterial[ i ].mAmbient = data->mMeshMaterial[ i ].mAmbient;
			mMaterial[ i ].mDiffuse = data->mMeshMaterial[ i ].mDiffuse;
			mMaterial[ i ].mSpecular = data->mMeshMaterial[ i ].mSpecular;
			ostringstream oss;
			oss.str( "" );
			oss << "Data/Model/" << data->mMeshMaterial[ i ].mTextureName;
			if(  -1 != oss.str().find( ".png" ) ){
				Graphics::Texture t = Graphics::Texture::create( oss.str().c_str() );
				mMaterial[ i ].mTexture = t;
			}
			mMaterial[ i ].mNumFace = data->mMeshMaterial[ i ].mNumFace;
		}
	}
	~Impl(){
		SAFE_DELETE_ARRAY( mIndexBuffer );
		SAFE_DELETE_ARRAY( mMaterial );
	}
	void setTexture( const Mashiro::Graphics::RenderTarget& tex, int num ){
		mUseRenderTargetTexture = tex;
		mRenderTexMaterialNumber = num;
	}
	void setTexture( const Mashiro::Graphics::Texture& tex, int num ){
		mUseTexture = tex;
		mTexMaterialNumber = num;
	}
	void setAngle( const Vector3& angle ){
		mAngle = angle;
	}
	void setPosition( const Vector3& position ){
		mPosition = position;
	}
	void setColor( const Vector3& color, int num ){
		mColor = color;
		mMaterialColorNumber = num;
	}
	void setPosition( const Vector2& in, const Matrix& proj, const Matrix& view ){
		Matrix convert;
		mPosition = convert.convertWorld( 
			Mashiro::WorkSpace::instance().width(),
			Mashiro::WorkSpace::instance().height(),
			&Vector3( in.x, in.y, 0.f ),
			proj, view );
	}
	void setScale( const Vector3& scale ){
		mScale = scale;
	}
	void setTransparaency( float c ){
		mTrans = c;
	}
	void setStartAnimation( int start ){
		mStartAnimaFrame = mAnimaFrame = start;
	}
	void setStopAnimation( int stop ){
		mStopAnimaFrame = stop;
	}
	void update(){
		++mAnimaFrame;
		if( mAnimaFrame > mStopAnimaFrame ){
			mAnimaFrame = mStartAnimaFrame;
		}
	}
	void setAnimation( FBXSkinMeshLoader::Impl* fbx ) {
		//現在のアニメーションのポーズを取得
		fbx->setNewPoseMatrices( mAnimaFrame );
		//行列を入れていきます
		for( unsigned long i = 0; i < fbx->mBoneCount; ++i ){
			mBoneData[ i ].m.set( fbx->mBoneData[ i ] );
		}
	}
	void setAnimation( SINFileLoader::Impl* sin ) {
		//現在のアニメーションのポーズを取得
		sin->setNewPoseMatrices( mAnimaFrame );
		//行列を入れていきます
		for( unsigned long i = 0; i < sin->mBoneCount; ++i ){
			mBoneData[ i ].m.set( sin->mBoneData[ i ] );
		}
	}
	void draw() {
		Graphics::Manager m = Graphics::Manager::instance();	
		//ワールド行列を作る
		Matrix w, trans, y, x, z, scale;

		//姿勢ボーン コンスタントバッファセット
		FbxSkinMeshConstant* cb = NULL;
		if( mShader.lock( (void**)&cb ) ){
			for( int j = 0; j < mBoneNum; ++j ){
				cb->mBoneData[ j ] = mBoneData[ j ];
			}
			mShader.unlock();
		}

		//描画(マテリアルごとに分割しながら描画します
		for( unsigned long i = 0; i < mMaterialCount; ++i ){
			if( mMaterial[ i ].mNumFace == 0 ){
				continue;
			}
			//Bufferをセット
			m.setVertexBuffer( mVertexBuffer );
			m.setIndexBuffer( mIndexBuffer[ i ] );
			//ワールド行列をセット
			trans.setTranslation( mPosition );
			y.setRotationY( mAngle.y );
			x.setRotationX( mAngle.x );
			z.setRotationZ( mAngle.z );
			scale.setScaling( mScale );
			w.setIdentity();
			w.setMul( w, scale );
			w.setMul( w, z );
			w.setMul( w, x );
			w.setMul( w, y );
			w.setMul( w, trans );
			//GraphicsManagerに渡す
			m.setWorldMatrix( w );
			m.setAmbient( Vector4( mMaterial[ i ].mAmbient ) );
			Vector3 dc( 0.f );
			if( mMaterialColorNumber == i ){
				dc.setMul( Vector3( mMaterial[ i ].mDiffuse.x, mMaterial[ i ].mDiffuse.y, mMaterial[ i ].mDiffuse.z ), mColor );
			} else {
				dc.setMul( Vector3( mMaterial[ i ].mDiffuse.x, mMaterial[ i ].mDiffuse.y, mMaterial[ i ].mDiffuse.z ), Vector3( 1.f ) );
			}
			m.setDiffuse( Vector4( dc.x, dc.y, dc.z, mTrans  ) );
			m.setSpecular( mMaterial[ i ].mSpecular );
			//テクスチャorマテリアル切り替え
			if( mTexMaterialNumber == i ){
				m.setTexture( mUseTexture, 0 );
			} else {
				 m.setTexture( mMaterial[ i ].mTexture, 0 );
			}
			if( mRenderTexMaterialNumber == i ){
				m.setTexture( mUseRenderTargetTexture, 0 );
			} else {
				 //m.setTexture( mMaterial[ i ].mTexture, 0 );
			}
			//シェーダセット
			m.setShader( mShader );
			m.drawIndexed( mIndexBuffer[ i ].indexNumber(), 0 );
		}
	}
	void draw( const Matrix& world ) {
		Graphics::Manager m = Graphics::Manager::instance();
		
		//姿勢ボーン コンスタントバッファセット
		FbxSkinMeshConstant* cb = NULL;
		if( mShader.lock( (void**)&cb ) ){
			for( int j = 0; j < mBoneNum; ++j ){
				cb->mBoneData[ j ] = mBoneData[ j ];
			}
			mShader.unlock();
		}

		//描画(マテリアルごとに分割しながら描画します
		for( unsigned long i = 0; i < mMaterialCount; ++i ){
			if( mMaterial[ i ].mNumFace == 0 ){
				continue;
			}
			//Bufferをセット
			m.setVertexBuffer( mVertexBuffer );
			m.setIndexBuffer( mIndexBuffer[ i ] );
			//GraphicsManagerに渡す
			m.setWorldMatrix( world );
			m.setAmbient( Vector4( mMaterial[ i ].mAmbient ) );
			Vector3 dc( 0.f );
			if( mMaterialColorNumber == i ){
				dc.setMul( Vector3( mMaterial[ i ].mDiffuse.x, mMaterial[ i ].mDiffuse.y, mMaterial[ i ].mDiffuse.z ), mColor );
			} else {
				dc.setMul( Vector3( mMaterial[ i ].mDiffuse.x, mMaterial[ i ].mDiffuse.y, mMaterial[ i ].mDiffuse.z ), Vector3( 1.f ) );
			}
			m.setDiffuse( Vector4( dc.x, dc.y, dc.z, mTrans  ) );
			m.setSpecular( mMaterial[ i ].mSpecular );
			//テクスチャorマテリアル切り替え
			if( mTexMaterialNumber == i ){
				m.setTexture( mUseTexture, 0 );
			} else {
				m.setTexture( mMaterial[ i ].mTexture, 0 );
			}
			if( mRenderTexMaterialNumber == i ){
				m.setTexture( mUseRenderTargetTexture, 0 );
			} else {
				//m.setTexture( mMaterial[ i ].mTexture, 0 );
			}
			//シェーダセット
			m.setShader( mShader );
			m.drawIndexed( mIndexBuffer[ i ].indexNumber(), 0 );
		}
	}
	//モデル情報
	Vector3 mPosition;
	Vector3 mAngle;
	Vector3 mScale;
	Vector3 mColor;
	float mTrans;
	//頂点バッファ
	VertexBuffer mVertexBuffer;
	//インデックスバッファ
	IndexBuffer* mIndexBuffer;
	//マテリアル
	struct Material {
		Vector4 mDiffuse;
		Vector4 mAmbient;
		Vector4 mSpecular;
		Graphics::Texture mTexture;
		const char* mFileName;
		unsigned long mNumFace; //マテリアルポリゴン数
	};
	Material* mMaterial;
	unsigned long mMaterialCount;
	
	//外部から強制貼り付け
	Graphics::Texture mUseTexture;
	Graphics::RenderTarget mUseRenderTargetTexture;
	int mRenderTexMaterialNumber;
	int mTexMaterialNumber;
	int mMaterialColorNumber;
	//アニメーション関連
	int mAnimaFrame; //現在のアニメーションフレーム
	int mStartAnimaFrame;
	int mStopAnimaFrame; //ストップ値のフレーム
	Matrix mBoneData[ 255 ];
	int mBoneNum;

	//シェーダ(決め打ち)
	Mashiro::Graphics::Shader mShader;

};

}
}


#endif
