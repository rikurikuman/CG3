#include "PerlinNoise.hlsli"

cbuffer noiseData : register(b0)
{
    float t;
}

float random(float2 fact)
{
    return frac(sin(dot(float2(fact.x + t, fact.y + t), float2(12.9898, 78.233))) * 43758.5453);
}

float2 randomVec(float2 fact)
{
    float2 angle = float2(
        dot(fact, float2(127.1, 311.7)),
        dot(fact, float2(269.5, 183.3))
    );

    return frac(sin(angle) * 43578.5453123) * 2 - 1;
}

float PerlinNoise(float density, float2 uv)
{
    float2 uvFloor = floor(uv * density);
    float2 uvFrac = frac(uv * density);
    
    float2 v00 = randomVec(uvFloor + float2(0, 0));
    float2 v01 = randomVec(uvFloor + float2(0, 1));
    float2 v10 = randomVec(uvFloor + float2(1, 0));
    float2 v11 = randomVec(uvFloor + float2(1, 1));
    
    float c00 = dot(v00, uvFrac - float2(0, 0));
    float c01 = dot(v01, uvFrac - float2(0, 1));
    float c10 = dot(v10, uvFrac - float2(1, 0));
    float c11 = dot(v11, uvFrac - float2(1, 1));
    
    float2 u = uvFrac * uvFrac * (3 - 2 * uvFrac);
    
    float v0010 = lerp(c00, c10, u.x);
    float v0111 = lerp(c01, c11, u.x);
    
    return lerp(v0010, v0111, u.y) / 2 + 0.5;
}

float FractalSumNoise(float density, float2 uv)
{
    float fn = 0;
    fn += PerlinNoise(density * 1, uv) * 1.0 / 2;
    fn += PerlinNoise(density * 2, uv) * 1.0 / 4;
    fn += PerlinNoise(density * 4, uv) * 1.0 / 8;
    fn += PerlinNoise(density * 8, uv) * 1.0 / 16;
    return fn;
}

float4 main(OutputVS i) : SV_TARGET
{
    float density = 20;
    float pn = FractalSumNoise(density, i.uv + float2(t, t));
    return float4(pn, pn, pn, 1.0f);
}