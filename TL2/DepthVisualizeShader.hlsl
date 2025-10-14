// DepthVisualizeShader.hlsl

Texture2D<float> DepthTexture : register(t0);
SamplerState PointSampler : register(s0);

cbuffer CameraInfo : register(b0)
{
    float NearClip;
    float FarClip;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD;
};

VS_OUTPUT VS(uint vid : SV_VertexID)
{
    VS_OUTPUT output;
    output.texcoord.x = (vid == 1) ? 2.0f : 0.0f;
    output.texcoord.y = (vid == 2) ? 2.0f : 0.0f;
    output.position = float4(output.texcoord * 2.0f - 1.0f, 0.0f, 1.0f);
    output.position.y = -output.position.y;
    
    return output;
}

float LinearizeDepth(float rawDepth)
{
    float z = NearClip * FarClip / (FarClip - rawDepth * (FarClip - NearClip));
    return z / FarClip;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float rawDepth = DepthTexture.Sample(PointSampler, input.texcoord).r;
    
    if (rawDepth >= 1.0f)
    {
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    
    float linearDepth = LinearizeDepth(rawDepth);
    
    return float4(linearDepth, linearDepth, linearDepth, 1.0f);
}