cbuffer ViewProjBuffer : register(b1)
{
    row_major float4x4 ViewMatrix;
    row_major float4x4 ProjectionMatrix;
}
cbuffer InvWorldBuffer : register(b3)
{
    row_major float4x4 ViewProjMatrixInverse;
}

cbuffer ViewportBuffer : register(b6)
{
    float4 ViewportRect; // x=StartX, y=StartY, z=SizeX, w=SizeY
}

cbuffer FogConstant : register(b8)
{
    float4 FogInscatteringColor;

    float FogDensity;
    float FogHeightFalloff;
    float StartDistance;
    float FogCutoffDistance;
    float FogMaxOpacity;
    float FogActorHeight;
    float2 Padding;
}

Texture2D ColorTexture : register(t0);
Texture2D DepthTexture : register(t1);
SamplerState Sampler : register(s0);

struct PS_INPUT
{
    float4 Position : SV_Position;
    float2 UV : UV;
};

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
};

PS_INPUT mainVS(uint Input : SV_VertexID)
{
    PS_INPUT Output;

    Output.UV = float2((Input << 1) & 2, Input & 2);

    Output.Position = float4(Output.UV * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    
    return Output;
}

PS_OUTPUT mainPS(PS_INPUT Input)
{
    PS_OUTPUT Output;
    
    float2 ViewportUV = Input.Position.xy - ViewportRect.xy;
    ViewportUV = ViewportUV / ViewportRect.zw;
    
    uint Width, Height;
    DepthTexture.GetDimensions(Width, Height);
    
    float2 UVPosition = Input.Position.xy / float2(Width, Height);
    float4 OriginalColor = ColorTexture.Sample(Sampler, UVPosition);
    float Depth = DepthTexture.Sample(Sampler, UVPosition);
    
    float4 NDCPosition = float4((ViewportUV.x - 0.5f) * 2.0f, (0.5f - ViewportUV.y) * 2.0f, Depth, 1.0f);
    
    float4 WorldPosition = mul(NDCPosition, ViewProjMatrixInverse);
    WorldPosition /= WorldPosition.w;

    
    float4 CameraPosition = ViewMatrix._m30_m31_m32_m33;
    //카메라부터 포그 적용될 점까지 거리
    float DistanceToPoint = length(CameraPosition - WorldPosition);
    //StartDistance보다 거리가 짧으면 밀도 0, FogCutoffDistance이상이면 1(밀도 최상)
    float DistanceFactor = saturate((DistanceToPoint - StartDistance) / (FogCutoffDistance - StartDistance));
    
    //비어-람베르트 공식(실제 포그 물리량) 위로갈수록 밀도가 낮아지므로 Transparency로 표현함. Falloff값이 커지면 Transparency도 커짐.
    float FogTransparency = exp(-(FogHeightFalloff * (WorldPosition.z - FogActorHeight)));
    //투명도 DensityFactor로 조절하고 거리가 멀면 더 짙어짐
    FogTransparency = exp(-(FogTransparency * FogDensity * DistanceToPoint));
    
    //실제 포그 밀도에 DistanceFactor 곱해서 clamp
    float FogFactor = (1.0f - FogTransparency) * DistanceFactor;
    Output.Color = FogFactor * FogInscatteringColor + (1.0f - FogFactor) * OriginalColor;
    
    return Output;
}