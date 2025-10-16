// DepthPrepassShader.hlsl

cbuffer ModelBuffer : register(b0)
{
    matrix World;
};

cbuffer ViewProjBuffer : register(b1)
{
    matrix View;
    matrix Projection;
};

struct VS_INPUT
{
    float3 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
};
    
VS_OUTPUT mainVS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0f), World);
    output.position = mul(mul(output.position, View), Projection);
    return output;
}

void PS()
{
    // nothing-to-do.
}