#ifndef INCLUDE_MASHIRO_GAME_SHADER_BASE_H_
#define INCLUDE_MASHIRO_GAME_SHADER_BASE_H_

#include <d3d10_1.h>
#include <d3dx10.h>
#include "Mashiro/Graphics/Enum.h"

namespace Mashiro{

namespace Shader {

class ShaderBase {
public:
	ShaderBase();
	virtual ~ShaderBase();

	//�V�F�[�_�̃R���p�C�����{
	virtual void compile( ID3D10Device1* ) = 0;

	//�R���X�^���g�o�b�t�@�ւ̃}�b�s���O
	HRESULT lock( void** cb );

	//�R���X�^���g�o�b�t�@�ւ̃}�b�s���O�I���
	void unLock();

	//�V�F�[�_�Z�b�g
	void setShader( ID3D10Device1* device );

protected:
	//vertexShader�R���p�C��
	HRESULT compileVSFromFile( ID3D10Device1* device, const char* fileName, const char* funcName, D3D10_INPUT_ELEMENT_DESC layout[], UINT numElements );
	//pixelShader�R���p�C��
	HRESULT compilePSFromFile( ID3D10Device1* device, const char* fileName, const char* funcName );
	//GeometryShader�R���p�C��
	HRESULT compileGSFromFile( ID3D10Device1* device, const char* fileName, const char* funcName );
	//�R���X�^���g�o�b�t�@�쐬
	HRESULT createConstantBuffer( ID3D10Device1* device, UINT bufferSize );

private:
	ID3D10VertexShader* mVertexShader;
	ID3D10PixelShader* mPixelShader;
	ID3D10GeometryShader* mGeometryShader;
	ID3D10Buffer* mConstantBuffer;
	ID3D10InputLayout* mInputLayout;

};

}
}


#endif