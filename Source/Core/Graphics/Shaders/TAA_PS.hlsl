#include "deferred_common.hlsli"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

cbuffer Parameters : register(b1)
{
    uint mode = 0;
    float denoise = 3.0f;
};

Texture2D currentFrameTexture : register(t0);
Texture2D lastFrameTexture : register(t1);

// YUV-RGB conversion routine from Hyper3D
float3 encodePalYuv(float3 rgb)
{
    rgb = pow(rgb, float3(2.0, 2.0, 2.0)); // gamma correction
    return float3(
        dot(rgb, float3(0.299, 0.587, 0.114)),
        dot(rgb, float3(-0.14713, -0.28886, 0.436)),
        dot(rgb, float3(0.615, -0.51499, -0.10001))
    );
}

float3 decodePalYuv(float3 yuv)
{
    float3 rgb = float3(
        dot(yuv, float3(1., 0., 1.13983)),
        dot(yuv, float3(1., -0.39465, -0.58060)),
        dot(yuv, float3(1., 2.03211, 0.))
    );
    return pow(rgb, float3(0.5, 0.5, 0.5)); // gamma correction
}

float4 main(const PixelInput aInput) : SV_TARGET
{

 //   float2 offset[25];
 //   offset[0] = float2(-2, -2);
 //   offset[1] = float2(-1, -2);
 //   offset[2] = float2(0, -2);
 //   offset[3] = float2(1, -2);
 //   offset[4] = float2(2, -2);
    
 //   offset[5] = float2(-2, -1);
 //   offset[6] = float2(-1, -1);
 //   offset[7] = float2(0, -1);
 //   offset[8] = float2(1, -1);
 //   offset[9] = float2(2, -1);
    
 //   offset[10] = float2(-2, 0);
 //   offset[11] = float2(-1, 0);
 //   offset[12] = float2(0, 0);
 //   offset[13] = float2(1, 0);
 //   offset[14] = float2(2, 0);
    
 //   offset[15] = float2(-2, 1);
 //   offset[16] = float2(-1, 1);
 //   offset[17] = float2(0, 1);
 //   offset[18] = float2(1, 1);
 //   offset[19] = float2(2, 1);
    
 //   offset[20] = float2(-2, 2);
 //   offset[21] = float2(-1, 2);
 //   offset[22] = float2(0, 2);
 //   offset[23] = float2(1, 2);
 //   offset[24] = float2(2, 2);
    
    
 //   float kernel[25];
 //   kernel[0] = 1.0f / 256.0f;
 //   kernel[1] = 1.0f / 64.0f;
 //   kernel[2] = 3.0f / 128.0f;
 //   kernel[3] = 1.0f / 64.0f;
 //   kernel[4] = 1.0f / 256.0f;
    
 //   kernel[5] = 1.0f / 64.0f;
 //   kernel[6] = 1.0f / 16.0f;
 //   kernel[7] = 3.0f / 32.0f;
 //   kernel[8] = 1.0f / 16.0f;
 //   kernel[9] = 1.0f / 64.0f;
    
 //   kernel[10] = 3.0f / 128.0f;
 //   kernel[11] = 3.0f / 32.0f;
 //   kernel[12] = 9.0f / 64.0f;
 //   kernel[13] = 3.0f / 32.0f;
 //   kernel[14] = 3.0f / 128.0f;
    
 //   kernel[15] = 1.0f / 64.0f;
 //   kernel[16] = 1.0f / 16.0f;
 //   kernel[17] = 3.0f / 32.0f;
 //   kernel[18] = 1.0f / 16.0f;
 //   kernel[19] = 1.0f / 64.0f;
    
 //   kernel[20] = 1.0f / 256.0f;
 //   kernel[21] = 1.0f / 64.0f;
 //   kernel[22] = 3.0f / 128.0f;
 //   kernel[23] = 1.0f / 64.0f;
 //   kernel[24] = 1.0f / 256.0f;
    
 //   float4 sum = float4(0,0,0,0);
 //   float c_phi = 1.0;
 //   float n_phi = 0.5;
 //   //float p_phi = 0.3;
 //   float4 cval = lastFrameTexture.Sample(linearSampler, aInput.texCoord);
 //   float4 nval = currentFrameTexture.Sample(linearSampler, aInput.texCoord);
	////float4 pval = texelFetch(iChannel2, ifloat2(fragCoord), 0);
    
 //   float cum_w = 0.0;
 //   for (int i = 0; i < 25; i++)
 //   {
 //       float2 uv = aInput.texCoord + offset[i] * denoise;
        
 //       float4 ctmp = lastFrameTexture.Sample(linearSampler, uv);
 //       float4 t = cval - ctmp;
 //       float dist2 = dot(t, t);
 //       float c_w = min(exp(-(dist2) / c_phi), 1.0);
        
 //       float4 ntmp = currentFrameTexture.Sample(linearSampler, uv);
 //       t = nval - ntmp;
 //       dist2 = max(dot(t, t), 0.0);
 //       float n_w = min(exp(-(dist2) / n_phi), 1.0);
        
 //       //float4 ptmp = texelFetch(iChannel2, ifloat2(uv), 0);
 //       //t = pval - ptmp;
 //       //dist2 = dot(t,t);
 //       //float p_w = min(exp(-(dist2)/p_phi), 1.0);
        
 //       //float weight = c_w*n_w*p_w;
 //       float weight = c_w * n_w;
 //       sum += ctmp * weight * kernel[i];
 //       cum_w += weight * kernel[i];
 //   }

 //   float4 result = sum / cum_w;
 //   return result;
    float2 uv = aInput.position.xy / clientResolution.xy;

    float4 lastColour = lastFrameTexture.Sample(linearSampler, uv).rgba;

    if (mode == 2)
    {
        //return float4(1, 0, 0, 1);
        return lastColour;
    }

    float3 antialiased = lastColour.rgb;
    float mixRate = min(length(lastColour.rgb), 0.5f);

    float2 off = float2(1.0f / clientResolution.x, 1.0f / clientResolution.y);
    float3 in0 = currentFrameTexture.Sample(linearSampler, uv).rgb;

    if (mode == 1)
    {
        //return float4(0, 1, 0, 1);
        return float4(in0, 1.0);
    }
    antialiased = lerp(antialiased * antialiased, in0 * in0, mixRate);
    antialiased = sqrt(antialiased);

    float3 minColour = antialiased;
    float3 maxColour = antialiased;

    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            float2 offset = float2(i, j) * off;
            float3 sample = currentFrameTexture.Sample(linearSampler, uv + offset).rgb;
            minColour = min(minColour, sample);
            maxColour = max(maxColour, sample);
        }
    }
    //return float4(0, 0, 1, 1);

    //antialiased = lerp(antialiased * antialiased, in0 * in0, mixRate);
    //antialiased = sqrt(antialiased);

    //float3 in1 = currentFrameTexture.Sample(linearSampler, uv + float2(+off.x, 0.0)).xyz;
    //float3 in2 = currentFrameTexture.Sample(linearSampler, uv + float2(-off.x, 0.0)).xyz;
    //float3 in3 = currentFrameTexture.Sample(linearSampler, uv + float2(0.0, +off.y)).xyz;
    //float3 in4 = currentFrameTexture.Sample(linearSampler, uv + float2(0.0, -off.y)).xyz;
    //float3 in5 = currentFrameTexture.Sample(linearSampler, uv + float2(+off.x, +off.y)).xyz;
    //float3 in6 = currentFrameTexture.Sample(linearSampler, uv + float2(-off.x, +off.y)).xyz;
    //float3 in7 = currentFrameTexture.Sample(linearSampler, uv + float2(+off.x, -off.y)).xyz;
    //float3 in8 = currentFrameTexture.Sample(linearSampler, uv + float2(-off.x, -off.y)).xyz;

    //antialiased = encodePalYuv(antialiased);
    //in0 = encodePalYuv(in0);
    //in1 = encodePalYuv(in1);
    //in2 = encodePalYuv(in2);
    //in3 = encodePalYuv(in3);
    //in4 = encodePalYuv(in4);
    //in5 = encodePalYuv(in5);
    //in6 = encodePalYuv(in6);
    //in7 = encodePalYuv(in7);
    //in8 = encodePalYuv(in8);

    //float3 minColour = min(min(min(in0, in1), min(in2, in3)), in4);
    //float3 maxColour = max(max(max(in0, in1), max(in2, in3)), in4);
    //minColour = lerp(minColour, min(min(min(in5, in6), min(in7, in8)), minColour), 0.5);
    //maxColour = lerp(maxColour, max(max(max(in5, in6), max(in7, in8)), maxColour), 0.5);
    
    const float3 preclamping = antialiased;
    antialiased = clamp(antialiased, minColour, maxColour);
    
    mixRate = 1.0 / (1.0 / mixRate + 1.0);
    
    const float3 diff = antialiased - preclamping;
    const float clampAmount = dot(diff, diff);
    
    mixRate += clampAmount * 4.0;
    mixRate = clamp(mixRate, 0.05, 0.5);
    
    antialiased = decodePalYuv(antialiased);

    return float4(antialiased, mixRate);
}

