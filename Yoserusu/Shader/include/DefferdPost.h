#ifndef INCLUDE_MASHIRO_GRAPHICS_SHADER_DEFFERDPOST_H_
#define INCLUDE_MASHIRO_GRAPHICS_SHADER_DEFFERDPOST_H_

#include "Mashiro/Shader/src/ShaderBase.h"

#include "Mashiro/Graphics/Vertex.h"

using namespace Mashiro::Shader;

namespace Mashiro {

namespace Graphics {

//CocTransシェーダ
class DefferdPost : public ShaderBase {
public:
	DefferdPost(ID3D10Device1* dev ){
		compile( dev );
	}
	~DefferdPost(){
	}
	//コンパイル
	void compile( ID3D10Device1* device ) {
		//頂点Desc
		D3D10_INPUT_ELEMENT_DESC desc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 28, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 44, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};

		//宣言
		HRESULT hr = S_OK;
		unsigned int size = sizeof( desc ) / sizeof( desc[ 0 ] );

		//頂点シェーダコンパイル
		if( FAILED( hr = compileVSFromFile( device, "hlsl/DeferredPost.fx", "DeferredPostVS", desc, size ) ) ){
			hr = E_FAIL;
			STRONG_ASSERT( hr != E_FAIL && "Vertexシェーダのコンパイル失敗" );
		}

		//ピクセルシェーダコンパイル
		if( FAILED( hr = compilePSFromFile( device, "hlsl/DeferredPost.fx", "DeferredPostPS" ) ) ){
			hr = E_FAIL;
			STRONG_ASSERT( hr != E_FAIL && "Pixelシェーダのコンパイル失敗" );
		}

		//コンスタントバッファ作成
		if( FAILED( createConstantBuffer( device, sizeof( DefferdPostConstant ) ) ) ){
			hr = E_FAIL;
			STRONG_ASSERT( hr != E_FAIL && "ConstantBufferのコンパイル失敗" );
		}
	}
};

}
}

#endif