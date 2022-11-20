// ビルボード行列
cbuffer ConstBufferDataBillBoard : register(b0)
{
	matrix matBillBoard;
};

// ビュー&射影変換行列
cbuffer ConstBufferDataViewProjection : register(b1)
{
	matrix matViewProjection;
};

// 頂点シェーダーの出力構造体
struct VSOutput
{
	float4 pos : POSITION;// 頂点座標
	float3 scale : SCALE;
	float4 color : COLOR;
};

// ジオメトリシェーダーの出力構造体
struct GSOutput
{
	float4 svpos : SV_POSITION;// システム用頂点座標
	float2 uv : TEXCOORD;// uv値
	float4 color : COLOR;
};