//TAA

//float3 encodePalYuv(float3 rgb)
//{
//    return float3(
//        dot(rgb, float3(0.299, 0.587, 0.114)),
//        dot(rgb, float3(-0.14713, -0.28886, 0.436)),
//        dot(rgb, float3(0.615, -0.51499, -0.10001))
//    );
//}

//float3 decodePalYuv(float3 yuv)
//{
//    return float3(
//        dot(yuv, float3(1., 0., 1.13983)),
//        dot(yuv, float3(1., -0.39465, -0.58060)),
//        dot(yuv, float3(1., 2.03211, 0.))
//    );
//}

//float3 mul3(in float3x3 m, in float3 v)
//{
//    return float3(dot(v, m[0]), dot(v, m[1]), dot(v, m[2]));
//}

//float3 mul3(in float3 v, in float3x3 m)
//{
//    return mul3(m, v);
//}

//float3 srgb2oklab(float3 c)
//{
    
//    float3x3 m1 = float3x3(
//        0.4122214708, 0.5363325363, 0.0514459929,
//        0.2119034982, 0.6806995451, 0.1073969566,
//        0.0883024619, 0.2817188376, 0.6299787005
//    );
    
//    float3 lms = mul3(m1, c);
    
//    lms = pow(lms, float3(1. / 3.));

