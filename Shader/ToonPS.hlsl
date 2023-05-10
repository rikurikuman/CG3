#include "Toon.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
	float4 texcolor = float4(tex.Sample(smp, input.uv));
	texcolor = texcolor * m_color;
	
	//����x
    const float shininess = 20.0f;
	
	//���_�ւ̃x�N�g��
    float3 eyedir = normalize(cameraPos - input.wpos.xyz);
	
	//�����ˌ�
    float3 ambient = ambientColor * m_ambient;
    
    float diffuseIntensity = 0;
    float3 diffuse = float3(0, 0, 0);
    float specularIntensity = 0;
    float3 specular = float3(0, 0, 0);
	
	//�V�F�[�f�B���O���ʂ̐F
    float4 shadecolor = float4(0, 0, 0, 1);
	
    //���s����
    for (int i = 0; i < DIRECTIONAL_LIGHT_NUM; i++)
    {
        if (directionalLights[i].active)
        {
            //�����ւ̃x�N�g���Ɩ@���̓���
            float dotNormal = dot(-directionalLights[i].lightVec, input.normal);
		    //���ˌ��x�N�g��
            float3 reflect = normalize(directionalLights[i].lightVec + 2.0f * dotNormal * input.normal);
		    //�g�U���ˌ�
            diffuseIntensity += smoothstep(0.25f, 0.3f, saturate(dotNormal));
		    //���ʔ��ˌ�
            specularIntensity += smoothstep(0.25f, 0.3f, pow(saturate(dot(reflect, eyedir)), shininess));

            diffuse += m_diffuse * directionalLights[i].lightColor;
            specular += m_specular * directionalLights[i].lightColor;
        }
    }
    
    if (specularIntensity > 0)
    {
        shadecolor = float4(specularIntensity * specular + (1 - specularIntensity) * diffuseIntensity * diffuse, 1);
    }
    else if(diffuseIntensity > 0)
    {
        shadecolor = float4(diffuseIntensity * diffuse + (1 - diffuseIntensity) * ambient, 1);
    }
    else
    {
        shadecolor = float4(ambient, 1);
    }
    
    return texcolor * shadecolor;
}