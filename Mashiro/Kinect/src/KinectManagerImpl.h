#ifndef INCLUDE_MASHIRO_KINECT_MANAGER_IMPL_H_
#define INCLUDE_MASHIRO_KINECT_MANAGER_IMPL_H_

#include <Windows.h>
#include <NuiApi.h>
#include <stdio.h>

#include "Mashiro/Mashiro.h"
#include "Mashiro/Kinect/KinectManager.h"
#include "Mashiro/Math/Vector2.h"
#include "Mashiro/Graphics/src/TextureImpl.h"
#include "Mashiro/Threading/Event.h"
#include "Mashiro/Graphics/Bitmap.h"
#include "Mashiro/Graphics/src/BitmapImpl.h"

using namespace Mashiro;
using namespace Mashiro::Graphics;

namespace Mashiro {

namespace Kinect {

class KinectManagerImpl {
public:
	KinectManagerImpl( int w, int h ) : 
	mNuiSensor( 0 ),
	mWidth( w ),
	mHeight( h ),
	mDepthPixels( 0 ),
	mTrackingFlag( false ),
	mUserIndex( -1 ),
	mTrackingID( -1 ){
		HRESULT hr;
		createInstance();
		if( NULL != mNuiSensor ){
			DWORD flag =  NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_SKELETON | NUI_INITIALIZE_FLAG_USES_COLOR;
			if( SUCCEEDED( mNuiSensor->NuiInitialize( flag ) ) ){

				hr = mNuiSensor->NuiImageStreamOpen(
					NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480,
					0, 2, 0, &mColor );

				hr = mNuiSensor->NuiImageStreamOpen(
					NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_640x480,
					0, 2, 0, &mDepth );

				//イベント作成する
				mEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
				hr = mNuiSensor->NuiSetFrameEndEvent( mEvent, 0 );
	
				//スケルトン初期化
				hr = mNuiSensor->NuiSkeletonTrackingEnable( mEvent, NULL );
			}		
		}

		ZeroMemory(temp, sizeof(temp));
		if( NULL == mNuiSensor || FAILED( hr ) ){
			//どっかでエラーですね
			hr = E_FAIL;
			STRONG_ASSERT( hr != E_FAIL && "Kinectのストリームの初期化に失敗しました" );
		}
		//スケルトンの初期化
		for( int i = 0; i < SKELETON_INDEX_NONE; ++i ){
			mScreenPoints[ i ].x = mScreenPoints[ i ].y = 0.f;
		}
		mDepthPixels = NEW BYTE[ w * h * 4]; 

		mCameraBitmap = Bitmap::create( 640, 480, 0 );
		mDepthBitmap = Bitmap::create( 640, 480, 0 );
	}
	void createInstance(){
		HRESULT hr;
		//キネクト初期化
		INuiSensor* nui = NULL;

		//接続されているKinectの台数をカウント
		int sensorCount = 0;
		hr = NuiGetSensorCount( &sensorCount );
		STRONG_ASSERT( hr != E_POINTER && "Kinectが接続されていません" );
		//接続されている台数に応じて初期化
		for( int i = 0; i < sensorCount; ++i ){
			hr = NuiCreateSensorByIndex( i, &nui );
			STRONG_ASSERT( hr != E_POINTER && "パラメータハンドルがないわ" );
			STRONG_ASSERT( hr != E_OUTOFMEMORY && "メモリだめ" );
			//初期化よー
			hr = nui->NuiStatus();
			if( hr == S_OK ){
				mNuiSensor = nui;
				break;
			}
			mNuiSensor->Release();
		}
	}
	~KinectManagerImpl(){
		if( 0 != mDepthPixels ){
			SAFE_DELETE( mDepthPixels );
		}
		if( mNuiSensor ){
			mNuiSensor->NuiShutdown();
		}
		mNuiSensor = 0;
	}
	//更新
	void update(){
		if ( WAIT_OBJECT_0 == WaitForSingleObject(mEvent, 0 ) ){
		 	processColor();
			processDepth();
			processSkeleton();
		}
	}
	//色情報を更新する
	void processColor(){
		HRESULT hr;
		NUI_IMAGE_FRAME imageFrame;

		//カメラから色情報を取得
		hr = mNuiSensor->NuiImageStreamGetNextFrame(
			mColor, 0, &imageFrame );
		if( FAILED( hr ) ){
			return;
		}

		INuiFrameTexture* texture = imageFrame.pFrameTexture;

		//情報をロック
		NUI_LOCKED_RECT lockedRect;
		texture->LockRect( 0, &lockedRect, NULL, 0 );

		if( lockedRect.Pitch != 0 ){
			mCameraBitmap.copyFromMemory( static_cast< BYTE* >( lockedRect.pBits ) );
		}
		//情報をロック解除
		texture->UnlockRect( 0 );

		hr = mNuiSensor->NuiImageStreamReleaseFrame( mColor, &imageFrame );
	}
	void processDepth(){
		HRESULT hr;
		NUI_IMAGE_FRAME depthFrame;

		//カメラ深度情報を取得
		hr = mNuiSensor->NuiImageStreamGetNextFrame( mDepth, 0, &depthFrame);
		if( FAILED( hr ) ){
			return;
		}

		BOOL nearMode;
		INuiFrameTexture* texture;

		//深度情報をテクスチャに格納する
		hr = mNuiSensor->NuiImageFrameGetDepthImagePixelFrameTexture(
			mDepth, &depthFrame, &nearMode, &texture );

		//深度情報を取得
		NUI_LOCKED_RECT lockedRect;
		texture->LockRect( 0, &lockedRect, 0, 0 );
		if( lockedRect.Pitch != 0 ){
			//Nearモードの場合、最大深度と最小深度を変更
			int minDepth = (nearMode ? NUI_IMAGE_DEPTH_MINIMUM_NEAR_MODE : NUI_IMAGE_DEPTH_MINIMUM) >> NUI_IMAGE_PLAYER_INDEX_SHIFT;
			int maxDepth = (nearMode ? NUI_IMAGE_DEPTH_MAXIMUM_NEAR_MODE : NUI_IMAGE_DEPTH_MAXIMUM) >> NUI_IMAGE_PLAYER_INDEX_SHIFT;

			//	ピクセル操作の開始地点と終了地点を作成
			unsigned char* rgbrun = mDepthPixels;
			const NUI_DEPTH_IMAGE_PIXEL * pBufferRun = reinterpret_cast<const NUI_DEPTH_IMAGE_PIXEL *>(lockedRect.pBits);
			const NUI_DEPTH_IMAGE_PIXEL * pBufferEnd = pBufferRun + (640 * 480);

			int i=0;
			while ( pBufferRun < pBufferEnd )
			{
				USHORT depth = pBufferRun->depth;
				mDepthBuffer[i] = depth;
				BYTE intensity = static_cast<BYTE>(depth >= minDepth && depth <= maxDepth ? depth % 256 : 0);

				// Write out blue byte
				*(rgbrun++) = intensity;

				// Write out green byte
				*(rgbrun++) = intensity;

				// Write out red byte
				*(rgbrun++) = intensity;
				
				++rgbrun;

				++pBufferRun;
				i++;
			}
			mDepthBitmap.copyFromMemory( mDepthPixels );
		}


		//	矩形情報のロックを解除
		texture->UnlockRect(0);
		texture->Release();

		mNuiSensor->NuiImageStreamReleaseFrame(mDepth, &depthFrame);
	}
	//UserIDがまだ最初の状態で未設定ならそこで設定してあげる
	void processSkeleton(){
		HRESULT hr;

		NUI_SKELETON_FRAME skeletonFrame = { 0 };
		if( NULL == mNuiSensor ){
			return;
		}

		//スケルトン情報を取得する
		hr = mNuiSensor->NuiSkeletonGetNextFrame( 0, &skeletonFrame );
		if( FAILED( hr ) ){
			return;
		}
		hr = mNuiSensor->NuiTransformSmooth( &skeletonFrame, NULL );
		
		NUI_SKELETON_DATA& skeletonData = skeletonFrame.SkeletonData[0];
		int number = -1;
		bool found = false;
		if( !mTrackingFlag ){
			for (int i = 0 ; i < NUI_SKELETON_COUNT; ++i){
				//	骨格追跡の状態
				NUI_SKELETON_DATA& skeletonData = skeletonFrame.SkeletonData[i];
				NUI_SKELETON_TRACKING_STATE trackingState = skeletonFrame.SkeletonData[i].eTrackingState;
				skeletonData = skeletonFrame.SkeletonData[ i ];
				//トラッキングチェック
				if ( trackingState == NUI_SKELETON_TRACKED ){
					mTrackingID = skeletonData.dwTrackingID;
					mUserIndex = skeletonData.dwUserIndex;
					mTrackingFlag = true;
					return;
				}
			}
		} else {
			bool notfound = true;
			for (int i = 0 ; i < NUI_SKELETON_COUNT; ++i){
				//	骨格追跡の状態
				NUI_SKELETON_DATA& skeletonData = skeletonFrame.SkeletonData[i];
				NUI_SKELETON_TRACKING_STATE trackingState = skeletonFrame.SkeletonData[i].eTrackingState;
				skeletonData = skeletonFrame.SkeletonData[ i ];
				if( ( skeletonData.dwTrackingID == mTrackingID ) && ( skeletonData.dwUserIndex = mUserIndex ) ){
					notfound = false;
					if( trackingState == NUI_SKELETON_TRACKED ){
					//	スケルトンの骨格座標をスクリーン座標系に変換
						for (int j = 0; j < NUI_SKELETON_POSITION_COUNT; ++j){	
							if( skeletonData.eSkeletonPositionTrackingState[j] != NUI_SKELETON_POSITION_NOT_TRACKED ){
								skeletonToScreen( skeletonData.SkeletonPositions[j], j );
							}
						}
					} else if( trackingState == NUI_SKELETON_POSITION_COUNT ){
						//	スケルトンの骨格座標をスクリーン座標系に変換
						for (int j = 0; j < NUI_SKELETON_POSITION_COUNT; ++j){	
							if( skeletonData.eSkeletonPositionTrackingState[j] != NUI_SKELETON_POSITION_NOT_TRACKED )
							{
								skeletonToScreen(skeletonData.SkeletonPositions[j], j);
							}
						}
					} 
				}
			}
			if( notfound ){
				initTrackingFlag();
			}
		}
	}
	bool tracking()  {
		return mTrackingFlag;
	}
	Math::Vector2 index( SkeletonIndex index ) const {
		return mScreenPoints[ index ];
	}
	void skeletonToScreen( _Vector4& skeletonPoint, int i ){
		float x, y;
		LONG screenPointX, screenPointY;

		//骨格座標をスクリーン座標へと変換する
		NuiTransformSkeletonToDepthImage( skeletonPoint, &x, &y, NUI_IMAGE_RESOLUTION_640x480 );

		mNuiSensor->NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(
			NUI_IMAGE_RESOLUTION_640x480, NUI_IMAGE_RESOLUTION_640x480, 0, (LONG)x, (LONG)y, 0, &screenPointX, &screenPointY );

		//転送領域に合わせて最適化
		x = ( mWidth/640.f ) * static_cast< float >( screenPointX );
		y = ( mHeight/480.f ) * static_cast< float >( screenPointY );

		mScreenPoints[ i ].x = x; mScreenPoints[ i ].y = y;
	}
	void setWidth( int w ){
		mWidth = w;
	}
	void setHeight( int h ){
		mHeight = h;
	}
	Bitmap colorTexture() {
		return mCameraBitmap;
	}
	Bitmap depthTexture() {
		return mDepthBitmap;
	}
	Math::Vector2 skeletonPos( int i ){
		return mScreenPoints[ i ];
	}
	//スケルトンの深度値を取得
	int depthSkeleton( int i ){
		if( i < 0 ){
			return 0;
		}

		if( mScreenPoints[ i ].y >= 780 || mScreenPoints[ i ].x >= 1280)
		{
			return temp[i] * 6;
		}

		//とあるスケルトンのとある深度値を収得しますよ
		int x = static_cast<int>(mScreenPoints[ i ].x / ( mWidth/640.f ));
		int y = static_cast<int>(mScreenPoints[ i ].y / ( mHeight/480.f ));
		//配列添え字エラー回避
		if( x < 0 || y < 0 || x >= 480 || y >= 640  ){
			return 0;
		}

		
		if(mDepthBuffer[(y * 640) + x] >>8 <= 0)
		{
			return temp[i] * 6;
		}
		temp[i] = (mDepthBuffer[(y * 640) + x] >>8);
		return (mDepthBuffer[(y * 640) + x] >>8)*6;
	}
	//カメラの角度変更
	void setCamera( float radian ){
		//mNuiSensor->NuiCameraElevationSetAngle( NUI_
	}
	//トラッキングフラグ
	void initTrackingFlag(){
		mUserIndex = -1;
		mTrackingID = -1;
		mTrackingFlag = false;
		processSkeleton();
	}
	//キネクトセンサー用のインターフェース
	INuiSensor* mNuiSensor;
	HANDLE mColor;
	HANDLE mDepth;
	HANDLE mSkeleton;
	HANDLE mNextColorFrameEvent;
	HANDLE mEvent;
	//ユーザー番号
	bool mTrackingFlag;
	unsigned long mUserIndex;
	unsigned long mTrackingID;
	//スケルトン[スクリーン座標系変換後]
	Math::Vector2 mScreenPoints[ NUI_SKELETON_POSITION_COUNT ];
	//デプスバッファ
	USHORT mDepthBuffer[640*480];
	BYTE*			mDepthPixels;
	int temp[NUI_SKELETON_POSITION_COUNT];
	int mWidth;
	int mHeight;
	//ビットマップでKinectの画像
	Bitmap mDepthBitmap;
	Bitmap mCameraBitmap;

};
extern KinectManagerImpl* gManagerImpl; //インスタンス

}
}


#endif
