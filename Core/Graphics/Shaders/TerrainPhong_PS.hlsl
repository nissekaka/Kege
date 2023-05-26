#include "Light.hlsli"
#include "ShaderOps.hlsl"

static const uint MAX_LIGHTS = 16u; // Needs to be the same in PointLight

cbuffer ModelBuffer : register(b0)
{
    bool normalMapEnabled;
    bool materialEnabled;
    float specularIntensity;
    float specularPower;
};

cbuffer DirectionalLightBuffer : register(b1)
{
    float3 dLightDirection;
    float padding;
    float3 dLightColour;
    float ambientLight;
};

cbuffer PointLightBuffer : register(b2)
{
    PointLightData plBuf[MAX_LIGHTS];
    uint activeLights;
}

struct PixelInput
{
    float3 worldPos : WPOSITION;
    float3 worldNorm : WNORMAL;
    float3 viewPos : POSITION;
    float4 position : SV_POSITION; // Not used
    float3 viewNormal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 viewTan : TANGENT;
    float3 viewBitan : BITANGENT;
    matrix modelView : MODELVIEW;
};

Texture2D albedoTexGrass : register(t0);
Texture2D normalTexGrass : register(t1);

Texture2D albedoTexDirt : register(t2);
Texture2D normalTexDirt : register(t3);

Texture2D albedoTexRock : register(t4);
Texture2D normalTexRock : register(t5);

Texture2D albedoTexSnow : register(t6);
Texture2D normalTexSnow : register(t7);

SamplerState splr;

float4 main(PixelInput aInput) : SV_TARGET
{
    const float3 groundColour = { 0.9f, 0.7f, 0.1f };
    const float3 skyColour = { 0.6f, 0.6f, 0.8f };

    const float3 grassColour = albedoTexGrass.Sample(splr, aInput.texCoord).rgb;
    const float3 grassNormal = normalTexGrass.Sample(splr, aInput.texCoord).wyz /** 2.0f - 1.0f*/;

    const float3 dirtColour = albedoTexDirt.Sample(splr, aInput.texCoord).rgb;
    const float3 dirtNormal = normalTexDirt.Sample(splr, aInput.texCoord).wyz /** 2.0f - 1.0f*/;

    const float3 rockColour = albedoTexRock.Sample(splr, aInput.texCoord).rgb;
    const float3 rockNormal = normalTexRock.Sample(splr, aInput.texCoord).wyz /** 2.0f - 1.0f*/;

    const float3 snowColour = albedoTexSnow.Sample(splr, aInput.texCoord).rgb;
    const float3 snowNormal = normalTexSnow.Sample(splr, aInput.texCoord).wyz/* * 2.0f - 1.0f*/;

    const float slopeBlend = smoothstep(0.6f, 0.8f, aInput.worldNorm.y);
    const float lowBlend = smoothstep(-30.0f, -5.0f, aInput.worldPos.y);
    const float heightBlend = smoothstep(5.0f, 25.0f, aInput.worldPos.y);

    const float3 colour = lerp(rockColour, lerp(lerp(grassColour, dirtColour, lowBlend), snowColour, heightBlend), slopeBlend).rgb;
    const float3 normal = lerp(rockNormal, lerp(lerp(grassNormal, dirtNormal, lowBlend), snowNormal, heightBlend), slopeBlend);

	// Sample normal from map if normal mapping enabled
    if (normalMapEnabled)
    {
        const float3 mappedNormal = MapNormal(normalize(aInput.viewTan), normalize(aInput.viewBitan), aInput.viewNormal, aInput.texCoord, normal, splr);
        aInput.viewNormal = lerp(aInput.viewNormal, mappedNormal, 1.0f);
    }

    float3 combinedLight = { 0, 0, 0 };
    float3 specular = { 0, 0, 0 };

    // Point lights
    for (uint i = 0; i < activeLights; ++i)
    {
        if (!plBuf[i].active)
        {
            continue;
        }

    	// Fragment to light vector data
        const float3 vToL = plBuf[i].pLightPosition - aInput.viewPos;
        const float distToL = length(vToL);
        const float3 dirToL = vToL / distToL;

		// Attenuation
        const float att = Attenuate(plBuf[i].attConst, plBuf[i].attLin, plBuf[i].attQuad, distToL);
		// Diffuse
        combinedLight += Diffuse(plBuf[i].pLightColour, plBuf[i].pLightIntensity, att, dirToL, aInput.viewNormal);;
		// Specular reflected
        specular += Speculate(plBuf[i].pLightColour * plBuf[i].pLightIntensity,
        specularIntensity, aInput.viewNormal,
        vToL, aInput.viewPos, att, specularPower);
    }

    // Directional light
    const float3 viewDirectionalLightDir = mul(dLightDirection, (float3x3) aInput.modelView);
    combinedLight += max(0, dot(aInput.viewNormal, -viewDirectionalLightDir)) * dLightColour;
    specular += Speculate(dLightColour,
        specularIntensity, aInput.viewNormal,
        -viewDirectionalLightDir, aInput.viewPos, 1.0f, specularPower);

    // Ambient light
    const float3 twoDirAmbientLight = ambientLight * ((0.5f + 0.5f * aInput.viewNormal.y) * skyColour + (0.5f - 0.5f * aInput.viewNormal.y) * groundColour);

	// Final color
    return float4(saturate((combinedLight + twoDirAmbientLight) * colour + specular), 1.0f);
}