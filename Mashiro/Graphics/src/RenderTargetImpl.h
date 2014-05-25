#ifndef INCLUDE_MASHIRO_GRAPHICS_RENDERTARGET_IMPL_H_
#define INCLUDE_MASHIRO_GRAPHICS_RENDERTARGET_IMPL_H_

#include <d3d10_1.h>
#include <d3dx10.h>
#include "Mashiro/Mashiro.h"
#include "Mashiro/Base/Impl/ReferenceType.h"
#include "Mashiro/Graphics/RenderTarget.h"
#include "Mashiro/Graphics/src/Release.h"

namespace Mashiro{
namespace Graphics{

class RenderTarget::Impl : public Mashiro::ReferenceType {
public:
	Impl( int w, int h, RenderFormat textureFormat, ID3D10Device1* device ) : 
	mWidth( w ),
	mHeight( h ),
	mTextureFormat( textureFormat ),
	mTexture( 0 ),
	mSrv( 0 ),
	mRtv( 0 ){
		createRenderTarget( device );
	}
	~Impl(){
		SafeRelease( &mTexture );
		SafeRelease( &mRtv );
		SafeRelease( &mSrv );
	}
	//�����_�����O�^�[�Q�b�g�̍쐬
	void createRenderTarget( ID3D10Device1* device ) {
		HRESULT hr = S_OK;
		D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		D3D10_RENDER_TARGET_VIEW_DESC RTVDesc;
		D3D10_TEXTURE2D_DESC desc;
		ZeroMemory( &desc, sizeof( desc ) );
		ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );
		ZeroMemory( &RTVDesc, sizeof( RTVDesc ) );

		//	�����_�����O�^�[�Q�b�g�p�̃e�N�X�`���𐶐�	
		desc.Width = mWidth;						//	�e�N�X�`�������T�C�Y
		desc.Height = mHeight;						//	�e�N�X�`���c���T�C�Y
		desc.MipLevels = 1;						//	�~�b�v�}�b�v���x��
		desc.ArraySize = 1;						//	�e�N�X�`���z��
		desc.Format = (DXGI_FORMAT)mTextureFormat;	//	�e�N�X�`���t�H�[�}�b�g(�t���[�e�B���O�o�b�t�@)
		desc.SampleDesc.Count = 1;				//	�}���`�T���v�����O��
		desc.SampleDesc.Quality = 0;			//	�T���v�����O�i��
		desc.Usage = D3D10_USAGE_DEFAULT;		//	�A�N�Z�X���@
		desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;		//	�o�C���h
		desc.CPUAccessFlags = 0;				//	CPU�A�N�Z�X����
		desc.MiscFlags = 0;						//	�I�v�V����

		//	�����_�����O�ΏۂƂȂ�e�N�X�`���̐���
		if( FAILED( hr = device->CreateTexture2D( &desc, NULL, &mTexture )))
		{
			hr = E_FAIL;
			STRONG_ASSERT( hr != E_FAIL && "�e�N�X�`���[�^�[�Q�b�g�̍쐬���s" );
		}

		//	�����_�����O�^�[�Q�b�g�r���[�̍쐬
		RTVDesc.Format = desc.Format;
		RTVDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
		RTVDesc.Texture2D.MipSlice = 0;
		if( FAILED( hr = device->CreateRenderTargetView( mTexture, &RTVDesc, &mRtv )))
		{
			hr = E_FAIL;
			STRONG_ASSERT( hr != E_FAIL && "�����_�����O�^�[�Q�b�g�̍쐬���s" );
		}

		//	�V�F�[�_�[���\�[�X�r���[�̍쐬
		ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );
		SRVDesc.Format =desc.Format;
		SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		if( FAILED(device->CreateShaderResourceView(mTexture,&SRVDesc,&mSrv)))
		{
			hr = E_FAIL;
			STRONG_ASSERT( hr != E_FAIL && "�V�F�[�_���\�[�X�r���[�̍쐬���s" );
		}
	}
	void targetTexture( const char* name ){
		D3DX10SaveTextureToFile( mTexture, D3DX10_IFF_PNG, name );
	}

	int mWidth;
	int mHeight;
	RenderFormat mTextureFormat;
	RenderFormat mDepthForamt;

	ID3D10Texture2D* mTexture;	//�e�N�X�`��
	ID3D10RenderTargetView*	mRtv; //�����_�����O�^�[�Q�b�g�r���[
	ID3D10ShaderResourceView* mSrv;	//�V�F�[�_�[���\�[�X�r���[
};

}
}

#endif