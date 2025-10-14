
Texture2D FrameColor : register(t0);
SamplerState LinearSampler : register(s0);
SamplerState PointSampler : register(s1);


struct VS_Input
{
    float3 posModel : POSITION;
    float2 uv : TEXCOORD0;
};

struct PS_Input
{
    float4 posCS : SV_Position;
    float2 uv : TEXCOORD0;
};
