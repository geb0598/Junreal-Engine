cbuffer ModelBuffer : register(b0)
{
    row_major float4x4 WorldMatrix;
    uint UUID;
    float3 Padding;
}

cbuffer ViewProjBuffer : register(b1)
{
    row_major float4x4 ViewMatrix;
    row_major float4x4 ProjectionMatrix;
}

cbuffer HighLightBuffer : register(b2)
{
    int Picked;
    float3 Color;
    int X;
    int Y;
    int Z;
    int GIzmo;
}


Texture2D g_DiffuseTexColor : register(t0);
SamplerState g_Sample : register(s0);

struct FMaterial
{
    float3 DiffuseColor; // Kd
    float OpticalDensity; // Ni
    
    float3 AmbientColor; // Ka
    float Transparency; // Tr Or d
    
    float3 SpecularColor; // Ks
    float SpecularExponent; // Ns
    
    float3 EmissiveColor; // Ke
    uint IlluminationModel; // illum. Default illumination model to Phong for non-Pbr materials
    
    float3 TransmissionFilter; // Tf
    float dummy;
};

cbuffer ColorBuffer : register(b3)
{
    float4 LerpColor;
}

cbuffer PixelConstData : register(b4)
{
    FMaterial Material;
    bool HasMaterial; // 4 bytes
    bool HasTexture;
}
cbuffer PSScrollCB : register(b5)
{
    float2 UVScrollSpeed;
    float  UVScrollTime;
    float  _pad_scrollcb;
}
#define MAX_PointLight 100

// C++ 구조체와 동일한 레이아웃
struct FPointLightData
{
    float4 Position;   // xyz=위치, w=반경
    float4 Color;      // rgb=색상, a=Intensity
    float FallOff;     // 감쇠
    float3 _pad;       // 패딩 (16바이트 정렬)
};

cbuffer PointLightBuffer : register(b9)
{
    int PointLightCount;
    float3 _pad;
    FPointLightData PointLights[MAX_PointLight];
}

float3 ComputePointLights(float3 worldPos)
{
    float3 totalLight = 0;
    for (int i = 0; i < PointLightCount; ++i)
    {
        float3 toLight = worldPos - PointLights[i].Position.xyz;
        float dist = length(toLight);
        float atten = saturate(1.0 - dist / PointLights[i].Position.w);
        atten = pow(atten, PointLights[i].FallOff);
        totalLight += PointLights[i].Color.rgb * PointLights[i].Color.a * atten;
    }
    return totalLight;
}



struct VS_INPUT
{
    float3 position : POSITION; // Input position from vertex buffer
    float3 normal : NORMAL0;
    float4 color : COLOR; // Input color from vertex buffer
    float2 texCoord : TEXCOORD0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
    float3 worldNormal : TEXCOORD1;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD2;
    uint UUID : UUID;
};
struct PS_OUTPUT
{
    float4 Color : SV_Target0;
    uint UUID : SV_Target1;
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    // 상수버퍼를 통해 넘겨 받은 Offset을 더해서 버텍스를 이동 시켜 픽셀쉐이더로 넘김
    // float3 scaledPosition = input.position.xyz * Scale;
    // output.position = float4(Offset + scaledPosition, 1.0);
        // World 변환
    float4 worldPos = mul(float4(input.position, 1.0f), WorldMatrix);
    output.worldPosition = worldPos.xyz;

    // 노멀 변환 (정규화)
    output.worldNormal = normalize(mul(input.normal, (float3x3) WorldMatrix));
    
    float4x4 MVP = mul(mul(WorldMatrix, ViewMatrix), ProjectionMatrix);
    
    output.position = mul(float4(input.position, 1.0f), MVP);
    
    
    // change color
    float4 c = input.color;
    
    if (GIzmo == 1)
    {
        if (Y == 1)
        {
            c = float4(1.0, 1.0, 0.0, c.a); // Yellow
        }
        else
        {
            if (X == 1)
                c = float4(1.0, 0.0, 0.0, c.a); // Red
            else if (X == 2)
                c = float4(0.0, 1.0, 0.0, c.a); // Green
            else if (X == 3)
                c = float4(0.0, 0.0, 1.0, c.a); // Blue
        }
        
    }
    
    
    // Pass the color to the pixel shader
    output.color = c;
    output.texCoord = input.texCoord;
    output.UUID = UUID;
    return output;
}

PS_OUTPUT mainPS(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT Result;
    // Lerp the incoming color with the global LerpColor
    float4 baseColor = input.color;
    baseColor.rgb = lerp(baseColor.rgb, LerpColor.rgb, LerpColor.a) * (1.0f - HasMaterial);
    //finalColor.rgb += Material.DiffuseColor * HasMaterial;
    
    if (HasMaterial && HasTexture)
    {
        float2 uv = input.texCoord + UVScrollSpeed * UVScrollTime;
        baseColor.rgb = g_DiffuseTexColor.Sample(g_Sample, uv);
    }
    if (Picked == 1)
    {
        // 노란색 하이라이트를 50% 블렌딩
        float3 highlightColor = float3(1.0, 1.0, 0.0); // 노란색
        baseColor.rgb = lerp(baseColor.rgb, highlightColor, 0.5);
    }
      // 🔥 FireBall(PointLight) lighting
    float3 lightAccum = ComputePointLights(input.worldPosition);

    // 약간의 기본 환경광 (ambient)
    float3 ambient = 0.25 * baseColor.rgb;

    float3 finalLit = baseColor.rgb * (lightAccum + ambient);

    Result.Color = float4(finalLit.rgb, 1.0f);
    Result.UUID = input.UUID;
    return Result;
}

