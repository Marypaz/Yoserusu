#ifndef INCLUDE_SHADER_BASE_H_
#define INCLUDE_SHADER_BASE_H_

#include "Mashiro/Graphics/Shader.h"

template< class U >
class ShaderBase {
public:
	static U* instance(){
		static U mInstance;
		return &mInstance;
	}

	virtual bool lock( void** cb ){
		if( mShader.lock( cb ) ){
			return true;
		}
		return false;
	}

	virtual void unLock(){
		mShader.unlock();
	}
	virtual Mashiro::Graphics::Shader shader(){
		return mShader;
	}
protected:
	virtual void create() = 0;

	ShaderBase(){}
	virtual ~ShaderBase(){}

	Mashiro::Graphics::Shader mShader;
};

#endif