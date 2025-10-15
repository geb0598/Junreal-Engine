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
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

VS_OUTPUT mainVS(uint vid : SV_VertexID)
{
    float2 pos[3] =
    {
        float2(-1.0f, 3.0f),
        float2(3.0f, -1.0f),
        float2(-1.0f, -1.0f)
    };
    VS_OUTPUT output;
    //output.texcoord.x = (vid == 2) ? 2.0f : 0.0f;
    //output.texcoord.y = (vid == 1) ? 2.0f : 0.0f;
    //output.position = float4(output.texcoord * 2.0f - 1.0f, 0.0f, 1.0f);
    //output.position.y = -output.position.y;
    output.position.xy = pos[vid];
    output.position.z = 0.1f;
    output.position.w = 1.0f;
    output.texcoord = output.position;
    
    return output;
}

float LinearizeDepth(float rawDepth)
{
    float z = NearClip * FarClip / (FarClip - rawDepth * (FarClip - NearClip));
    return z / FarClip;
}

float4 mainPS(VS_OUTPUT input) : SV_Target
{
    //return float4(1.0f, 1.0f, 1.0f, 1.0f);
    float rawDepth = DepthTexture.Sample(PointSampler, input.texcoord).r;
    
    if (rawDepth >= 1.0f)
    {
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    
    float linearDepth = LinearizeDepth(rawDepth);
    
    return float4(linearDepth, linearDepth, linearDepth, 1.0f);
}
