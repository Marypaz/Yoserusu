#include "Mashiro/Shader/src/ShaderBase.h"
#include "Mashiro/Graphics/src/Release.h"
#include <fstream>
using namespace std;
using namespace Mashiro::Graphics;
namespace Mashiro{

namespace Shader {

ShaderBase::ShaderBase() :
mConstantBuffer( 0 ),
mVertexShader( 0 ),
mGeometryShader( 0 ),
mPixelShader( 0 ),
mInputLayout( 0 ){
}

ShaderBase::~ShaderBase(){
	SafeRelease( &mConstantBuffer );
	SafeRelease( &mVertexShader );
	SafeRelease( &mPixelShader );
	SafeRelease( &mGeometryShader );
	SafeRelease( &mInputLayout );
}

HRESULT ShaderBase::compileVSFromFile( ID3D10Device1* device, const char* fileName, const char* funcName, D3D10_INPUT_ELEMENT_DESC layout[],UINT numElements ){
	ID3D10Blob* shader=NULL;
	ID3D10Blob* errors=NULL;
	HRESULT hr = S_OK;

	//�V�F�[�_�[�t�@�C���̃R���p�C��
	if( FAILED( hr = D3DX10CompileFromFile( fileName, NULL, NULL, funcName, "vs_4_0", 0, 0, NULL, &shader, &errors, NULL ) ) ){
		char* c = (char*)errors->GetBufferPointer();
		//�G���[���O�o���܂�
		ofstream file;
		file.open( "errorVertexLog.txt" );
		file << c << endl;
		file.close();
		return E_FAIL;
	}
	SafeRelease(&errors);

	//VertexShader�𐶐�
	if( FAILED( device->CreateVertexShader( shader->GetBufferPointer(), shader->GetBufferSize(), &mVertexShader ) ) ){
		SafeRelease(&shader);
		return E_FAIL;
	}

	//���_�C���v�b�g���C�A�E�g���쐬
	if( FAILED( hr = device->CreateInputLayout( layout, numElements, shader->GetBufferPointer(), shader->GetBufferSize(), &mInputLayout ) ) ){
		return E_FAIL;
	}
	SafeRelease( &shader );
	return S_OK;
}

HRESULT ShaderBase::compilePSFromFile( ID3D10Device1* device, const char* fileName, const char* funcName ){
	ID3D10Blob* shader=NULL;
	ID3D10Blob* errors=NULL;

	//�V�F�[�_�[�t�@�C���̃R���p�C��
	if( FAILED( D3DX10CompileFromFile( fileName, NULL, NULL, funcName, "ps_4_0", 0, 0, NULL, &shader, &errors, NULL ) ) ){
		char* c = (char*)errors->GetBufferPointer();
		//�G���[���O�o���܂�
		ofstream file;
		file.open( "errorPixelLog.txt" );
		file << c << endl;
		file.close();
		return E_FAIL;
	}
	SafeRelease(&errors);

	//PixelShader�𐶐�
	if( FAILED( device->CreatePixelShader( shader->GetBufferPointer(), shader->GetBufferSize(), &mPixelShader ) ) ){
		SafeRelease(&shader);
		return E_FAIL;
	}

	SafeRelease( &shader );
	return S_OK;
}

HRESULT ShaderBase::compileGSFromFile( ID3D10Device1* device, const char* fileName, const char* funcName ){
	ID3D10Blob* shader=NULL;
	ID3D10Blob* errors=NULL;

	//�V�F�[�_�[�t�@�C���̃R���p�C��
	if( FAILED( D3DX10CompileFromFile( fileName, NULL, NULL, funcName, "gs_4_0", 0, 0, NULL, &shader, &errors, NULL ) ) ){
		char* c = (char*)errors->GetBufferPointer();
		//�G���[���O�o���܂�
		ofstream file;
		file.open( "errorGeometryLog.txt" );
		file << c << endl;
		file.close();
		return E_FAIL;
	}
	SafeRelease(&errors);

	//GeometryShader�𐶐�
	if( FAILED( device->CreateGeometryShader( shader->GetBufferPointer(), shader->GetBufferSize(), &mGeometryShader ) ) ){
		SafeRelease(&shader);
		return E_FAIL;
	}

	SafeRelease( &shader );
	return S_OK;
}

HRESULT ShaderBase::createConstantBuffer( ID3D10Device1* device, UINT bufferSize ){
	D3D10_BUFFER_DESC cb;
	cb.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = bufferSize;
	cb.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D10_USAGE_DYNAMIC;

	HRESULT hr = E_FAIL;

	//�R���X�^���g�o�b�t�@���쐬
	if( FAILED( hr = device->CreateBuffer( &cb, NULL, &mConstantBuffer ) ) ){
		return E_FAIL;
	}
	return S_OK;
}

void ShaderBase::setShader( ID3D10Device1* device ){
	//�e�V�F�[�_���Z�b�g
	if( NULL != mVertexShader ){
		device->VSSetShader( mVertexShader );
	}
	if( NULL != mGeometryShader ){
		device->GSSetShader( mGeometryShader );
	}
	if( NULL != mPixelShader ){
		device->PSSetShader( mPixelShader );
	}

	//�R���X�^���g�o�b�t�@���Z�b�g
	if( NULL != mConstantBuffer ){
		device->VSSetConstantBuffers( 1, 1, &mConstantBuffer );
	}
	if( NULL != mConstantBuffer ){
		device->GSSetConstantBuffers( 1, 1, &mConstantBuffer );
	}
	if( NULL != mConstantBuffer ){
		device->PSSetConstantBuffers( 1, 1, &mConstantBuffer );
	}

	//���_���C�A�E�g�Z�b�g
	if( NULL != mInputLayout ){
		device->IASetInputLayout( mInputLayout );
	}
}

HRESULT ShaderBase::lock( void** cb ){
	HRESULT hr = S_OK;
	if( FAILED( hr = mConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, cb ) ) ){
		return E_FAIL;
	}
	return S_OK;
}

void ShaderBase::unLock(){
	mConstantBuffer->Unmap();
}

}
}
