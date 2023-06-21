struct OutputVS
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer ConstBufferData : register(b0)
{
    float sigma;
    float stepwidth;
};