#ifndef INCLUDE_MASHIRO_GRAPHICS_FBX_FILELOADERIMPL_H_
#define INCLUDE_MASHIRO_GRAPHICS_FBX_FILELOADERIMPL_H_

#define FBXSDK_new_API

#include <fbxsdk.h>
#include "Mashiro/Mashiro.h"
#include "Mashiro/Base/Impl/ReferenceType.h"
#include "Mashiro/Graphics/IndexBuffer.h"
#include "Mashiro/Graphics/VertexBuffer.h"
#include "Mashiro/Graphics/Texture.h"
#include "Mashiro/Scene/FBXFileLoader.h"
#include "Mashiro/Graphics/Vertex.h"
#include "Mashiro/Math/Vector4.h"
#include "Mashiro/Math/Vector2.h"
using namespace std;
using namespace Mashiro::Math;
using namespace Mashiro::Graphics;

namespace Mashiro{

namespace Scene {

class FBXFileLoader::Impl : public Mashiro::ReferenceType {
public:
	//���_���
	struct FBX_VERTEX{
		FBX_VERTEX() : mPosition( 0.f ), mNormal( 0.f ), mUv( 0.f ){ }
		Vector3 mPosition;
		Vector3 mNormal;
		Vector2 mUv;
		Vector4 mColor;
	};
	//�}�e���A�����
	struct MeshMaterial{
		MeshMaterial() : mAmbient( 0.f ), mDiffuse( 0.f ), mSpecular( 0.f ){ }
		Vector4 mAmbient;
		Vector4 mDiffuse;
		Vector4 mSpecular;
		Graphics::Texture mTexture;
		const char* mFileName;
		unsigned long mNumFace; //�}�e���A�����g�p���Ă���|���S����
	};
public:
	Impl(){
	}
	~Impl(){
		if( mMaterial ){
			SAFE_DELETE_ARRAY( mMaterial );
		}
		if( mIndex ){
			SAFE_DELETE_ARRAY( mIndex );
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
		mNumVert = mesh->GetControlPointsCount();
		mNumUv = mesh->GetTextureUVCount();
		mNumFace = mesh->GetPolygonCount();

		//�ꎞ�I�ȃ������m��
		FBX_VERTEX* vb = NULL;
		( mNumVert < mNumUv ) ? vb = NEW FBX_VERTEX[ mNumUv ] : vb = NEW FBX_VERTEX[ mNumVert ];
		int* faceBuffer = NEW int[ mNumFace*3 ];

		//���_�ǂݍ���
		loadVertex( mesh, vb );

		//�|���R�����
		loadIndex( mesh, faceBuffer );

		//�@���ǂݍ���
		loadNormal( mesh, faceBuffer, vb );

		//�J���[�̓ǂݍ���
		loadColor( mesh, faceBuffer, vb );

		//�e�N�X�`�����W�ǂݍ���
		loadUv( mesh, vb );

		//�}�e���A���̓ǂݍ���
		loadMaterial( mesh );
		
		//���_�o�b�t�@�𐶐�

		if( mNumVert < mNumUv ) {
			mNumVert = mNumUv;
		}
		mVertex = VertexBuffer::create( mNumVert, sizeof( Vertex ) );
		Vertex* v = NULL;
		if( mVertex.lock((void**)&v)){
			for( unsigned long i = 0; i < mNumVert; i++ ){
				v[ i ].mPosition = vb[ i ].mPosition;
				v[ i ].mNormal = vb[ i ].mNormal;
				v[ i ].mUV = vb[ i ].mUv;
				v[ i ].mColor = vb[ i ].mColor;
			}
			mVertex.unlock();
		}

		SAFE_DELETE_ARRAY( faceBuffer );
		SAFE_DELETE_ARRAY( vb );
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
		memcpy( index, iIndex, sizeof(int)*mNumFace*3 );
	}
	//���_�̓ǂݍ���
	void loadVertex( FbxMesh* mesh, FBX_VERTEX* vb ){
		FbxVector4* Coord=mesh->GetControlPoints();

		int* piIndex=mesh->GetPolygonVertices();//	���_�C���f�b�N�X�ǂݍ���

		//	���_���ł͂Ȃ��A�ʂ��璸�_�f�[�^���擾����
		for(unsigned long i = 0; i < mNumFace; i++)
		{		
			//	�|���S�����\������C���f�b�N�X���擾
			int startIndex=mesh->GetPolygonVertexIndex(i);
		
			//	���_�ԍ����擾
			int idx0=piIndex[ startIndex ];		
			int idx1=piIndex[ startIndex+1 ];
			int idx2=piIndex[ startIndex+2 ];

			//	UV���̂ق��������ꍇ��UV�C���f�b�N�X����ɂ���
			if(mNumVert<mNumUv)	
			{
				idx0 = mesh->GetTextureUVIndex(i,0,FbxLayerElement::eTextureDiffuse);
				idx1 = mesh->GetTextureUVIndex(i,1,FbxLayerElement::eTextureDiffuse);
				idx2 = mesh->GetTextureUVIndex(i,2,FbxLayerElement::eTextureDiffuse);
			}

			//	���_�f�[�^0�̎擾
			int index=mesh->GetPolygonVertex(i,0);
			vb[idx0].mPosition.x =  static_cast<float>(Coord[ index ].mData[0]);
			vb[idx0].mPosition.y =  static_cast<float>(Coord[ index ].mData[1]);
			vb[idx0].mPosition.z =  static_cast<float>(Coord[ index ].mData[2]);

			//	���_�f�[�^1�̎擾
			index=mesh->GetPolygonVertex(i,1);
			vb[idx1].mPosition.x =  static_cast<float>(Coord[ index ].mData[0]);
			vb[idx1].mPosition.y =  static_cast<float>(Coord[ index ].mData[1]);
			vb[idx1].mPosition.z =  static_cast<float>(Coord[ index ].mData[2]);

			//	���_�f�[�^2�̎擾
			index=mesh->GetPolygonVertex(i,2);
			vb[idx2].mPosition.x =  static_cast<float>(Coord[ index ].mData[0]);
			vb[idx2].mPosition.y =  static_cast<float>(Coord[ index ].mData[1]);
			vb[idx2].mPosition.z =  static_cast<float>(Coord[ index ].mData[2]);

		}
	}
	//�@���̓ǂݍ���
	void loadNormal( FbxMesh* mesh, const int* index, FBX_VERTEX* vb ){
		FbxVector4 normal;

		for(unsigned long i = 0; i < mNumFace; i++)
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
			if(mNumVert<mNumUv)	
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
	void loadUv( FbxMesh* mesh, FBX_VERTEX* vb ){
		const FbxLayerElementUV* pUV=0;

		//	FBX�e�N�X�`�����W�̐擪�|�C���^���擾
		pUV = mesh->GetLayer(0)->GetUVs();

		//	UV�̐������[�v
		for(unsigned long i = 0; i < mNumUv; i++)
		{
			FbxVector2 v2;

			//	�e�N�X�`�����W�擾
			v2=pUV->GetDirectArray().GetAt(i);

			//	�e�N�X�`�����W�𒸓_�z��ɃR�s�[
			vb[i].mUv.x = static_cast<float>(v2.mData[0]);
			vb[i].mUv.y = 1.0f - static_cast<float>(v2.mData[1]);
		}
	}
	//�J���[�̓ǂݍ���
	void loadColor( FbxMesh* mesh, const int* index, FBX_VERTEX* vb ){
		const FbxLayerElementVertexColor* color=0;

		//	FBX�e�N�X�`�����W�̐擪�|�C���^���擾
		color = mesh->GetLayer(0)->GetVertexColors();

		//	UV�̐������[�v
		if( color ){
			for(unsigned long i = 0; i < mNumUv; i++)
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
			for(unsigned long i = 0; i < mNumUv; i++){
				vb[i].mColor.set( 1.f, 1.f, 1.f, 1.f );
			}
		}
	}
	//�}�e���A���̓ǂݍ���
	void loadMaterial( FbxMesh* mesh ){
		FbxNode* pNode	= mesh->GetNode();
		mNumMaterial	= pNode->GetMaterialCount();
		mMaterial = NEW MeshMaterial[mNumMaterial];

		//	�}�e���A���̐������C���f�b�N�X�o�b�t�@�[���쐬
		mIndex = NEW IndexBuffer[mNumMaterial];

		for(unsigned long i=0;i<mNumMaterial;i++)
		{
			//	�t�H�����f����z��
			FbxSurfaceMaterial* pMaterial=pNode->GetMaterial(i);
			FbxSurfacePhong* pPhong=(FbxSurfacePhong*)pMaterial;


			//	�A���r�G���g�J���[(����)

			FbxDouble3 d3Ambient=pPhong->Ambient;
			mMaterial[i].mAmbient.x = static_cast<float>(d3Ambient.mData[0]);
			mMaterial[i].mAmbient.y = static_cast<float>(d3Ambient.mData[1]);
			mMaterial[i].mAmbient.z = static_cast<float>(d3Ambient.mData[2]);
			mMaterial[i].mAmbient.w = 1.0f;

			//	�f�B�t���[�Y�J���[(�g�U���ˌ�)
			FbxDouble3 d3Diffuse=pPhong->Diffuse;
			mMaterial[i].mDiffuse.x = static_cast<float>(d3Diffuse.mData[0]);
			mMaterial[i].mDiffuse.y = static_cast<float>(d3Diffuse.mData[1]);
			mMaterial[i].mDiffuse.z = static_cast<float>(d3Diffuse.mData[2]);
			mMaterial[i].mDiffuse.w = 1.0f;
			
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
				mMaterial[ i ].mFileName = texture->GetName();
				mMaterial[i].mTexture = Graphics::Texture::create( "kazan3.png" );
			}	

			//�}�e���A���̐������C���f�b�N�X�o�b�t�@�[���쐬
			int iCount=0;
			unsigned long* pIndex=NEW unsigned long[mNumFace*3];//�Ƃ肠�����A���b�V�����̃|���S�����Ń������m��
	
			//���̃}�e���A���ł���C���f�b�N�X�z����̊J�n�C���f�b�N�X�𒲂ׂ�@����ɃC���f�b�N�X�̌������ׂ�		
			iCount=0;
			for(unsigned long k=0;k<mNumFace;k++)
			{
				const FbxLayerElementMaterial* mat = mesh->GetLayer(0)->GetMaterials();
				int matId=mat->GetIndexArray().GetAt(k);

				if(matId==i)
				{
					//	���_�����UV�������ꍇ�AUV�C���f�b�N�X���g�p
					if(mNumVert<mNumUv) 
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
			unsigned long* indices = mIndex[ i ].lock();
			for( int x = 0; x < iCount; x++ ){
				indices[ x ] = pIndex[ x ];
			}
			mIndex[ i ].unlock( &indices );

			mMaterial[ i ].mNumFace = iCount / 3;

			SAFE_DELETE_ARRAY( pIndex );
		}
	}
	//Fbx�t�@�C���ǂݍ��݊֘A
	FbxManager* mManager;
	FbxImporter* mImport;
	FbxScene* mScene;

	//���b�V���֘A
	unsigned long mNumVert;
	unsigned long mNumFace;
	unsigned long mNumUv;

	//�o�b�t�@�֘A
	VertexBuffer mVertex;
	IndexBuffer* mIndex;

	//�}�e���A���֘A
	MeshMaterial* mMaterial;
	unsigned long mNumMaterial;
};

}
}

#endif