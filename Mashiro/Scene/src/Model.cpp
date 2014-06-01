#include "Mashiro/Mashiro.h"
#include "Mashiro/Scene/Model.h"
#include "Mashiro/Graphics/RenderTarget.h"
#include "Mashiro/Graphics/Texture.h"
#include "Mashiro/Graphics/GraphicsManager.h"
#include "Mashiro/Graphics/src/GraphicsManagerImpl.h"
#include "Mashiro/Graphics/Enum.h"
#include "Mashiro/Math/Vector3.h"
#include "Mashiro/Math/Functions.h"
#include "Mashiro/Base/Impl/ReferenceType.h"
#include "Mashiro/Math/Matrix.h"

#include "ModelImpl.h"

namespace Mashiro{

using namespace Mashiro::Graphics;

namespace Scene {

Model::Model( Impl* impl ) : mImpl( impl ){
	if( mImpl ){
		mImpl->refer();
	}
}

void Model::draw() const {
	ASSERT( mImpl && "Graphics::Model : This is empty Object" );
	mImpl->draw();
}

void Model::setAngle( const Vector3& angle ){
	ASSERT( mImpl && "Graphics::Model : This is empty Object" );
	mImpl->setAngle( angle );
}

void Model::setColor( const Vector3& color ){
	ASSERT( mImpl && "Graphics::Model : This is empty Object" );
	mImpl->setColor( color );
}

void Model::setScale( const Vector3& scale ){
	ASSERT( mImpl && "Graphics::Model : This is empty Object" );
	mImpl->setScale( scale );
}

void Model::setPosition( const Vector3& position ){
	ASSERT( mImpl && "Graphics::Model : This is empty Object" );
	mImpl->setPosition( position );
}

void Model::setPosition( const Vector2& pos ){
	ASSERT( mImpl && "Graphics::Model : This is empty Object" );
	//mImpl->setPosition( pos, gManagerImpl->mShaderParameter.mProjection, gManagerImpl->mShaderParameter.mView );
}

void Model::setTexture( const Texture& tex, int num ){
	ASSERT( mImpl && "Graphics::Model : This is empty Object" );
	mImpl->setTexture( tex, num );
}

void Model::setTexture(const RenderTarget& targetTexture, int num /*= 0 */) {
	ASSERT( mImpl && "Graphics::Model : This is empty Object" );
	mImpl->setTexture( targetTexture, num );
}

void Model::setTransparaency( float c ){
	ASSERT( mImpl && "Graphics::Model : This is empty Object" );
	mImpl->setTransparaency( c );
}

void Model::draw( const Matrix& world ) const {
	ASSERT( mImpl && "Graphics::Model : This is empty Object" );
	mImpl->draw( world );
}

const Vector3* Model::angle() const {
	ASSERT( mImpl && "Graphics::Model : This is empty Object" );
	return &mImpl->mAngle;
}

const Vector3* Model::scale() const {
	ASSERT( mImpl && "Graphics::Model : This is empty Object" );
	return &mImpl->mScale;
}

const Vector3* Model::color() const {
	ASSERT( mImpl && "Graphics::Model : This is empty Object" );
	return &mImpl->mColor;
}

float Model::transparaency() const {
	ASSERT( mImpl && "Graphics::Model : This is empty Object" );
	return mImpl->mTrans;
}

const Vector3* Model::position() const {
	ASSERT( mImpl && "Graphics::Model : This is empty Object" );
	return &mImpl->mPosition;
}

Matrix Model::worldMatrix() const {
	ASSERT( mImpl && "Graphics::Model : This is empty Object" );
	return  mImpl->worldMatrix();
}


#define TYPE Model
#include "Mashiro/Base/Impl/ReferenceTypeTemplate.h"

}
}