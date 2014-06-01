#include "Mashiro/Mashiro.h"
#include "Mashiro/WorkSpace/WorkSpace.h"
#include "Mashiro/Graphics/GraphicsManager.h"
#include "Mashiro/Scene/PMDFileLoader.h"
#include "Mashiro/Kinect/KinectManager.h"
#include "Game/ArmRight.h"
#include "Util/ModelLoader.h"
#include "Shader/include/CocTrans.h"
using namespace Mashiro::Graphics;
using namespace Mashiro::Scene;

ArmRight::ArmRight(){
	wrist.model = ModelLoader::instance()->createModel("res/model/RightHand.pmd");
	shoulder.model = ModelLoader::instance()->createModel("res/model/RightShoulder.pmd");
	elbow.model = ModelLoader::instance()->createModel("res/model/RightElbow.pmd");

}

ArmRight::~ArmRight(){
}

void ArmRight::Update(float depth,int number){
	Kinect::Manager kinect = Kinect::Manager::instance();
	Vector2 pos1(kinect.skeletonPos(Kinect::SKELETON_INDEX_HAND_RIGHT));
	Vector2 pos2(kinect.skeletonPos(Kinect::SKELETON_INDEX_ELBOW_RIGHT));

	if(kinect.depthSkeleton(number) < 250){
		this->depth = kinect.depthSkeleton(number);
	}

	if( pos1.x >= 800){
		keep = false;
	}

	//	pos1,pos2にz軸を追加
	Vector3 pos1super = Vector3(pos1.x/10-50,-pos1.y/10+15,depth*2);
	Vector3 pos2super = Vector3(15,-15,0);

	//	pos1,pos2の2点間の距離を求める
	Vector3 dir=Vector3(pos1super.x - pos2super.x,pos1super.y - pos2super.y,pos1super.z - pos2super.z );

	//	atan2を使いx軸、y軸の角度を求める
	float zxLength = sqrt(dir.x * dir.x + dir.z * dir.z);
	float tx = atan2(dir.y, zxLength);

	float yxLength = sqrt(dir.y * dir.y + dir.x * dir.x);
	float ty = atan2(dir.x,dir.z);
	tx=tx/3.14159265f*180.f;
	ty=ty/3.14159265f*180.f;


	Matrix scale;
	scale.setScaling(Vector3(1.0f));
	Matrix rotateZ;
	rotateZ.setRotationZ(180);
	Matrix rotateX;
	rotateX.setRotationX(270-tx);
	Matrix rotateY;
	rotateY.setRotationY(ty);
	Matrix shouldertrans;
	shouldertrans.setTranslation(Vector3(pos2super));
	Matrix elbowertrans;
	elbowertrans.setTranslation(Vector3((pos1super.x+pos2super.x) /2,(pos1super.y+pos2super.y)/2+tx/5,(pos1super.z + pos2super.z) / 2));
	Matrix handtrans;
	handtrans.setTranslation(Vector3(pos1super));

	sworld.setIdentity();
	sworld.setMul(sworld,scale);
	sworld.setMul(sworld,rotateZ);
	sworld.setMul(sworld,rotateX);
	sworld.setMul(sworld,rotateY);
	sworld.setMul(sworld, shouldertrans);

	scale.setScaling(Vector3(1.0f,2.0f,1.0f));
	eworld.setIdentity();
	eworld.setMul(eworld,scale);
	eworld.setMul(eworld,rotateZ);
	eworld.setMul(eworld,rotateX);
	eworld.setMul(eworld,rotateY);	
	eworld.setMul(eworld, elbowertrans);


	scale.setScaling(Vector3(1.0f));
	rotateY.setRotationY(135);
	rotateX.setRotationX(270);
	hworld.setIdentity();
	hworld.setMul(hworld,scale);
	hworld.setMul(hworld,rotateZ);
	hworld.setMul(hworld,rotateX);
	hworld.setMul(hworld,rotateY);
	hworld.setMul(hworld, handtrans);
	
}

Vector2 ArmRight::Pos(){
	return Kinect::Manager::instance().skeletonPos(Kinect::SKELETON_INDEX_HAND_RIGHT);
}

void ArmRight::Draw()
{
	Graphics::Manager m = Graphics::Manager::instance();

	CocTrans* coc = CocTrans::instance();
	CocTrans::ConstantBuffer* cb = NULL;
	if( coc->lock( (void**)&cb ) ){
		cb->mDrawType = CocTrans::TYPE_ARM;
		coc->unLock();
	}
	cb = nullptr;
	m.setShader( CocTrans::instance()->shader() );

	shoulder.model.setColor( Vector3(  1.f, 0.f, 0.f ) );
	wrist.model.setColor( Vector3( 1.f, 0.f, 0.f ) );
	elbow.model.setColor( Vector3( 1.f, 0.f, 0.f ) );

	Vector4 light = coc->instance()->worldLight( sworld );	
	Graphics::Manager().setLight( light );
	shoulder.model.draw(sworld);
	light = coc->instance()->worldLight( sworld );	
	Graphics::Manager().setLight( light );
	wrist.model.draw(hworld);
	light = coc->instance()->worldLight( sworld );	
	Graphics::Manager().setLight( light );
	elbow.model.draw(eworld);


}