#ifndef INCLUDE_MASHIRO_MATH_FUNCTIONS_H_
#define INCLUDE_MASHIRO_MATH_FUNCTIONS_H_

#undef min
#undef max

//���w���C�u�����[�B�p�x�P�ʂ͓x�ŕԂ��܂�
namespace Mashiro {

namespace Math{

//�O�p�֐�
float sin( float );
float cos( float );
float tan( float );
//�t�O�p�֐�
float asin( float );
float acos( float );
float atan( float );
float atan2( float, float );

float pow( float a, float b );//�ׂ���(a��b��)
float log2( float );//2���Ƃ���ΐ�
float log10( float );//10���Ƃ���ΐ�
float abs( float );//��Βl
float min( float, float );//�ŏ�
float max( float, float );//�ő�
float sqrt( float );//������

const float EPSILON = 00.00000011920928955078125f;
const float FLOAT_MAX = 1.7014117331926442990585209174226e+38;

bool isSpecial( float );//NaN,Inf�Ȃ�true

}
}

#endif
