// ShaderW0.hlsl

cbuffer ModelBuffer : register(b0)
{
    row_major float4x4 WorldMatrix;
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

// 그리드 파라미터 (b3)
cbuffer GridParams : register(b3)
{
    float CellSize; // 한 칸 간격(월드 유닛). 예: 1.0
    float LineWidth; // 라인 두께(픽셀 단위). 예: 1.0
    int MajorEvery; // 메이저 라인 간격(몇 칸마다 굵게). 예: 10
    float AxisWidth; // X/Z 축 두께(월드 유닛). 예: 0.02

    float4 MinorColor; // (r,g,b,a) 예: (0.5,0.5,0.5,0.8)
    float4 MajorColor; // (r,g,b,a) 예: (0.7,0.7,0.7,0.9)
    float3 AxisXColor; // X축(빨강) 예: (1,0.2,0.2)
    float _pad0;
    float3 AxisZColor; // Z축(파랑) 예: (0.2,0.4,1)
    float _pad1;
}

struct VS_INPUT
{
    float3 position : POSITION; // Input position from vertex buffer
    float4 color : COLOR; // Input color from vertex buffer
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 color : COLOR; // Color to pass to the pixel shader
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    // 상수버퍼를 통해 넘겨 받은 Offset을 더해서 버텍스를 이동 시켜 픽셀쉐이더로 넘김
    // float3 scaledPosition = input.position.xyz * Scale;
    // output.position = float4(Offset + scaledPosition, 1.0);
    
    float4x4 MVP = mul(mul(WorldMatrix, ViewMatrix), ProjectionMatrix);
    
    output.position = mul(float4(input.position, 1.0f), MVP);    
    
    
    // change color
    float4 c = input.color;

    

    // Picked가 1이면 전달된 하이라이트 색으로 완전 덮어쓰기
    if (Picked == 1)
    {
        const float highlightAmount = 0.35f; // 필요시 조절
        float3 highlighted = saturate(lerp(c.rgb, Color, highlightAmount));
        
        // 정수 Picked(0/1)를 마스크로 사용해 분기 없이 적용
        float mask = (Picked != 0) ? 1.0f : 0.0f;
        c.rgb = lerp(c.rgb, highlighted, mask);
        
        //알파 값 설정
        c.a = 0.5;
    }
    
    if(GIzmo == 1)
    {
        if(Y == 1)
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
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    // Output the color directly
    return input.color;
}

