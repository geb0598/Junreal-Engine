
Texture2D FrameColor : register(t0);
SamplerState LinearSampler : register(s0);


//struct VS_Input
//{
//    float3 posModel : POSITION;
//    float2 uv : TEXCOORD0;
//};

struct PS_Input
{
    float4 posCS : SV_Position;
    float2 uv : TEXCOORD0;
};

PS_Input mainVS(uint Input : SV_VertexID)
{
     PS_Input o;
   
    float2 UVMap[] =
    {
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 0.0f),
        float2(1.0f, 1.0f),
    };

    o.uv = UVMap[Input];
    o.posCS = float4(o.uv.x * 2.0f - 1.0f, 1.0f - (o.uv.y * 2.0f), 0.0f, 1.0f);
    return o;
}


//Get Texture Info
//uint OutTexWidth, OutTexHeight, OutMipCount = 0;
//uint InMipLevel = 0;
//FrameColor.GetDimensions(InMipLevel, TexWidth, TexHeight, MipCount);

float GetFrameSample(float2 uv)
{
    float3 Lumaniance = float3(0.299f, 0.587f, 0.114f);
    return dot(FrameColor.Sample(LinearSampler, uv).rgb, Lumaniance);
}

float4 mainPS(PS_Input i) : SV_TARGET
{
    float3 Lumaniance = float3(0.299f, 0.587f, 0.114f);
    float ContrastThreshold = 0.0312f;
    float RelativeThreshold = 0.063f;
    float FXAAStrength = 1.0f;
    
    //Get Texture Info
    uint TexWidth, TexHeight, MipCount = 0;
    FrameColor.GetDimensions(0, TexWidth, TexHeight, MipCount);
    float2 TexSizeRCP = float2(1.0f / TexWidth, 1.0f / TexHeight);
    float2 uv = float2(i.posCS.x / TexWidth, i. posCS.y / TexHeight);
    
    float3 Color = FrameColor.Sample(LinearSampler, uv).rgb;
    //NearPixel Sample
    float M = dot(Color, Lumaniance);
    float N = GetFrameSample(uv + int2( 0,  1) * TexSizeRCP);
    float S = GetFrameSample(uv + int2( 0, -1) * TexSizeRCP);
    float E = GetFrameSample(uv + int2( 1,  0) * TexSizeRCP);
    float W = GetFrameSample(uv + int2(-1,  0) * TexSizeRCP);
    
    //Calc Contrast
    float lowest = min(M, min(N, min(S, min(E, W))));
    float heighest = max(M, max(N, max(S, max(E, W))));
    float Contrast = heighest - lowest;
    
    //Calc Threshold
    float Threshold = max(ContrastThreshold, RelativeThreshold * heighest);
    if(Contrast < Threshold)
    {
        //return float4(1, 1, 1, 1);
        discard;
    }
    
    
    //1 2 1
    //2 M 2
    //1 2 1
    float NE = GetFrameSample(uv + int2( 1,  1) * TexSizeRCP);
    float NW = GetFrameSample(uv + int2(-1,  1) * TexSizeRCP);
    float SE = GetFrameSample(uv + int2( 1, -1) * TexSizeRCP);
    float SW = GetFrameSample(uv + int2(-1, -1) * TexSizeRCP);
    
    float filter = (2 * (N + S + E + W) + (NE + NW + SE + SW)) / 12.0f;
    float BlendFactor = saturate(abs(filter - M) / Contrast);
    BlendFactor = smoothstep(0, 1, BlendFactor);
    
    float Horizontal = 2 * abs(N + S - 2 * M) + abs(NE + SE - 2 * E) + abs(NW + SW - 2 * W); 
    float Vertical = 2 * abs(E + W - 2 * M) + abs(NE + NW - 2 * N) + abs(SE + SW - 2 * S);
    bool bHorizontal = Horizontal > Vertical;
    
    float2 tempColor = bHorizontal ? float2(1, 0) : float2(0, 1);
    return float4(tempColor, Color.b, 1); //가로 r 세로 g

    
    float PLuminance = bHorizontal ? N : E; 
    float NLuminance = bHorizontal ? S : W; 
    float PGradient = abs(PLuminance - M);
    float NGradient = abs(NLuminance - M);
    
    float2 PixelUVStep = bHorizontal ? float2(0, TexSizeRCP.y) : float2(TexSizeRCP.x, 0);
    
    PixelUVStep = PGradient > NGradient ? PixelUVStep : -PixelUVStep;
    
    float2 BlendUV = uv + (bHorizontal ? float2(0, 1) : float2(1, 0)) * PixelUVStep * BlendFactor;
    float3 BlendColor = FrameColor.Sample(LinearSampler, BlendUV).rgb;
    
    return float4(Color.rgb, 1);
}

