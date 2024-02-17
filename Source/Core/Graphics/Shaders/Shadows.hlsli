
#include "PCSS.hlsli"

Texture2D directionalLightShadowMap : register(t14);

cbuffer ShadowBuffer : register(b7)
{
    bool usePCF;
    float offsetScalePCF;
    int sampleCountPCF;
    bool usePoisson;
    float offsetScalePoissonDisk;
}

static float2 POISSON_DISK_16[16] =
{
    float2(-0.94201624, -0.39906216),
    float2(0.94558609, -0.76890725),
    float2(-0.094184101, -0.92938870),
    float2(0.34495938, 0.29387760),
    float2(-0.91588581, 0.45771432),
    float2(-0.81544232, -0.87912464),
    float2(-0.38277543, 0.27676845),
    float2(0.97484398, 0.75648379),
    float2(0.44323325, -0.97511554),
    float2(0.53742981, -0.47373420),
    float2(-0.26496911, -0.41893023),
    float2(0.79197514, 0.19090188),
    float2(-0.24188840, 0.99706507),
    float2(-0.81409955, 0.91437590),
    float2(0.19984126, 0.78641367),
    float2(0.14383161, -0.14100790)
};

static float2 POISSON_DISK_64[64] =
{
    float2(-0.613392, 0.617481),
    float2(0.170019, -0.040254),
    float2(-0.299417, 0.791925),
    float2(0.645680, 0.493210),
    float2(-0.651784, 0.717887),
    float2(0.421003, 0.027070),
    float2(-0.817194, -0.271096),
    float2(-0.705374, -0.668203),
    float2(0.977050, -0.108615),
    float2(0.063326, 0.142369),
    float2(0.203528, 0.214331),
    float2(-0.667531, 0.326090),
    float2(-0.098422, -0.295755),
    float2(-0.885922, 0.215369),
    float2(0.566637, 0.605213),
    float2(0.039766, -0.396100),
    float2(0.751946, 0.453352),
    float2(0.078707, -0.715323),
    float2(-0.075838, -0.529344),
    float2(0.724479, -0.580798),
    float2(0.222999, -0.215125),
    float2(-0.467574, -0.405438),
    float2(-0.248268, -0.814753),
    float2(0.354411, -0.887570),
    float2(0.175817, 0.382366),
    float2(0.487472, -0.063082),
    float2(-0.084078, 0.898312),
    float2(0.488876, -0.783441),
    float2(0.470016, 0.217933),
    float2(-0.696890, -0.549791),
    float2(-0.149693, 0.605762),
    float2(0.034211, 0.979980),
    float2(0.503098, -0.308878),
    float2(-0.016205, -0.872921),
    float2(0.385784, -0.393902),
    float2(-0.146886, -0.859249),
    float2(0.643361, 0.164098),
    float2(0.634388, -0.049471),
    float2(-0.688894, 0.007843),
    float2(0.464034, -0.188818),
    float2(-0.440840, 0.137486),
    float2(0.364483, 0.511704),
    float2(0.034028, 0.325968),
    float2(0.099094, -0.308023),
    float2(0.693960, -0.366253),
    float2(0.678884, -0.204688),
    float2(0.001801, 0.780328),
    float2(0.145177, -0.898984),
    float2(0.062655, -0.611866),
    float2(0.315226, -0.604297),
    float2(-0.780145, 0.486251),
    float2(-0.371868, 0.882138),
    float2(0.200476, 0.494430),
    float2(-0.494552, -0.711051),
    float2(0.612476, 0.705252),
    float2(-0.578845, -0.768792),
    float2(-0.772454, -0.090976),
    float2(0.504440, 0.372295),
    float2(0.155736, 0.065157),
    float2(0.391522, 0.849605),
    float2(-0.620106, -0.328104),
    float2(0.789239, -0.419965),
    float2(-0.545396, 0.538133),
    float2(-0.178564, -0.596057),
};

float PoissonDisk(float3 aDirectionLightProjectedPosition)
{
    const float computedZ = aDirectionLightProjectedPosition.z;
    const float bias = 0.001f;

    float shadowFactor = 0.0f;

    const int sampleCount = 64;
    const float offsetScale = offsetScalePoissonDisk;

    for (int i = 0; i < sampleCount; ++i)
    {
        const float2 sampleOffset = POISSON_DISK_64[i];

    // Adjust sampleUV based on facing direction
        const float2 sampleUV = 0.5f + float2(0.5f, -0.5f) * (aDirectionLightProjectedPosition.xy) + sampleOffset * offsetScale;

        const float shadowMapZ = directionalLightShadowMap.Sample(shadowSplr, sampleUV);

        //shadowFactor -= (1.0f / 16.0f) * clamp(1.0f - shadowMapZ + bias, 0.0f, 1.0f);
        shadowFactor += (computedZ < shadowMapZ + bias) ? 1.0f : 0.0f;
    }

    return shadowFactor / (float)sampleCount;
}

float PCF(float3 aDirectionLightProjectedPosition)
{
    const float computedZ = aDirectionLightProjectedPosition.z;
    const float bias = 0.001f;
    
    float shadowFactor = 0.0f;

	// Filter kernel for PCF eg. (15x15)
    const int sampleCount = clamp(sampleCountPCF, 3, 25);
	// Offset scale decides how much the shadow edge is moved for "blurring"
    const float offsetScale = offsetScalePCF;

    for (int i = -sampleCount / 2; i <= sampleCount / 2; ++i)
    {
        for (int j = -sampleCount / 2; j <= sampleCount / 2; ++j)
        {
            const float2 sampleOffset = float2(i, j) / float(sampleCount);
            const float2 sampleUV = 0.5f + float2(0.5f, -0.5f) * (aDirectionLightProjectedPosition.xy + sampleOffset * offsetScale);
            const float shadowMapZ = directionalLightShadowMap.Sample(shadowSplr, sampleUV);

            shadowFactor += (computedZ < shadowMapZ + bias) ? 1.0f : 0.0f;
        }
    }

	// Average the results
    return shadowFactor / float(sampleCount * sampleCount);
}

float Shadow(const in float4x4 aCameraTransform, const in float4 aWorldPosition)
{
    const float4 directionalLightProjectedPositionTemp = mul(aCameraTransform, aWorldPosition);
    float3 directionLightProjectedPosition = directionalLightProjectedPositionTemp.xyz / directionalLightProjectedPositionTemp.w;

    float shadowFactor = 1.0f;
    if (clamp(directionLightProjectedPosition.x, -1.0f, 1.0f) == directionLightProjectedPosition.x &&
        clamp(directionLightProjectedPosition.y, -1.0f, 1.0f) == directionLightProjectedPosition.y)
    {
        float shadowFactorPCF = 0.0f;
        float shadowFactorPoisson = 0.0f;

        if (usePoisson)
        {
            shadowFactorPoisson = PoissonDisk(directionLightProjectedPosition);
            shadowFactor = shadowFactorPoisson;
        }
        if (usePCF)
        {
            shadowFactorPCF = PCF(directionLightProjectedPosition);
            shadowFactor = shadowFactorPCF;
        }
        if (usePoisson && usePCF)
        {
            shadowFactor = (shadowFactorPoisson + shadowFactorPCF) / 2.0f;
        }
    }

    return shadowFactor;
}