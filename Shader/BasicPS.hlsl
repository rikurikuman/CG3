#include "Basic.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
	float4 texcolor = float4(tex.Sample(smp, input.uv));
	texcolor = texcolor * m_color;
	
    float dotNormal = saturate(dot(-light_vec, input.normal));
	
	//���_�ւ̃x�N�g��
    float3 eyedir = normalize(cameraPos - input.wpos.xyz);
	
	//���ˌ��x�N�g��
    float3 reflect = normalize(light_vec + 2 * dotNormal * input.normal);

	float3 ambient = m_ambient;
    float3 diffuse = dotNormal * m_diffuse;
    float3 specular = pow(saturate(dot(reflect, eyedir)), 4.0f) * m_specular;

    float3 light = (ambient + diffuse + specular) * light_color;
	return float4(texcolor.rgb * light, texcolor.a);
}