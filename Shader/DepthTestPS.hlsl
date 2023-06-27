#include "DepthTest.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
Texture2D<float4> depthTex : register(t1); //1番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(OutputVS i) : SV_TARGET
{
    float4 color = depthTex.Sample(smp, i.uv);
    return color;
}