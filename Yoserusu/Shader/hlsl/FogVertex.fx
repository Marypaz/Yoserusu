cbuffer ConstantBuffer
{
	float4x4 mProj;
	float4x4 mView;
	float4x4 mWorld;
	float4 mDiffuse;
	float4 mLight;
	float4 mFog;
	float4 mEye;
};

struct VS_INPUT{
	float4 pos : POSITION;
	float3 nor : NORMAL;
	float4 col : COLOR;
	float2 tex : TEXCOORD;
};

struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
	float3 tex : TEXCOORD0;
	float  fog : TEXCOORD1;
};

VS_OUTPUT vs_fog( VS_INPUT In ){
	VS_OUTPUT Out;
	//フォグ有効化
	FogEnable = true;
	FogVertexMode = LINEAR;
	FogColor = 0xd8e3fe;

	//座標変換
	Out.pos = mul( In.pos, mWorld );
	Out.pos = mul( Out.pos, mView );
	Out.pos = mul( Out.pos, mProj );

	Out.col = mDiffuse * max( dot( -mLight, In.nor ), 0 );

	Out.tex = In.tex;

	float4 pos = In.pos - mEye;
	Out.fog = mFog.x + length( pos ) * mFog.y;

	return Out;
}