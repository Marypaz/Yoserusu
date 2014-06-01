cbuffer ConstantBasic : register(b0)
{
	float4x4 g_World;
	float4x4 g_View;
	float4x4 g_Proj;
	float4 g_vecLight;
	float4 mColor;
	float4 mAmbient;
	float4 mEye;
	float4 mSpecular;
};

//テクスチャー
Texture2D tex : register( t0 ); //オブジェクトのテクスチャ

// サンプラーステート
SamplerState  g_Sampler : register( s0 ){
   Filter = MIN_MAG_MIP_LINEAR;
   AddressU = Wrap;
   AddressV = Wrap;
};

//Input
struct VS_INPUT{
	float4 pos  : POSITION;   // 頂点座標
    float3 nor  : NORMAL;     // 法線
    float4 col  : COLOR;      //カラー
    float2 tex  : TEXCOORD;   // テクセル
};

//Output
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
	float2 tex : TEXCOORD0;
};

VS_OUTPUT vs_effect( VS_INPUT In ){
	VS_OUTPUT Out;

	//座標変換
	Out.pos = mul( In.pos, g_World );
	Out.pos = mul( Out.pos, g_View );
	Out.pos = mul( Out.pos, g_Proj );

	//頂点単位でライティング
	float p = dot( In.nor, float3( 1,1,1 ) );
	p = p * 0.5f + 0.5f;
	p = p * p;

	Out.col = In.col * mColor * p;
	Out.tex = In.tex;

	return Out;
}

float4 ps_effect( VS_OUTPUT In ) : SV_Target {
	float4 color;
	color = In.col * tex.Sample( g_Sampler, In.tex );
	return color;
}