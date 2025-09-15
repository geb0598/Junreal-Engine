// C++에서 상수 버퍼를 통해 전달될 데이터
cbuffer CameraInfo : register(b0)
{
    float3 textWorldPos;
    row_major matrix viewMatrix;
    row_major matrix projectionMatrix;
    row_major matrix viewInverse;
    //float3 cameraRight_worldspace;
    //float3 cameraUp_worldspace;
};

// C++의 BillboardCharInfo와 레이아웃이 동일해야 하는 입력 구조체
struct VS_INPUT
{
    float3 centerPos : WORLDPOSITION;
    float2 size : SIZE;
    float4 uvRect : UVRECT;
    uint vertexId : SV_VertexID; // GPU가 자동으로 부여하는 고유 정점 ID
};

struct PS_INPUT
{
    float4 pos_screenspace : SV_POSITION;
    float2 tex : TEXCOORD0;
};

Texture2D fontAtlas : register(t0);
SamplerState linearSampler : register(s0);


PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;

    float3 pos_aligned = mul(float4(input.centerPos, 0.0f), viewInverse).xyz;
    float3 finalPos_worldspace = textWorldPos + pos_aligned;

    // 3. 최종 화면 좌표로 변환
    output.pos_screenspace = mul(float4(finalPos_worldspace, 1.0f), mul(viewMatrix, projectionMatrix))*0.1;
    
    output.tex = input.uvRect.xy; // UV는 C++에서 계산했으므로 그대로 전달

    return output;
}

/*PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    // 1. 빌보드 중심점을 월드 공간 -> '뷰 공간'으로 변환
    float4 pos_viewspace = mul(float4(input.centerPos, 1.0f), viewMatrix);

    // 2. 뷰 공간에서 2D 오프셋 계산 (카메라 시점에서는 X,Y가 바로 좌우,상하)
    float2 cornerOffsets[4] =
    {
        float2(-0.5f, 0.5f), float2(0.5f, 0.5f),
        float2(-0.5f, -0.5f), float2(0.5f, -0.5f)
    };
    uint cornerId = input.vertexId % 4;
    float2 offset = cornerOffsets[cornerId] * input.size.xy;

    // 3. 뷰 공간 좌표에 2D 오프셋을 더해 최종 꼭짓점 위치 생성
    pos_viewspace.xy += offset;
    
    // 4. 이 뷰 공간 좌표를 최종 화면 좌표(투영 공간)로 변환
    output.pos_screenspace = mul(pos_viewspace, projectionMatrix);

    // UV 좌표 계산은 동일
    float2 uvCorners[4] = { float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 1) };
    output.tex = input.uvRect.xy + uvCorners[cornerId] * input.uvRect.zw;
    
    return output;
}*/
/*PS_INPUT mainVS(V/S_INPUT input)
{
    PS_INPUT output;
    
    // 1. 모델 공간의 기본 꼭짓점 위치 (-0.5 ~ 0.5)
    float2 cornerOffsets[4] =
    {
        float2(-0.5f, 0.5f), float2(0.5f, 0.5f),
        float2(-0.5f, -0.5f), float2(0.5f, -0.5f)
    };
    
    // SV_VertexID를 직접 인덱스로 사용
    float3 vertexPos_modelspace = float3(cornerOffsets[input.vertexId % 4] * input.size, 0);

    // 2. [핵심] 카메라의 회전을 상쇄시켜 빌보드를 카메라와 정렬시킴
    // inverseViewMatrix에서 이동(translation) 정보는 제거해야 순수한 회전만 적용됨
    // (보통 C++에서 역행렬의 4행 1,2,3열을 0으로 만들어 전달)
    float3 vertexPos_aligned = mul(float4(vertexPos_modelspace, 0.0f), viwInverse).xyz;

    // 3. 빌보드의 월드 위치로 이동
    float3 finalPos_worldspace = input.centerPos + vertexPos_aligned;

    // 4. 최종 화면 좌표로 변환
    output.pos_screenspace = mul(float4(finalPos_worldspace, 1.0f), viewProjectionMatrix);

    // UV 좌표 계산은 동일
    float2 uvCorners[4] = { float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 1) };
    output.tex = input.uvRect.xy + uvCorners[input.vertexId % 4] * input.uvRect.zw;

    return output;
}*/

// 정점 셰이더: Quad를 생성하는 핵심 로직
/*PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    float2 cornerOffsets[4] =
    {
        float2(-0.5f, 0.5f), 
        float2(0.5f, 0.5f), 
        float2(-0.5f, -0.5f), 
        float2(0.5f, -0.5f) 
    };
    uint cornerId = input.vertexId % 4;

    
    float2 offset = cornerOffsets[cornerId] * input.size.xy;
    float3 worldOffset = offset.x * cameraRight_worldspace + offset.y * cameraUp_worldspace;
    float3 finalPos_worldspace = input.centerPos + worldOffset;

    output.pos_screenspace = mul(float4(finalPos_worldspace, 1.0f), viewProjectionMatrix);
    float2 uvCorners[4] = { float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 1) };
    output.tex = input.uvRect.xy + uvCorners[cornerId] * input.uvRect.zw;

    return output;
}*/

float4 mainPS(PS_INPUT input) : SV_Target
{
    return fontAtlas.Sample(linearSampler, input.tex);
}