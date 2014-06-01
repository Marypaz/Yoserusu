//定義
#define MAX_BONE_MATRIX 255

//テクスチャー
Texture2D tex0 : register( t0 ); //オブジェクトのテクスチャ
Texture2D tex1 : register( t1 ); //トゥールマップテクスチャ
// サンプラーステート
SamplerState  g_Sampler : register( s0 ){
   Filter = MIN_MAG_MIP_LINEAR;
   AddressU = Wrap;
   AddressV = Wrap;
};

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

// 定数バッファ
cbuffer ConstantBuffer : register( b1 )
{
	float4x4 constBoneWorld[ MAX_BONE_MATRIX ];
};

//スキニング後の頂点・法線が入る
struct Skin {
	float4 pos;
	float3 nor;
};

//頂点レイアウト
struct VS_INPUT{
    float3 pos	: POSITION;			//	座標
    float3 nor : NORMAL;			//	法線
    float2 tex	: TEXCOORD;			//	テクスチャ座標
    uint4  bones : BONE_INDEX;		//	頂点に対応したボーンのインデックス
	float4 weights : BONE_WEIGHT;	//	ボーンの重み
};

struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float3 nor : NORMAL;
	float4 col : COLOR;
	float2 tex : TEXCOORD0;
};

Skin skinVert( VS_INPUT In ){
	Skin Out = (Skin)0;

	float4 pos = float4( In.pos, 1 );
	float3 nor = In.nor;

	//ボーン0
	uint iBone = In.bones.x;
	float fWeight = In.weights.x;
	float4x4 m = constBoneWorld[ iBone ];
	Out.pos += fWeight * mul( pos, m );
	Out.nor += fWeight * mul( nor, (float3x3) m );

	//ボーン1
	iBone = In.bones.y;
	fWeight = In.weights.y;
	m = constBoneWorld[ iBone ];
	Out.pos += fWeight * mul( pos, m );
	Out.nor += fWeight * mul( nor, (float3x3) m );

	//ボーン2
	iBone = In.bones.z;
	fWeight = In.weights.z;
	m = constBoneWorld[ iBone ];
	Out.pos += fWeight * mul( pos, m );
	Out.nor += fWeight * mul( nor, (float3x3) m );

	//ボーン3
	iBone = In.bones.w;
	fWeight = In.weights.w;
	m = constBoneWorld[ iBone ];
	Out.pos += fWeight * mul( pos, m );
	Out.nor += fWeight * mul( nor, (float3x3) m );

	return Out;
}

VS_OUTPUT vs_SkinMesh( VS_INPUT In ){
	VS_OUTPUT Out;

	//習得した姿勢行列を頂点に掛け合わせる
	Skin vSkinned = skinVert( In );
	
	//座標
	Out.pos = mul( vSkinned.pos, g_World );
	Out.pos = mul( Out.pos, g_View );
	Out.pos = mul( Out.pos, g_Proj );
	//法線
	Out.nor = normalize( In.nor.xyz );

	float3 light = -normalize( g_vecLight.xyz );

	//カラー
	Out.col = max( float4( 1, 1, 1, 1 ), dot( Out.nor, light) );
	//uv
	Out.tex = In.tex;

	return Out;
};

float4 ps_SkinMesh( VS_OUTPUT In ) : SV_Target {
	float4 Out;

	float p = dot( In.nor, -g_vecLight.xyz );
	p = p * 0.5f + 0.5f;
	p = p * p;

	float4 TexDiffuse = tex0.Sample( g_Sampler, In.tex );

	Out = g_Diffuse *  TexDiffuse * In.col;

	return Out;
}