#ifndef INCLUDE_MASHIRO_MATH_MATRIX_44_H_
#define INCLUDE_MASHIRO_MATH_MATRIX_44_H_

//#include <D3DX10math.h>

namespace Mashiro{

namespace Math{

class Vector3;
class Vector4;

class Matrix{
public:
	Matrix();
	//�P�ʍs��
	void setIdentity();
	//�����ϊ��s��̐���
	void setPerspectiveFovLH( 
		float fieldOfViewYInRadian,
		float windowWidth,
		float windowHeight,
		float nearClip,
		float farClip );
	//���ˉe�s��̐���
	void setOrthoLH(
		float width,
		float height,
		float nearClip,
		float farClip );
	//view�s�������Ă��̏�Ŋ|���Z����
	void setLookAtLH(
		const Vector3& eyePosition,
		const Vector3& eyeTarget,
		const Vector3& upVector );
	//�X�N���[�����W���RD��Ԃ�
	Vector3 convertWorld(
		int width, int height,
		Vector3* vec,
		Matrix projectionMatrix,
		Matrix viewMatrix );
	//�r���{�[�h
	void bilBoard(
		const Vector3& bilPos,
		const Vector3& targetPos );

	//�ړ�
	void setTranslation( float, float, float );
	void setTranslation( const Vector3& );
	Math::Vector4 translation( const Vector4&, const Matrix& );
	//��]
	void setRotationX( float );
	void setRotationY( float );
	void setRotationZ( float );
	//�g��k��
	void setScaling( float xyz );
	void setScaling( float x, float y, float z );
	void setScaling( const Vector3& );
	//�t�s��
	void setInverse( const Matrix& );
	//�]�u�s��
	void setTranspose( const Matrix& );

	void setMul( const Matrix&, const Matrix& );
	void mul( Vector3* out, const Vector3& in ) const;
	///���S�Ɉ�v���Ă��邩�`�F�b�N
	bool operator==( const Matrix& ) const;
	///�����ł��Ⴄ�̂����`�F�b�N
	bool operator!=( const Matrix& ) const;
	Matrix operator*( const Matrix& );
	void operator*=( const Matrix& );
	float& operator()( int row, int col );
	float operator()( int row, int col ) const;

	//D3DXMATRIX m;
	struct Data {
		Data();
		void set( const float* fp );
		float m11, m12, m13, m14;
		float m21, m22, m23, m24;
		float m31, m32, m33, m34;
		float m41, m42, m43, m44;
	};
	Data m;
};

}
}

#endif
