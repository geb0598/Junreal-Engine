#include "Common.hlsli"

//Post Processing VertexShader

Texture2D FrameColor : register(t0);
SamplerState PointSampler : register(s0);

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
    o.posCS = i.posModel;
    o.uv = i.uv;
    return o;
}