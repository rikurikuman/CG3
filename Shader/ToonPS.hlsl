#include "Toon.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	float4 texcolor = float4(tex.Sample(smp, input.uv));
	texcolor = texcolor * m_color;
	
	//光沢度
    const float shininess = 20.0f;
	
	//視点へのベクトル
    float3 eyedir = normalize(cameraPos - input.wpos.xyz);
	
	//環境反射光
    float3 ambient = ambientColor * m_ambient;
    
    float diffuseIntensity = 0;
    float3 diffuse = float3(0, 0, 0);
    float specularIntensity = 0;
    float3 specular = float3(0, 0, 0);
	
	//シェーディング結果の色
    float4 shadecolor = float4(0, 0, 0, 1);
	
    //平行光源
    for (int i = 0; i < DIRECTIONAL_LIGHT_NUM; i++)
    {
        if (directionalLights[i].active)
        {
            //光源へのベクトルと法線の内積
            float dotNormal = dot(-directionalLights[i].lightVec, input.normal);
		    //反射光ベクトル
            float3 reflect = normalize(directionalLights[i].lightVec + 2.0f * dotNormal * input.normal);
		    //拡散反射光
            diffuseIntensity += smoothstep(0.25f, 0.3f, saturate(dotNormal));
		    //鏡面反射光
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