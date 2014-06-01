//グローバル
Texture2D g_TexDecal: register(t0);
SamplerState g_SamLinear : register(s0)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

cbuffer ConstantBasic : register(b0)
{
	float4x4 mWorld;
	float4x4 mView;
	float4x4 mProj;
	float4 mLight;
	float4 mDiffuse;
	float4 mAmbient;
	float4 mEye;
	float4 mSpecular;
};

cbuffer ConstantBuffer : register( b1 )
{
	float4 mFog;
	float4 mFogColor;
	float4 mHeight;
};

struct VS_INPUT{
	float4 pos : POSITION;
	float3 nor : NORMAL;
	float4 col : COLOR;
	float2 tex : TEXCOORD;
};

struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float3 light : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float3 eyeVector : TEXCOORD2;
	float2 tex : TEXCOORD3;
	float4 pos2 : TEXCOORD4;
	float4 col : COLOR;
};

VS_OUTPUT vs_fog( VS_INPUT In ){
	VS_OUTPUT Out;
	//射影変換
	Out.pos = mul( In.pos, mWorld );
	Out.pos = mul( Out.pos, mView );
	Out.pos = mul( Out.pos, mProj );
	Out.pos2 = mul( In.pos, mWorld );
	//法線をモデルの姿勢に合わせる
	Out.normal = mul( In.nor, (float3x3)mWorld );
	//ライト方向
	Out.light = normalize( -mLight );
	//視線ベクトル ワールド空間での頂点から視線へ向かうベクトル
	float3 posWorld = normalize( mul( In.pos, mWorld ) );
	Out.eyeVector = normalize( mEye - posWorld );
	//テクスチャ座標
	Out.tex = In.tex;
	//カラー
	Out.col = In.col;

	return Out;
}

float4 ps_fog( VS_OUTPUT In ) : SV_Target {
	float4 tex_col = g_TexDecal.Sample( g_SamLinear, In.tex );
	//環境光
	float4 amb = tex_col;
	//拡散反射光
	float NL = saturate( dot( In.normal, In.light )  );
	float4 diffuse = ( In.col / 2 + tex_col / 2 ) * NL;
	//鏡面反射光
	float3 ref = normalize( 2 * NL * In.normal - In.light  );
	float4 specular = pow( saturate( dot( ref, In.eyeVector ) ), 4 ) * mSpecular;
	//フォンモデルの最終色
	float4 color = amb + diffuse + specular;

	//フォグの処理
	In.pos2 /= In.pos2.w;
	float y = 0.1; //絶対座標での高さ
	float h = mHeight.x; //基準の高さ
	float f = ( y / h ); //フォグファクター
	f = saturate( f ); //0～1に範囲にクランプ
	f *= mFog.x;
	f = saturate( f );
	float4 c = f * color + ( 1- f ) * mFogColor;

	return c;
}