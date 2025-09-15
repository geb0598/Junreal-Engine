// C++에서 상수 버퍼를 통해 전달될 데이터
cbuffer CameraInfo : register(b0)
{
    row_major matrix viewProjectionMatrix;
    float3 cameraRight_worldspace;
    float3 cameraUp_worldspace;
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

// 정점 셰이더: Quad를 생성하는 핵심 로직
PS_INPUT mainVS(VS_INPUT input)
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

    // 2. 단위 오프셋에 실제 문자 크기를 곱하여 2D 오프셋 계산
    float2 offset = cornerOffsets[cornerId] * input.size.xy;

    // 3. [빌보드 계산] 2D 오프셋을 카메라 방향에 맞춰 3D 오프셋으로 변환
    // 수정된 코드
    float3 worldOffset = offset.x * cameraRight_worldspace + offset.y * cameraUp_worldspace;

    // 4. [최종 위치 생성] 입력받은 '월드 좌표 중심점'에 3D 오프셋을 더함
    float3 finalPos_worldspace = input.centerPos + worldOffset;

    // 5. 생성된 월드 좌표를 최종 화면 좌표로 변환
    output.pos_screenspace = mul(float4(finalPos_worldspace, 1.0f), viewProjectionMatrix);

    // 6. UV 좌표 계산
    float2 uvCorners[4] = { float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 1) };
    output.tex = input.uvRect.xy + uvCorners[cornerId] * input.uvRect.zw;

    return output;
}

// 픽셀 셰이더: 텍스처에서 색상 추출
float4 mainPS(PS_INPUT input) : SV_Target
{
    // 정점 셰이더가 계산해준 UV 좌표로 폰트 아틀라스 텍스처를 샘플링
    return fontAtlas.Sample(linearSampler, input.tex);
}