#include "Mashiro/Mashiro.h"
#include "Mashiro/Graphics/Bitmap.h"
#include "Mashiro/Graphics/src/BitmapImpl.h"
#include "Mashiro/Graphics/src/SpriteManagerImpl.h"



namespace Mashiro{
namespace Graphics {

Bitmap Bitmap::create( const char* fileName ){
	Bitmap r;
	r.mImpl = NEW Impl( fileName, gSpriteImpl->backBuffer(), gSpriteImpl->image() );
	return r;
}

Bitmap Bitmap::create( int w, int h, long s ){
	Bitmap r;
	r.mImpl = NEW Impl( w, h, s, gSpriteImpl->backBuffer() );
	return r;
}

void Bitmap::copyFromMemory( unsigned char* data ){
	mImpl->copyFromMemory( data );
}

Bitmap::Bitmap( const Impl* ) : mImpl( 0 ){
}

int Bitmap::height() const {
	Bitmap r;
	return static_cast< int >( r.mImpl->mImageSize.y );
}

int Bitmap::width() const {
	Bitmap r;
	return static_cast< int >( r.mImpl->mImageSize.x );
}

bool Bitmap::operator<( const Bitmap& o ) const {
	return ( mImpl < o.mImpl );
}

#define TYPE Bitmap
#include "Mashiro/Base/Impl/ReferenceTypeTemplate.h"

}
}
