#include "Basic.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
	float4 texcolor = float4(tex.Sample(smp, input.uv));
	float3 shade_color;

	float3 light = normalize(float3(1, -1, 1)); //�E���������̃��C�g
	float diffuse = saturate(dot(-light, input.normal)); //�g�U���ˌ���[0, 1]�͈̔͂�Clamp����

	texcolor = texcolor * m_color;
	shade_color = m_ambient;
	shade_color += m_diffuse * diffuse;

	return float4(texcolor.rgb * shade_color, texcolor.a * m_color.a);
}