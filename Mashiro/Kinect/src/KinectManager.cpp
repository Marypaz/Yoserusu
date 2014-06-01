#include "Mashiro/Mashiro.h"
#include "Mashiro/Kinect/KinectManager.h"
#include "Mashiro/Graphics/Bitmap.h"
#include "Mashiro/Math/Vector2.h"

#include "Mashiro/Kinect/src/KinectManagerImpl.h"

#include "Mashiro/Window/Window.h"

namespace Mashiro {

namespace Kinect {

KinectManagerImpl* gManagerImpl;

Manager::Manager() {
	//•Ê‚ÌƒXƒŒƒbƒh‚©‚ç‚ÍNG
	ASSERT( Window::isMainThread() && "you must call from Main thread" );
}

Manager Manager::instance(){
	return Manager();
}

void Manager::create( int w, int h, bool window ){
	STRONG_ASSERT( !gManagerImpl );
	gManagerImpl = NEW KinectManagerImpl( w, h );
}

void Manager::update(){
	gManagerImpl->update();
}

Graphics::Bitmap Manager::colorTexture() const {
	return gManagerImpl->colorTexture();
}

Graphics::Bitmap Manager::depthTexture() const {
	return gManagerImpl->depthTexture();
}

Math::Vector2 Manager::skeletonPos( int i ) const {
	return gManagerImpl->skeletonPos( i );
}

int Manager::depthSkeleton( int i ) const {
	return gManagerImpl->depthSkeleton( i );
}

void Manager::destroy() {
	SAFE_DELETE( gManagerImpl );
}

bool Manager::tracking() const {
	return gManagerImpl->tracking();
}

void Manager::initTrackingFlag(){
	gManagerImpl->initTrackingFlag();
}

}
}