//    float3x3 m2 = float3x3(
//        +0.2104542553, +0.7936177850, -0.0040720468,
//        +1.9779984951, -2.4285922050, +0.4505937099,
//        +0.0259040371, +0.7827717662, -0.8086757660
//    );
    
//    return mul3(m2, lms);
//}

//float3 oklab2srgb(float3 c)
//{
//    float3x3 m1 = float3x3(
//        1.0000000000, +0.3963377774, +0.2158037573,
//        1.0000000000, -0.1055613458, -0.0638541728,
//        1.0000000000, -0.0894841775, -1.2914855480
//    );

//    float3 lms = mul3(m1, c);
    
//    lms = lms * lms * lms;
  
//    float3x3 m2 = float3x3(
//        +4.0767416621, -3.3077115913, +0.2309699292,
//        -1.2684380046, +2.6097574011, -0.3413193965,
//        -0.0041960863, -0.7034186147, +1.7076147010
//    );
//    return mul3(m2, lms);
//}

//float3 enc_color(float3 x)
//{
//    return srgb2oklab(x);
//}

//float3 dec_color(float3 x)
//{
//    return oklab2srgb(x);
//}

//void mainImage(out float4 c, in float2 p)
//{
//    rng_initialize(p, iFrame);
//    load_scene(iChannel2, iTime, iResolution.xy);
//    float2 jitter = halton(iFrame % 16) - 0.5;
//    float4 bufB = texture(iChannel0, (p - jitter) / iResolution.xy);
    
//    float4 col = float4(bufB.xyz, 1.);
    
//    float2 uv = (p - 0.5 * iResolution.xy) / iResolution.y;
//    float4 ro = float4(campos, bufB.w);
//    float3 rd = normalize(cam * float3(1, FOV * uv));
//    ro.xyz += ro.w * rd;
//    float4 X = ro;
//    material mat = getMaterial(X);
//    ro.xyz -= mat.velocity;
    
//    float3 reprj = reproject(pcam, pcampos, pResolution.xy, ro.xyz);
//    float2 puv = reprj.xy / iResolution.xy;
//    float2 dpuv = abs(puv - float2(0.5));

//    float3 prev_col = texture_Bicubic(iChannel1, puv).xyz;
    
//    //neighborhood clamping
//    float3 minc = float3(1e10);
//    float3 maxc = float3(0.);
//    for (int i = -NEIGHBOR_CLAMP_RADIUS; i < NEIGHBOR_CLAMP_RADIUS; i++)
//        for (int j = -NEIGHBOR_CLAMP_RADIUS; j < NEIGHBOR_CLAMP_RADIUS; j++)
//        {
//            float3 pix = enc_color(texelFetch(iChannel0, ifloat2(p) + ifloat2(i, j), 0).xyz);
//            minc = min(pix, minc);
//            maxc = max(pix, maxc);
//        }
    
//    float3 preclamp = enc_color(prev_col);
//    prev_col = clamp(preclamp, minc, maxc);
//    float delta = distance(prev_col, preclamp);
//    prev_col = dec_color(prev_col);
     
    
//    float2 v = decode(texelFetch(iChannel1, ifloat2(puv * iResolution.xy), 0).w);
    
//    float4 prev = float4(prev_col, 1.0) * v.y;
//    float prev_td = 2.0 / v.x;
    
//    float3 prev_pos = normalize(ro.xyz - pcampos) * prev_td + pcampos;
//    float ang_distance = distance(normalize(prev_pos - campos), normalize(ro.xyz - campos));
    
//    if (iFrame < 2)
//        prev *= 0.0;
//    //prev*=mix(1.0, smoothstep(0.6, 0.5, delta), 0.1);
//    prev *= mix(1.0, step(ang_distance, DISOCCLUSION_REJECTION), DISOCCLUSION_REJECTION_STR);
//    float dist = distance(prev.xyz / prev.w, col.xyz / col.w);
//    prev *= mix(1.0, smoothstep(CAMERA_MOVEMENT_REJECTION, 0., distance(campos, pcampos)), 0.05);
    

//    //prev*=mix(1.0, smoothstep(0.7, 0.6, dist),1.0);
//    col += prev * REPROJECTION * step(dpuv.x, 0.5) * step(dpuv.y, 0.5);
    
//    c.xyz = 1. * col.xyz / col.w + 0. * ang_distance;
//    c.w = encode(float2(2.0 / ro.w, col.w));
//}