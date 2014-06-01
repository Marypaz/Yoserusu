cbuffer ConstantBasic : register(b0)
{
	float4x4 mWorld;
	float4x4 mView;
	float4x4 mProj;
	float4 mLight;
	float4 mColor;
	float4 mAmbient;
	float4 mEye;
	float4 mSpecular;
};

struct VS_INPUT{
	float4 pos : POSITION;
	float3 nor : NORMAL;
	float4 col : COLOR;
	float2 tex : TEXCOORD;
};

struct PS_INPUT{
	float4 pos : SV_POSITION;
	float3 nor : NORMAL;
	float4 col : COLOR;
	float2 tex : TEXCOORD;
};

PS_INPUT vs_outline( VS_INPUT In ){
	PS_INPUT Out;

	//座標変換
	Out.pos = mul( In.pos, mWorld );
	Out.pos = mul( Out.pos, mView );
	Out.pos = mul( Out.pos, mProj );

	float4 n = mul( In.nor, mWorld );
	n = mul( n, mView );
	n = mul( n, mProj );
	n.zw = 0;

	Out.pos = Out.pos + 0.05f * n;
	Out.col = 0;
	Out.col.a = 1;

	return Out;
};

float4 ps_outline( PS_INPUT In ) : SV_Target {
	return In.col;
}