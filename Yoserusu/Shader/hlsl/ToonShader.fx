cbuffer ConstantBasic : register(b0)
{
	float4x4 g_World;
	float4x4 g_View;
	float4x4 g_Proj;
	float4 g_vecLight;
	float4 g_Diffuse;
	float4 mAmbient;
	float4 mEye;
	float4 mSpecular;
};

//テクスチャー
Texture2D tex0 : register( t0 ); //オブジェクトのテクスチャ
Texture2D tex1 : register( t1 ); //トゥールマップテクスチャ

// サンプラーステート
SamplerState  g_Sampler : register( s0 ){
   Filter = MIN_MAG_MIP_LINEAR;
   AddressU = Wrap;
   AddressV = Wrap;
};

//頂点レイアウト
struct VS_INPUT{
	float4 pos  : POSITION;   // 頂点座標
    float3 nor  : NORMAL;     // 法線
    float4 col  : COLOR;      //カラー
    float2 tex  : TEXCOORD;   // テクセル
};

struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float3 nor : NORMAL;
	float4 col : COLOR;
	float2 tex : TEXCOORD0;
};

VS_OUTPUT vs_toon( VS_INPUT In ){
	VS_OUTPUT Out;
	//座標
	Out.pos = mul( In.pos, g_World );
	Out.pos = mul( Out.pos, g_View );
	Out.pos = mul( Out.pos, g_Proj );
	//法線
	Out.nor = normalize( In.nor.xyz );
	//カラー
	Out.col = In.col;
	//uv
	Out.tex = In.tex;


   //照明を逆方向にする。(注意１)
   float3 L = -normalize( g_vecLight.xyz );
   
   //法線ベクトル。
   float3 N = normalize( In.nor.xyz );

   //照明と法線からdot関数により、内積を計算する。またmax関数により頂点カラーの最小値を環境光に抑えるように設定する。(注意２)
   Out.col.xyz = max( g_Diffuse, dot(N, L) ) * In.col.xyz;

	return Out;
};

float4 ps_toon( VS_OUTPUT In ) : SV_Target {
	float4 Out;

	/*//ハーフランバート
	float p = dot( In.nor, g_vecLight.xyz );
	p = p * 0.5f + 0.5f;
	p = p * p;

	//トゥーン
	//float4 col = tex1.Sample( g_Sampler, float2( p, 0.f )  );

	//色情報を格納
		//In.col.a += 1.f;
	float4 tex = tex0.Sample( g_Sampler, In.tex );
	Out = g_Diffuse * tex  * p * In.col;
	Out.a = 1.f;
	//Out = In.col * p;*/

	Out = In.col * tex0.Sample( g_Sampler, In.tex );

	return Out;
}