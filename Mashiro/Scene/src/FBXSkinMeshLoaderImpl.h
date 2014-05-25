#ifndef INCLUDE_MASHIRO_GRAPHICS_FBX_SKIN_MESH_LOADERIMPL_H_
#define INCLUDE_MASHIRO_GRAPHICS_FBX_SKIN_MESH_LOADERIMPL_H_

#define FBXSDK_new_API

#include <D3DX10math.h>

#include <fbxsdk.h>
#include "Mashiro/Mashiro.h"
#include "Mashiro/WorkSpace/WorkSpace.h"
#include "Mashiro/Base/Impl/ReferenceType.h"
#include "Mashiro/Graphics/IndexBuffer.h"
#include "Mashiro/Graphics/VertexBuffer.h"
#include "Mashiro/Graphics/Texture.h"
#include "Mashiro/Scene/FBXSkinMeshLoader.h"
#include "Mashiro/Math/Vector4.h"
#include "Mashiro/Math/Vector2.h"
#include "Mashiro/Math/Matrix.h"


#include <fstream>
#include <sstream>

using namespace std;
using namespace Mashiro::Math;
using namespace Mashiro::Graphics;

namespace Mashiro{

namespace Scene {

class FBXSkinMeshLoader::Impl : public Mashiro::ReferenceType {
public:
#pragma pack( push, 1 )
	unsigned long mVertexCount; //���_��
	unsigned long mIndexCount;
	unsigned long mUVCount;
	unsigned long mMaterialCount; //�}�e���A���� = �C���f�b�N�X�o�b�t�@��
	unsigned long mBoneCount;
	//	FBX�X�L�����b�V���p���_�\����
	struct FBX_SKINMESH_VERTEX
	{
		Vector3 mPosition;		//	�|�W�V����            4*3=12
		Vector3 mNormal;		//	�@��                  4*3=12
		Vector2 mUV;			//	�e�N�X�`�����W        4*2=8
		Vector4 mColor;         //  �J���[�l              4*4=16
		Vector4 mBoneWeight;	//	�{�[���̏d��          4*4=16
		unsigned int mBoneIndex[4];		//	�{�[���̔ԍ�  4*4=16
	};
	FBX_SKINMESH_VERTEX* mMRSVertex;
	//�C���f�b�N�X�\����
	struct FBX_INDEX{
		unsigned long mIndex[ 5000 ];	//4
		int mCount;				//4
	};
	FBX_INDEX* mMRSIndex;
	//�{�[���\����
	struct FBX_BONE{
		D3DXMATRIX mBindPose; //�������W		(4*4)*4=64
		D3DXMATRIX mNowPose; //���݂̃|�[�Y		(4*4)*4=64
		FBX_BONE()
		{
			D3DXMatrixIdentity( &mNowPose );
		}
	};
	FBX_BONE* mMRSBone;
	struct FBX_ANIMETION_FRAME{
		int mStartFrame;
		int mEndFrame;
	};
	FBX_ANIMETION_FRAME* mMRSAnimationFrame;
	struct FBX_FRAME_BONE{
		D3DXMATRIX mFramePose[ 27 ];					//4*4*4 = 64
	};
	FBX_FRAME_BONE* mMRSFrameBone;
	struct FBX_POLY_TABLE{
		int mPolyIndex[ 20 ]; //�|���S���ԍ�				4*20=80
		int mIndex123[ 20 ]; //3�̒��_�̂����A���Ԗڂ�	3*20=80
		int mNumRef; //�����Ă���|���S���̐�					  4
		FBX_POLY_TABLE()
		{	
			ZeroMemory(this, sizeof(FBX_POLY_TABLE));
		}
	};
	FBX_POLY_TABLE* mMRSPolyTabel;
	//�}�e���A�����
	struct MeshMaterial{
		Vector4 mAmbient;					//4*4=16
		Vector4 mDiffuse;					//4*4=16
		Vector4 mSpecular;					//4*4=16				
		unsigned long mNumFace;					 //4
		MeshMaterial()
		{
			ZeroMemory(this,sizeof(MeshMaterial));
		}
	};
	MeshMaterial* mMeshMaterial;
	unsigned long mFaceCount;
#pragma pack( pop )
public:
	Impl() :
	mFaceCount( 0 ),
	mVertexCount( 0 ), //���_��
	mIndexCount( 0 ),
	mUVCount( 0 ),
	mMaterialCount( 0 ), //�}�e���A���� = �C���f�b�N�X�o�b�t�@��
	mBoneCount( 0 ),
	mMeshMaterial( 0 ),
	mIndex( 0 ),
	mMRSAnimationFrame( 0 ),
	mMRSVertex( 0 ),
	mMRSIndex( 0 ),
	mMRSPolyTabel( 0 ),
	mMRSFrameBone( 0 ),
	mMRSBone( 0 ){
	}
	~Impl(){
		if( mMRSBone ){
			SAFE_DELETE_ARRAY( mMRSBone );
		}
		if( mCluster ){
			SAFE_DELETE_ARRAY( mCluster );
		}
		if( mMeshMaterial ){
			SAFE_DELETE_ARRAY( mMeshMaterial );
		}
		if( mIndex ){
			SAFE_DELETE_ARRAY( mIndex );
		}
		if(	mMRSVertex ){
			SAFE_DELETE_ARRAY( mMRSVertex );
		}
		if( mManager ){
			mManager->Destroy();
			mManager = 0;
		}
	}
	void createModel( const char* fileName ){
		//������
		if( bool hr = !initFBX( fileName ) ){
			STRONG_ASSERT( hr != false && "FBX���[�_���������s" );
		}
		//���[�g�m�[�h�̎擾
		FbxNode* rootNode = mScene->GetRootNode();
		FbxMesh* mesh = 0;

		getRootNode( &rootNode, &mesh );

		//���O�ɒ��_���A�|���R�����𒲂ׂ�
		mVertexCount = mesh->GetControlPointsCount();
		mUVCount = mesh->GetTextureUVCount();
		mFaceCount = mesh->GetPolygonCount();

		//�ꎞ�I�ȃ������m��
		( mVertexCount < mUVCount ) ? mMRSVertex = NEW FBX_SKINMESH_VERTEX[ mUVCount ] : mMRSVertex = NEW FBX_SKINMESH_VERTEX[ mVertexCount ];
		int* faceBuffer = NEW int[ mFaceCount*3 ];

		//���_�ǂݍ���
		loadVertex( mesh, mMRSVertex );

		//�|���R�����
		loadIndex( mesh, faceBuffer );

		//�@���ǂݍ���
		loadNormal( mesh, faceBuffer, mMRSVertex );

		//�J���[�̓ǂݍ���
		loadColor( mesh, faceBuffer, mMRSVertex );

		//�e�N�X�`�����W�ǂݍ���
		loadUv( mesh, mMRSVertex );

		//�}�e���A���̓ǂݍ���
		loadMaterial( mesh );

		//���_����|���S�����t�����������̂ŁA�t�����e�[�u��
		mMRSPolyTabel = NEW FBX_POLY_TABLE[ mVertexCount ];
		for( unsigned long i = 0; i < mVertexCount; ++i ){
			for( unsigned long k = 0; k < mFaceCount; ++k ){
				for( int m = 0; m < 3; ++m ){
					if( mesh->GetPolygonVertex( k, m ) == i ){
						mMRSPolyTabel[ i ].mPolyIndex[ mMRSPolyTabel[ i ].mNumRef ] = k;
						mMRSPolyTabel[ i ].mIndex123[ mMRSPolyTabel[ i ].mNumRef ] = m;
						mMRSPolyTabel[ i ].mNumRef++;
					}
				}
			}
		}

		//�X�L�����i�V���C���g�A�E�F�C�g�j�ǂݍ���
		readSkinInfo( mesh, mMRSVertex, mMRSPolyTabel );
		
		//���_�o�b�t�@�𐶐�

		if( mVertexCount < mUVCount ) {
			mVertexCount = mUVCount;
		}
		mVertex = VertexBuffer::create( mVertexCount, sizeof( SkinMeshVertex ) );

		SkinMeshVertex* v = NULL;
		if( mVertex.lock((void**)&v)){
			for( unsigned long i = 0; i < mVertexCount; i++ ){
				v[ i ].mPosition = mMRSVertex[ i ].mPosition;
				v[ i ].mNormal = mMRSVertex[ i ].mNormal;
				v[ i ].mUV = mMRSVertex[ i ].mUV;
				v[ i ].mBoneWeight = mMRSVertex[ i ].mBoneWeight;
				memcpy( v[ i ].mBoneIndex, mMRSVertex[ i ].mBoneIndex, sizeof( mMRSVertex[ i ].mBoneIndex ) );
			}

			//�A�j���[�V�����t���[�������o��
			frameMatrix( 0, 500 );

			mVertex.unlock();
		}

		SAFE_DELETE_ARRAY( mMRSPolyTabel );
		SAFE_DELETE_ARRAY( faceBuffer );
	}
	//������
	bool initFBX( const char* fileName ){
		mManager = FbxManager::Create();

		mImport = FbxImporter::Create( mManager, "importer" );

		mImport->Initialize( fileName );

		mScene = FbxScene::Create( mManager, "scene" );
		
		mImport->Import( mScene );

		return true;
	}
	//���[�g�m�[�h���擾
	void getRootNode( FbxNode** root, FbxMesh** mesh ){
		FbxNodeAttribute *attr=(*root)->GetNodeAttribute();
		FbxNodeAttribute::EType type;

		//	�e�Ƀf�[�^�������Ă���P�[�X�͏��Ȃ�
		if(attr!=NULL)
		{
			//	�^�C�v���m�F
			type =attr->GetAttributeType();

			//	���b�V���������ꍇ
			if(type==FbxNodeAttribute::eMesh)
			{
				//	�f�[�^�ǂݍ���
				(*mesh) = (*root)->GetMesh();
			}
		}
		else
		{
			//	�Ԃ牺�����Ă���q�m�[�h�����擾
			int iNumChild = (*root)->GetChildCount();

			//	�m�[�h�������[�v
			for(int i=0;i<iNumChild;i++)
			{
				//	�m�[�h���擾����
				FbxNode* pChildNode = (*root)->GetChild(i);

				//	�m�[�h�����݂��Ȃ�!?
				if(!pChildNode) continue;

				//	�擾�����m�[�h�̃^�C�v���m�F
				attr = pChildNode->GetNodeAttribute();
				type = attr->GetAttributeType();

				//	�m�[�h�^�C�v�����b�V���̏ꍇ
				if( type == FbxNodeAttribute::eMesh)
				{
					//	�f�[�^�ǂݍ���
					(*mesh) = pChildNode->GetMesh();
					break;
				}
			}		
		}
	}
	//�C���f�b�N�X�ǂݍ���
	void loadIndex( const FbxMesh* mesh, int* index ){
		//Fbx�̒��_�C���f�b�N�X�擪�̃|�C���^���擾
		int* iIndex = mesh->GetPolygonVertices();
		//4*�ʐ�*3�̃C���f�b�N�X�R�s�[
		memcpy( index, iIndex, sizeof(int)*mFaceCount*3 );
	}
	//���_�̓ǂݍ���
	void loadVertex( FbxMesh* mesh, FBX_SKINMESH_VERTEX* vb ){
		int* piIndex=mesh->GetPolygonVertices();//	���_�C���f�b�N�X�ǂݍ���

		//	���_���ł͂Ȃ��A�ʂ��璸�_�f�[�^���擾����
		for(unsigned long i = 0; i < mFaceCount; i++)
		{		
			//	�|���S�����\������C���f�b�N�X���擾
			int startIndex=mesh->GetPolygonVertexIndex(i);
		
			//	���_�ԍ����擾
			int idx0=piIndex[ startIndex ];		
			int idx1=piIndex[ startIndex+1 ];
			int idx2=piIndex[ startIndex+2 ];

			//	UV���̂ق��������ꍇ��UV�C���f�b�N�X����ɂ���
			if(mVertexCount<mUVCount)	
			{
				idx0 = mesh->GetTextureUVIndex(i,0,FbxLayerElement::eTextureDiffuse);
				idx1 = mesh->GetTextureUVIndex(i,1,FbxLayerElement::eTextureDiffuse);
				idx2 = mesh->GetTextureUVIndex(i,2,FbxLayerElement::eTextureDiffuse);
			}

			//	���_�f�[�^0�̎擾
			int index=mesh->GetPolygonVertex(i,0);
			FbxVector4 Coord=mesh->GetControlPointAt(index);
			vb[idx0].mPosition.x =  static_cast<float>(Coord.mData[0]);
			vb[idx0].mPosition.y =  static_cast<float>(Coord.mData[1]);
			vb[idx0].mPosition.z =  static_cast<float>(Coord.mData[2]);

			//	���_�f�[�^1�̎擾
			index=mesh->GetPolygonVertex(i,1);
			Coord=mesh->GetControlPointAt(index);
			vb[idx1].mPosition.x =  static_cast<float>(Coord.mData[0]);
			vb[idx1].mPosition.y =  static_cast<float>(Coord.mData[1]);
			vb[idx1].mPosition.z =  static_cast<float>(Coord.mData[2]);

			//	���_�f�[�^2�̎擾
			index=mesh->GetPolygonVertex(i,2);
			Coord=mesh->GetControlPointAt(index);
			vb[idx2].mPosition.x =  static_cast<float>(Coord.mData[0]);
			vb[idx2].mPosition.y =  static_cast<float>(Coord.mData[1]);
			vb[idx2].mPosition.z =  static_cast<float>(Coord.mData[2]);

		}
	}
	//�@���̓ǂݍ���
	void loadNormal( FbxMesh* mesh, const int* index, FBX_SKINMESH_VERTEX* vb ){
		FbxVector4 normal;

		for(unsigned long i = 0; i < mFaceCount; i++)
		{	
			//	�|���S�����\�����Ă��钸�_�̃C���f�b�N�X�o�b�t�@
			//	�J�n�ʒu���擾
			int startIdx=mesh->GetPolygonVertexIndex(i);
		
			//	�J�n�ʒu���N�_�Ƃ��A�O���_���̃C���f�b�N�X���擾
			int idx0 = index[ startIdx ];
			int idx1 = index[ startIdx + 1 ];
			int idx2 = index[ startIdx + 2 ];

			if(idx0<=0) continue;
			//	UV���̂ق��������ꍇ��UV�C���f�b�N�X����ɂ���
			if(mVertexCount<mUVCount)	
			{
				idx0 = mesh->GetTextureUVIndex(i,0,FbxLayerElement::eTextureDiffuse);
				idx1 = mesh->GetTextureUVIndex(i,1,FbxLayerElement::eTextureDiffuse);
				idx2 = mesh->GetTextureUVIndex(i,2,FbxLayerElement::eTextureDiffuse);
			}

			//	�|���S�����\�����Ă���0���_�̖@�����擾
			mesh->GetPolygonVertexNormal(i,0,normal);

			//	�擾�����@���𒸓_���ɃR�s�[���Ă����B
			vb[idx0].mNormal.x = -static_cast<float>(normal[0]);
			vb[idx0].mNormal.y = static_cast<float>(normal[1]);
			vb[idx0].mNormal.z = static_cast<float>(normal[2]);

			//	�|���S�����\�����Ă���1���_�̖@�����擾
			mesh->GetPolygonVertexNormal(i,1,normal);
			vb[idx1].mNormal.x = -static_cast<float>(normal[0]);
			vb[idx1].mNormal.y = static_cast<float>(normal[1]);
			vb[idx1].mNormal.z = static_cast<float>(normal[2]);

			//	�|���S�����\�����Ă���2���_�̖@�����擾
			mesh->GetPolygonVertexNormal(i,2,normal);
			vb[idx2].mNormal.x = -static_cast<float>(normal[0]);
			vb[idx2].mNormal.y = static_cast<float>(normal[1]);
			vb[idx2].mNormal.z = static_cast<float>(normal[2]);
		}
	}
	//�e�N�X�`���̓ǂݍ���
	void loadUv( FbxMesh* mesh, FBX_SKINMESH_VERTEX* vb ){
		const FbxLayerElementUV* pUV=0;

		//	FBX�e�N�X�`�����W�̐擪�|�C���^���擾
		pUV = mesh->GetLayer(0)->GetUVs();

		//	UV�̐������[�v
		for(unsigned long i = 0; i < mUVCount; i++)
		{
			FbxVector2 v2;

			//	�e�N�X�`�����W�擾
			v2=pUV->GetDirectArray().GetAt(i);

			//	�e�N�X�`�����W�𒸓_�z��ɃR�s�[
			vb[i].mUV.x = static_cast<float>(v2.mData[0]);
			vb[i].mUV.y = 1.0f - static_cast<float>(v2.mData[1]);
		}
	}
	//�J���[�̓ǂݍ���
	void loadColor( FbxMesh* mesh, const int* index, FBX_SKINMESH_VERTEX* vb ){
		const FbxLayerElementVertexColor* color=0;

		//	FBX�e�N�X�`�����W�̐擪�|�C���^���擾
		color = mesh->GetLayer(0)->GetVertexColors();

		//	UV�̐������[�v
		if( color ){
			for(unsigned long i = 0; i < mUVCount; i++)
			{
				FbxColor c;

				//	�e�N�X�`�����W�擾
				c = color->GetDirectArray().GetAt( i );

				//	�e�N�X�`�����W�𒸓_�z��ɃR�s�[
				vb[i].mColor.x = static_cast<float>(c.mRed);
				vb[i].mColor.y = static_cast<float>(c.mGreen);
				vb[i].mColor.z = static_cast<float>(c.mBlue);
				vb[i].mColor.w = static_cast<float>(c.mAlpha);
			}
		} else {
			for(unsigned long i = 0; i < mUVCount; i++){
				vb[i].mColor.set( 1.f, 1.f, 1.f, 1.f );
			}
		}
	}
	//�}�e���A���̓ǂݍ���
	void loadMaterial( FbxMesh* mesh ){
		FbxNode* pNode	= mesh->GetNode();
		mNumMaterial	= pNode->GetMaterialCount();
		mMeshMaterial = NEW MeshMaterial[mNumMaterial];
		mMRSIndex = NEW FBX_INDEX[ mNumMaterial ];
		//	�}�e���A���̐������C���f�b�N�X�o�b�t�@�[���쐬
		mIndex = NEW IndexBuffer[mNumMaterial];

		for(unsigned long i=0;i<mNumMaterial;i++)
		{
			//	�t�H�����f����z��
			FbxSurfaceMaterial* pMaterial=pNode->GetMaterial(i);
			FbxSurfacePhong* pPhong=(FbxSurfacePhong*)pMaterial;


			//	�A���r�G���g�J���[(����)

			FbxDouble3 d3Ambient=pPhong->Ambient;
			mMeshMaterial[i].mAmbient.x = static_cast<float>(d3Ambient.mData[0]);
			mMeshMaterial[i].mAmbient.y = static_cast<float>(d3Ambient.mData[1]);
			mMeshMaterial[i].mAmbient.z = static_cast<float>(d3Ambient.mData[2]);
			mMeshMaterial[i].mAmbient.w = 1.0f;

			//	�f�B�t���[�Y�J���[(�g�U���ˌ�)
			FbxDouble3 d3Diffuse=pPhong->Diffuse;
			mMeshMaterial[i].mDiffuse.x = static_cast<float>(d3Diffuse.mData[0]);
			mMeshMaterial[i].mDiffuse.y = static_cast<float>(d3Diffuse.mData[1]);
			mMeshMaterial[i].mDiffuse.z = static_cast<float>(d3Diffuse.mData[2]);
			mMeshMaterial[i].mDiffuse.w = 1.0f;
			
			//	�X�y�L�����J���[(���ʔ��ˌ�)
			//FbxDouble3 d3Specular=pPhong->Specular;
			//mMaterial[i].mSpecular.x = static_cast<float>(d3Specular.mData[0]);
			//mMaterial[i].mSpecular.y = static_cast<float>(d3Specular.mData[1]);
			//mMaterial[i].mSpecular.z = static_cast<float>(d3Specular.mData[2]);
			//mMaterial[i].mSpecular.w = 1.0f;
			
			//�e�N�X�`���[�i�f�B�t���[�Y�e�N�X�`���[�̂݁j
			FbxProperty lProperty;
			lProperty=pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
			FbxTexture* texture= lProperty.GetSrcObject<FbxTexture>();

			//	�e�N�X�`���̓ǂݍ���
			if(texture)
			{			
				//mMaterial[ i ].mFileName = texture->GetName();
				//mMaterial[i].mTexture = Texture::create( "robo3.png" );
			}	

			//�}�e���A���̐������C���f�b�N�X�o�b�t�@�[���쐬
			int iCount=0;
			unsigned long* pIndex=NEW unsigned long[mFaceCount*3];//�Ƃ肠�����A���b�V�����̃|���S�����Ń������m��
	
			//���̃}�e���A���ł���C���f�b�N�X�z����̊J�n�C���f�b�N�X�𒲂ׂ�@����ɃC���f�b�N�X�̌������ׂ�		
			iCount=0;
			for(unsigned long k=0;k<mFaceCount;k++)
			{
				const FbxLayerElementMaterial* mat = mesh->GetLayer(0)->GetMaterials();
				int matId=mat->GetIndexArray().GetAt(k);

				if(matId==i)
				{
					//	���_�����UV�������ꍇ�AUV�C���f�b�N�X���g�p
					if(mVertexCount<mUVCount) 
					{
						pIndex[iCount]=mesh->GetTextureUVIndex(k,0,FbxLayerElement::eTextureDiffuse);
						pIndex[iCount+1]=mesh->GetTextureUVIndex(k,1,FbxLayerElement::eTextureDiffuse);
						pIndex[iCount+2]=mesh->GetTextureUVIndex(k,2,FbxLayerElement::eTextureDiffuse);
					}
					else
					{
						pIndex[iCount]=mesh->GetPolygonVertex(k,0);
						pIndex[iCount+1]=mesh->GetPolygonVertex(k,1);
						pIndex[iCount+2]=mesh->GetPolygonVertex(k,2);
					}		
				
					iCount+=3;
				}
			}

			mIndex[ i ] = Mashiro::Graphics::IndexBuffer::create( iCount );
			mMRSIndex[ i ].mCount = iCount;
			unsigned long* indices = mIndex[ i ].lock();
			for( int x = 0; x < iCount; x++ ){
				indices[ x ] = pIndex[ x ];
				mMRSIndex[ i ].mIndex[ x ] = pIndex[ x ];
			}
			mIndex[ i ].unlock( &indices );

			mMeshMaterial[ i ].mNumFace = iCount / 3;

			SAFE_DELETE_ARRAY( pIndex );
		}
	}

