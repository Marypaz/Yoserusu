#ifndef INCLUDE_MASHIRO_GRAPHICS_SIN_FILELOADERIMPL_H_
#define INCLUDE_MASHIRO_GRAPHICS_SIN_FILELOADERIMPL_H_

#include <D3DX10math.h>

#include <stdio.h>
#include <iostream>
#include <vector>
#include "Mashiro/Mashiro.h"
#include "Mashiro/Base/Impl/ReferenceType.h"
#include "Mashiro/Graphics/Vertex.h"
#include "Mashiro/Graphics/IndexBuffer.h"
#include "Mashiro/Graphics/VertexBuffer.h"
#include "Mashiro/Graphics/Texture.h"
#include "Mashiro/Scene/SINFileLoader.h"
#include "Mashiro/Math/Vector4.h"
#include "Mashiro/Math/Vector2.h"
using namespace std;
using namespace Mashiro::Math;
using namespace Mashiro::Graphics;

namespace Mashiro{

namespace Scene {

class SINFileLoader::Impl : public Mashiro::ReferenceType {
public:
#pragma pack( push, 1 )
	unsigned long mBoneCount;
	//	FBX�X�L�����b�V���p���_�\����
	//�{�[���\����
	struct FBX_BONE{
		D3DXMATRIX mBindPose; //�������W
		D3DXMATRIX mNowPose; //���݂̃|�[�Y
	};
	FBX_BONE* mSINBone;
	struct FBX_ANIMETION_FRAME{
		int mStartFrame;
		int mEndFrame;
	};
	FBX_ANIMETION_FRAME* mSINAnimationFrame;
	struct FBX_FRAME_BONE{
		D3DXMATRIX mFramePose[ 27 ];					//4*4*4 = 64
	};
	FBX_FRAME_BONE* mSINFrameBone;
#pragma pack( pop )
public:
	D3DXMATRIX mBoneData[ 255 ];
public:
	Impl( const char* fileName ) : 
	mBoneCount( 0 ),
	mSINFrameBone( 0 ),
	mSINBone( 0 ),
	mSINAnimationFrame( 0 ){
		createAnimation( fileName );
	}
	~Impl(){
		destory();
	}
	void destory(){
		if( mSINBone ){
			SAFE_DELETE_ARRAY( mSINBone );
		}
		if( mSINFrameBone ){
			SAFE_DELETE_ARRAY( mSINFrameBone );
		}
		if( mSINAnimationFrame ){
			SAFE_DELETE_ARRAY( mSINAnimationFrame );
		}
	}
	void createAnimation( const char* fileName ){
		//���NEW�������̂�j��������
		destory();
		FILE* fp;
		fp = fopen( fileName, "rb" );
		if( fp != 0 ){
			//---------�{�[����-----------//
			fread( &mBoneCount, sizeof( mBoneCount ), 1, fp );

			//---------BoneData----------//
			mSINBone = NEW FBX_BONE[ mBoneCount ];
			fread( mSINBone, 128, mBoneCount, fp );

			//---------�t���[���̎���-------------------------//
			mSINAnimationFrame = NEW FBX_ANIMETION_FRAME();
			fread( mSINAnimationFrame, sizeof( FBX_ANIMETION_FRAME ), 1, fp );

			//-----�A�j���[�V�����f�[�^------//
			int frame = mSINAnimationFrame->mEndFrame - mSINAnimationFrame->mStartFrame;
			mSINFrameBone = NEW FBX_FRAME_BONE[ frame ]; 
			fread( mSINFrameBone, sizeof( FBX_FRAME_BONE ), frame, fp );

			fclose( fp );
		} else {
			STRONG_ASSERT( NULL && "fopen ERR" );
		}
	}

	//	���݂̎p���s����擾����
	D3DXMATRIX getCurrentPoseMatrix(int index){
		D3DXMATRIX inv;
		D3DXMatrixInverse(&inv,0,&mSINBone[index].mBindPose);	//	FBX�̃o�C���h�|�[�Y�̋t�s����擾
		D3DXMATRIX ret=inv*mSINBone[index].mNowPose;
		
		return ret;
	}

	//	�V���Ȏp���s������o���B
	void setNewPoseMatrices(int frame){
		//	�{�[���̐������[�v���s
		for(unsigned long i=0;i<mBoneCount;i++){		
			D3DXMATRIX mat = mSINFrameBone[ frame ].mFramePose[ i ];

			mSINBone[i].mNowPose = mat;
		}

		//	�V�F�[�_�[�ɓn�����߂̃����o�ϐ��ɃR�s�[
		for( unsigned long j = 0; j < mBoneCount; j++ )
		{
			//	�s����擾
			D3DXMATRIX mat = getCurrentPoseMatrix(j);

			//	�V�F�[�_�[�ɓn���̂œ]�u�s��Ƃ���
			D3DXMatrixTranspose(&mat,&mat);

			//	�s����R�s�[
			mBoneData[j]=mat;
		}
	}
};

}
}

#endif