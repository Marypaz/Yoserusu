#include "Mashiro/Mashiro.h"
#include "Mashiro/Graphics/Shader.h"
#include "Mashiro/Graphics/src/ShaderImpl.h"
#include "Mashiro/Graphics/src/GraphicsManagerImpl.h"

namespace Mashiro{
namespace Graphics{

Shader Shader::create( ShaderFile shaderFile, ElementDesc desc[], unsigned int size, size_t constantBufferSize ){
	Shader r;
	r.mImpl = NEW Impl( shaderFile, desc, size, constantBufferSize, gManagerImpl->dxDevice() );
	return r;
}

bool Shader::lock( void** buffer ){
	ASSERT( mImpl && "Graphics::IndexBuffer : This is empty object." );
	return mImpl->lock( buffer );
}

void Shader::unlock(){
	ASSERT( mImpl && "Graphics::IndexBuffer : This is empty object." );
	mImpl->unLock();
}

#define TYPE Shader
#include "Mashiro/Base/Impl/ReferenceTypeTemplate.h"

}
}