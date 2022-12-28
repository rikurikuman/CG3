#include "Basic.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	float4 texcolor = float4(tex.Sample(smp, input.uv));
	texcolor = texcolor * m_color;

	float3 lightdir = float3(1, -1, 1);
	lightdir = normalize(lightdir);

	float3 diffuse = dot(-lightdir, input.normal) * m_diffuse;
	float3 ambient = m_ambient;

	float3 light = diffuse + ambient;
	return float4(texcolor.rgb * light, texcolor.a);
}