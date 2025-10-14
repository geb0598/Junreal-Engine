
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

PS_Input mainVS(VS_Input i)
{
    PS_Input o;
    o.posCS = float4(i.posModel, 1);
    o.uv = i.uv;
    return o;
}

float4 mainPS(PS_Input i) : SV_TARGET
{
    return float4(1,0,0,1);
}


