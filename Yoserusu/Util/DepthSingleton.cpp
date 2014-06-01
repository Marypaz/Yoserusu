#include "Mashiro/Mashiro.h"
#include "DepthSingleton.h"

DepthSingleton* DepthSingleton::mInstance = 0;

DepthSingleton::DepthSingleton() : mDepthMax( 0.f ), mDepthMin( 0.f ) {
}

DepthSingleton::~DepthSingleton(){
}

DepthSingleton* DepthSingleton::instance() {
	return mInstance;
}

void DepthSingleton::create() {
	ASSERT( !mInstance );
	mInstance = NEW DepthSingleton();
}

void DepthSingleton::destory() {
	ASSERT( mInstance );
	SAFE_DELETE( mInstance );
}

int DepthSingleton::getDepthMax() const {
	return mDepthMax;
}

int DepthSingleton::getDepthMin() const {
	return mDepthMin;
}

void DepthSingleton::setDepthMax( float i ) {
	mDepthMax = i;
}

void DepthSingleton::setDepthMin( float i ){
	mDepthMin = i;
}