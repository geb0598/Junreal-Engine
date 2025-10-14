#include "PostProcessing.hlsli"

float3 Lumaniance = { 0.299f, 0.587f, 0.114 };
float ContrastThreshold = 0.0312f;
float RelativeThreshold = 0.063f;
//Get Texture Info
//uint OutTexWidth, OutTexHeight, OutMipCount = 0;
//uint InMipLevel = 0;
//FrameColor.GetDimensions(InMipLevel, TexWidth, TexHeight, MipCount);

float GetFrameSample(float2 uv)
{
    return dot(FrameColor.Sample(PointSampler, uv))
}

float4 mainPS(PS_Input i) : SV_TARGET
{
    //Get Texture Info
    uint TexWidth, TexHeight, MipCount = 0;
    FrameColor.GetDimensions(0, TexWidth, TexHeight, MipCount);
    float2 TexSizeRCP = float2(1 / (float) TexWidth, 1 / (float) TexHeight);
    
    //NearPixel Sample
    float M = GetFrameSample(i.uv + int2( 0,  0) * TexSizeRCP);
    float N = GetFrameSample(i.uv + int2( 0,  1) * TexSizeRCP);
    float S = GetFrameSample(i.uv + int2( 0, -1) * TexSizeRCP);
    float E = GetFrameSample(i.uv + int2( 1,  0) * TexSizeRCP);
    float W = GetFrameSample(i.uv + int2(-1,  0) * TexSizeRCP);
    
    //Calc Contrast
    float lowest = min(M, min(N, min(S, min(E, W))));
    float heighest = max(M, max(N, max(S, max(E, W))));
    float Contrast = heighest - lowest;
    
    //Calc Threshold
    float Threshold = max(ContrastThreshold, RelativeThreshold * heighest);
    if(Contrast < Threshold)
    {
        discard;
    }
    
    
    //1 2 1
    //2 M 2
    //1 2 1
    float NE = GetFrameSample(i.uv + int2( 1,  1) * TexSizeRCP);
    float NW = GetFrameSample(i.uv + int2(-1,  1) * TexSizeRCP);
    float SE = GetFrameSample(i.uv + int2( 1, -1) * TexSizeRCP);
    float SW = GetFrameSample(i.uv + int2(-1, -1) * TexSizeRCP);
    
    float filter = (2 * (N + S + E + W) + (NE + NW + SE + SW)) / 12.0f;
    
    float BlendFactor = saturate(abs(filter - M) / Contrast);
    
    BlendFactor = smoothstep(0, 1, BlendFactor);
    
    
}




//#version 420

//in
//vec2 texCoord0;

//out
//vec4 color;

//uniform sampler2D scene;
//uniform vec3 inverseFilterTextureSize;

//void main(void)
//{
//    float fxaaSpanMax = 8.0f;
//    float fxaaReduceMin = 1.0f / 128.0f;
//    float fxaaReduceMul = 1.0f / 8.0f;

//    vec2 texCoordOffset = inverseFilterTextureSize.xy;

//    vec3 luma = vec3(0.299, 0.587, 0.114);
//    float lumaTL = dot(luma, 
//    texture2D( scene, texCoord0.
//    xy + (vec2(-1.0, 1.0) * texCoordOffset)).
//    xyz);
//    float lumaTR = dot(luma, 
//    texture2D( scene, texCoord0.
//    xy + (vec2(1.0, 1.0) * texCoordOffset)).
//    xyz);
//    float lumaBL = dot(luma, 
//    texture2D( scene, texCoord0.
//    xy + (vec2(-1.0, -1.0) * texCoordOffset)).
//    xyz);
//    float lumaBR = dot(luma, 
//    texture2D( scene, texCoord0.
//    xy + (vec2(1.0, -1.0) * texCoordOffset)).
//    xyz);
//    float lumaM = dot(luma, 
//    texture2D( scene, texCoord0.
//    xy).
//    xyz);

//    vec2 dir;
//    dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
//    dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));
	
//    float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (fxaaReduceMul * 0.25), fxaaReduceMin);
//    float inverseDirAdjustment = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
	
//    dir = min(vec2(fxaaSpanMax, fxaaSpanMax),
//		max(vec2(-fxaaSpanMax, -fxaaSpanMax), dir * inverseDirAdjustment));
	
//    dir.x = dir.x * step(1.0, abs(dir.x));
//    dir.y = dir.y * step(1.0, abs(dir.y));
	
	
//    dir = dir * texCoordOffset;

//    vec3 result1 = (1.0 / 2.0) * (

//    texture2D( scene, texCoord0.
//    xy + (dir * vec2(1.0 / 3.0 - 0.5))).
//    xyz +

//    texture2D( scene, texCoord0.
//    xy + (dir * vec2(2.0 / 3.0 - 0.5))).
//    xyz);

//    vec3 result2 = result1 * (1.0 / 2.0) + (1.0 / 4.0) * (

//    texture2D( scene, texCoord0.
//    xy + (dir * vec2(0.0 / 3.0 - 0.5))).
//    xyz +

//    texture2D( scene, texCoord0.
//    xy + (dir * vec2(3.0 / 3.0 - 0.5))).
//    xyz);

//    float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
//    float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
//    float lumaResult2 = dot(luma, result2);
	
//    if (lumaResult2 < lumaMin || lumaResult2 > lumaMax)
//        color = vec4(result1, 1.0);
//    else
//        color = vec4(result2, 1.0);
		
	
	
//}