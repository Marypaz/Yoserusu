#ifndef INCLUDE_GRAPHICS_SHADER_DESC_H_
#define INCLUDE_GRAPHICS_SHADER_DESC_H_

#include "Mashiro/Graphics/Enum.h"

namespace Mashiro{
namespace Graphics{

struct ElementDesc{
	const char* mElementName;
	RenderFormat mFromat;
	int mOffset;
};

struct ShaderFile{
	ShaderFile() : mFileName(), mVSFunc(), mPSFunc(), mGSFunc() {}
	const char* mFileName;
	const char* mVSFunc;
	const char* mPSFunc;
	const char* mGSFunc;
};

}
}

#endif