	//	���݂̎p���s����擾����
	D3DXMATRIX getCurrentPoseMatrix(int index){
		D3DXMATRIX inv;
		D3DXMatrixInverse(&inv,0,&mMRSBone[index].mBindPose);	//	FBX�̃o�C���h�|�[�Y�̋t�s����擾
		D3DXMATRIX ret=inv*mMRSBone[index].mNowPose;
		
		return ret;
	}

	//	�V���Ȏp���s������o���B
	void setNewPoseMatrices(int frame){
		FbxTime time;
		//	�t���[������A�j���[�V�������Ԃ��擾
		time.SetTime(0,0,0,frame,0,FbxTime::eFrames30);

		//	�{�[���̐������[�v���s
		for(unsigned long i=0;i<mBoneCount;i++){		
			D3DXMATRIX mat = mMRSFrameBone[ frame ].mFramePose[ i ];

			mMRSBone[i].mNowPose = mat;
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

	//	FBX�t�@�C������{�[���������o��
	void readSkinInfo(
		FbxMesh* mesh, 
		FBX_SKINMESH_VERTEX* vb,
		FBX_POLY_TABLE* polyTable ){
		
		//FBX���璊�o���ׂ����́A���_���Ƃ̃{�[���C���f�b�N�X�A���_���Ƃ̃{�[���E�F�C�g�A�o�C���h�s��A�|�[�Y�s��@��4����
	
		int i,k;
		int num = 0;//�{�[����

		//�f�t�H�[�}�[�𓾂� 
		FbxDeformer* deformer = mesh->GetDeformer(0);
		FbxSkin* skinInfo = static_cast<FbxSkin*>(deformer);
	
		//	�{�[���̐����擾
		num = skinInfo->GetClusterCount();
		mCluster = NEW FbxCluster*[ num ];		//	�{�[���𐶐�

		//	�{�[���̏����擾����
		for( i=0;i<num;i++)
		{
			mCluster[i] = skinInfo->GetCluster(i);
		}

		//	�ʏ�̏ꍇ
		if(mVertexCount >= mUVCount)
		{
			//	���ꂼ��̃{�[���ɉe�����󂯂钸�_�𒲂ׂ�
			for(i = 0; i < num; i++)
			{
				//	�ΏۂƂȂ�{�[���ɉe�����󂯂钸�_�̐��𒲂ׂ�
				int  numIndex = mCluster[i]->GetControlPointIndicesCount();

				//	�{�[���ɑΉ����Ă���C���f�b�N�X���擾
				int* index = mCluster[i]->GetControlPointIndices();

				//	�E�F�C�g�����擾
				double* pdWeight=mCluster[i]->GetControlPointWeights();

				//	���_������C���f�b�N�X�����ǂ��āA���_�T�C�h�Ő�������
				for(k = 0; k < numIndex; k++)
				{
					//FBX��CG�\�t�g���{�[��4�{�ȓ��Ƃ͌���Ȃ��B5�{�ȏ�̏ꍇ�́A�d�݂̑傫������4�{�ɍi��
					for(int m= 0; m < 4; m++) 
					{
						if( pdWeight[k] > vb[ index[k] ].mBoneWeight[m])
						{
							vb[ index[k] ].mBoneIndex[m] = i;
							vb[ index[k] ].mBoneWeight[m] = static_cast<float>(pdWeight[k]);				
							break;
						}
					}				
				}
			}
		}
		//UV�C���f�b�N�X�x�[�X�̏ꍇ�@�i���_��<UV���j
		else
		{
			int polyIndex=0;
			int UVIndex=0;

			//	���ꂼ��̃{�[���ɉe�����󂯂钸�_�𒲂ׂ�
			for( i = 0; i < num ;i++)
			{
				//	�ΏۂƂȂ�{�[���ɉe�����󂯂钸�_�̐��𒲂ׂ�
				int numIndex = mCluster[i]->GetControlPointIndicesCount();

				//	�Ή����Ă��钸�_�̃C���f�b�N�X���擾
				int* index = mCluster[i]->GetControlPointIndices();

				//	���ꂼ��̒��_�ɑ΂���d�݂��擾
				double* pdWeight = mCluster[i]->GetControlPointWeights();

				//���_������C���f�b�N�X�����ǂ��āA���_�T�C�h�Ő�������
				for(k = 0; k < numIndex; k++)
				{				
					//	���̒��_���܂�ł���|���S�����ׂĂɁA�{�[���ƃE�F�C�g��K�p����
					for(int p = 0; p < polyTable[ index[k] ].mNumRef; p++)
					{	
						//	���_ �� ������|���S�� �� ���̃|���S����UV�C���f�b�N�X�Ƌt����
						polyIndex = polyTable[ index[k] ].mPolyIndex[p];
						UVIndex = mesh->GetTextureUVIndex(polyIndex,polyTable[ index[k] ].mIndex123[p],FbxLayerElement::eTextureDiffuse);
								
						for(int m = 0; m < 4; m++) 
						{	
							if( pdWeight[k] > vb[ UVIndex ].mBoneWeight[m] )
							{
								vb[UVIndex].mBoneIndex[ m] = i;
								vb[UVIndex].mBoneWeight[m] = static_cast<float>(pdWeight[k]);
								break;	
							}		
						}
					}	
				}
			}
		}

		//
		//�{�[���𐶐�
		mBoneCount = num;
		mMRSBone = NEW FBX_BONE[ num ];

		for( int j = 0; j < mBoneCount; j++)
		{
			FbxAMatrix mat;
			mCluster[j]->GetTransformLinkMatrix(mat);
		
			for(int x = 0; x < 4; x++)
			{
				for(int y = 0; y < 4; y++)
				{
					mMRSBone[j].mBindPose(y,x) = static_cast<float>(mat.Get(y,x));
					mMRSBone[j].mBindPose._11 *=-1;
				}
			}
		}
	}

	//�A�j���[�V�����{�[�������o��
	void frameMatrix( int frame1, int frame2 ){
		FbxTime time;
		if( mMRSFrameBone ){
			SAFE_DELETE_ARRAY( mMRSFrameBone );
			SAFE_DELETE( mMRSAnimationFrame );
		}
		mMRSFrameBone = NEW FBX_FRAME_BONE[ frame2 - frame1 ];
		mMRSAnimationFrame = NEW FBX_ANIMETION_FRAME;
		mMRSAnimationFrame->mStartFrame = frame1;
		mMRSAnimationFrame->mEndFrame = frame2;
		//	�t���[������A�j���[�V�������Ԃ��擾
		for( int i = frame1; i <  frame2; i++ ){
			time.SetTime(0,0,0,i,0,FbxTime::eFrames30);
			
			//	�{�[���̐������[�v���s
			for(unsigned long j=0; j<mBoneCount; j++)
			{		
				D3DXMatrixIdentity( &mMRSFrameBone[ i ].mFramePose[ j ] );
				//	�p���s������o��
				FbxAMatrix mat=mCluster[ j ]->GetLink()->EvaluateGlobalTransform( time );

				//	DirectX�̍s��ϐ��ɕϊ������s
				for(int x=0;x<4;x++)
				{
					for(int y=0;y<4;y++)
					{
						mMRSFrameBone[ i ].mFramePose[ j ]( y, x ) = static_cast< float >( mat.Get( y, x ) );
					}
				}
			}
		}
	}

	void createAnimationFile( const char* fileName, int frame1, int frame2 ){
		ostringstream oss;
		oss.str( "" );
		oss << fileName << ".sin";
		
		//�܂��̓t�@�C�����쐬
		FILE* fp;
		fp = fopen( oss.str().c_str(), "wb" );

		//---------�{�[����------------//
		fwrite( &mBoneCount, sizeof( mBoneCount ), 1, fp );

		//---------BoneData----------//
		fwrite( mMRSBone, sizeof( FBX_BONE ), mBoneCount, fp );

		//---------�t���[���̎���-------------------------//
		fwrite( mMRSAnimationFrame, sizeof( FBX_ANIMETION_FRAME ), 1, fp );
		
		//---------�t���[���̃A�j���[�V�����f�[�^-------//
		frameMatrix( frame1, frame2 );
		fwrite( mMRSFrameBone, sizeof( FBX_FRAME_BONE ), frame2-frame1, fp );

		fclose( fp );
	}

	void createFile( const char* fileName ){
		ostringstream oss;
		oss.str( "" );
		oss << fileName << ".mrs";
		
		//�܂��̓t�@�C�����쐬
		FILE* fp;
		fp = fopen( oss.str().c_str(), "wb" );

		//---------���_��----------------//
		fwrite( &mVertexCount, sizeof( mVertexCount ), 1, fp );

		//---------�C���f�b�N�X��-------//
		fwrite( &mIndexCount, sizeof( mIndexCount ), 1, fp );

		//---------UV��-----------------//
		fwrite( &mUVCount, sizeof( mUVCount ), 1, fp );

		//---------�}�e���A����--------//
		mMaterialCount = mNumMaterial;
		fwrite( &mMaterialCount, sizeof( mMaterialCount ), 1, fp );

		//---------�{�[����------------//
		fwrite( &mBoneCount, sizeof( mBoneCount ), 1, fp );

		//---------VertexBuffer-------//
		int count = 0;
		( mVertexCount < mUVCount ) ? count = mUVCount : count = mVertexCount;
		fwrite( mMRSVertex, sizeof( FBX_SKINMESH_VERTEX ), count, fp );

		//---------IndexBuffer--------//
		fwrite( mMRSIndex, sizeof( FBX_INDEX ), mMaterialCount, fp );

		//---------Material-----------//
		fwrite( mMeshMaterial, sizeof( MeshMaterial ), mMaterialCount, fp );

		fclose( fp );
	}


	//Fbx�t�@�C���ǂݍ��݊֘A
	FbxManager* mManager;
	FbxImporter* mImport;
	FbxScene* mScene;

	//�o�b�t�@�֘A
	VertexBuffer mVertex;
	IndexBuffer* mIndex;

	//�}�e���A���֘A
	unsigned long mNumMaterial;

	//�A�j���[�V�����֘A
	int mAnimFrame;

	//�{�[���֘A
	FbxCluster** mCluster;
	D3DXMATRIX mBoneData[ 255 ];
};

}
}

#endif