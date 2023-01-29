#include "Basic.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	float4 texcolor = float4(tex.Sample(smp, input.uv));
	texcolor = texcolor * m_color;
	
    float dotNormal = saturate(dot(-light_vec, input.normal));
	
	//視点へのベクトル
    float3 eyedir = normalize(cameraPos - input.wpos.xyz);
	
	//反射光ベクトル
    float3 reflect = normalize(light_vec + 2 * dotNormal * input.normal);

	float3 ambient = m_ambient;
    float3 diffuse = dotNormal * m_diffuse;
    float3 specular = pow(saturate(dot(reflect, eyedir)), 4.0f) * m_specular;

    float3 light = (ambient + diffuse + specular) * light_color;
	return float4(texcolor.rgb * light, texcolor.a);
}