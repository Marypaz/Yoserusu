Texture2D diffuseTexture : register( t0 );
Texture2D normalTexture : register( t1 );
Texture2D positionTexture : register( t2 );
SamplerState texSample:register(s0)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

cbuffer ConstantBasic : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 proj;
	float4 mLight;
	float4 mDiffuse;
	float4 mAmbient;
	float4 mEye;
	float4 mSpecular;
};

cbuffer globalPost : register( b1 ){
	float4 lightPos;
};

struct VS_INPUT{
	float4 pos : POSITION;
	float3 normal : NORMAL;
	float4 col : COLOR;
	float2 uv : TEXCOORD;
};

struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float4 col : COLOR;
	float2 uv : TEXCOORD;
};

VS_OUTPUT DeferredPostVS( VS_INPUT input ){
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.pos = input.pos;
	output.uv = input.uv;

	return output;
}

float4 DeferredPostPS( VS_OUTPUT input ) : SV_TARGET0 {
	float4 diffuse = diffuseTexture.Sample( texSample, input.uv );
	float3 normal = normalTexture.Sample( texSample, input.uv );
	float3 pos = positionTexture.Sample( texSample, input.uv );

	float3 lightdir = mLight.xyz - pos.xyz;
	float3 lightvec = normalize(lightdir);
	float3 eyeVec = mEye.xyz;
	float3 diffuseIntensity = dot( lightvec, normal );
	
	float attenution = 2.0f / length( lightvec );

	float3 outcolor = diffuse * diffuseIntensity * attenution / 2;

	float4 finalColor = float4( outcolor, 1 );

	return finalColor;

}