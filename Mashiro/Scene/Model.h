#ifndef INCLUDE_MASHIRO_GRAPHICS_MODEL_H_
#define INCLUDE_MASHIRO_GRAPHICS_MODEL_H_

namespace Mashiro{

namespace Math{
	class Vector2;
	class Vector3;
	class Vector4;
	class Matrix;
}

namespace Graphics{
	class Texture;
	class RenderTarget;
}

namespace Scene {

using namespace Math;
using namespace Graphics;

class Model {
public:
	//•`‰æ
	void draw() const;
	void draw( const Matrix& world ) const;
	
	const Vector3* position() const;
	const Vector3* angle() const;
	const Vector3* scale() const;
	const Vector3* color() const;
	Matrix worldMatrix() const;	
	float transparaency() const;
	

	void setPosition( const Vector3& );
	void setPosition( const Vector2& in );
	void setAngle( const Vector3& );
	void setScale( const Vector3& );
	void setColor( const Vector3& );
	void setTransparaency( float );
	void setTexture( const RenderTarget& targetTexture, int num = 0 );
	void setTexture( const Texture& tex, int num = 0 );

	

	Model();
	~Model();
	Model( const Model& );
	operator void*() const;
	Model& operator=( const Model& );
	void release();
	bool operator==( const Model& ) const;
	bool operator!=( const Model& ) const;

	class Impl;
	explicit Model( Impl* );
private:
	Impl* mImpl;

};


}
}

#endif
