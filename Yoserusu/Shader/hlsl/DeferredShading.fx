Texture2D sceneTexture : register( t0 );
SamplerState texSample :  register(s0)
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

struct VS_INPUT{
	float4 pos : POSITION;
	float3 normal : NORMAL;
	float4 col : COLOR;
	float2 uv : TEXCOORD;
};

struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float4 posWorld : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct PS_OUTPUT{
	float4 diffuse : SV_Target0;
	float4 normal : SV_Target1;
	float4 pos : SV_Target2;
};

VS_OUTPUT DeferredShadingVS( VS_INPUT input ){
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.normal = mul(input.normal,(float3x3)world);

	output.posWorld = mul(input.pos, world);
	output.pos = mul(input.pos, world);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, proj);

	output.uv = input.uv;

	return output;
}

PS_OUTPUT DeferredShadingPS( VS_OUTPUT input ){
	PS_OUTPUT output;

	output.diffuse =  sceneTexture.Sample( texSample, input.uv );

	float3 normal =  input.normal;

	output.normal = float4( input.normal, 1 );

	output.pos = input.posWorld;
	output.pos.a = 1.0f;

	return output;
}