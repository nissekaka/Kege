#include "Light.hlsli"
//#include "ShaderOps.hlsl"
#include "PBRFunctions.hlsli"
#include "PostprocessTonemapPS.hlsl"

static const uint MAX_LIGHTS = 50u; // Needs to be the same in PointLight

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
    float ambientLightPower;
};

cbuffer PointLightBuffer : register(b2)
{
    PointLightData plBuf[MAX_LIGHTS];
    uint activeLights;
}

cbuffer Reflection : register(b11)
{
    float planeHeight;
}

struct PixelInput
{
    float3 worldPos : WPOSITION;
    float3 worldNorm : WNORMAL;
    float3 viewPos : POSITION;
    float4 position : SV_POSITION;
    float3 viewNormal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 tangent : TANGENT;
    float3 bitan : BITANGENT;
    matrix modelView : MODELVIEW;
};

Texture2D albTexGrass : register(t2);
Texture2D nrmTexGrass : register(t3);
Texture2D matTexGrass : register(t4);

Texture2D albTexRock : register(t5);
Texture2D nrmTexRock : register(t6);
Texture2D matTexRock : register(t7);

Texture2D albTexSnow : register(t8);
Texture2D nrmTexSnow : register(t9);
Texture2D matTexSnow : register(t10);

float4 main(PixelInput aInput) : SV_TARGET
{
    const float3 grassColour = albTexGrass.Sample(splr, aInput.texCoord).rgb;
    const float3 grassNormal = nrmTexGrass.Sample(splr, aInput.texCoord).wyz;
    const float3 grassMaterial = matTexGrass.Sample(splr, aInput.texCoord).rgb;

    const float3 rockColour = albTexRock.Sample(splr, aInput.texCoord).rgb;
    const float3 rockNormal = nrmTexRock.Sample(splr, aInput.texCoord).wyz;
    const float3 rockMaterial = matTexRock.Sample(splr, aInput.texCoord).rgb;

    const float3 snowColour = albTexSnow.Sample(splr, aInput.texCoord).rgb;
    const float3 snowNormal = nrmTexSnow.Sample(splr, aInput.texCoord).wyz;
    const float3 snowMaterial = matTexSnow.Sample(splr, aInput.texCoord).rgb;

    const float slopeBlend = smoothstep(0.6f, 0.8f, aInput.worldNorm.y);
    const float heightBlend = smoothstep(5.0f, 25.0f, aInput.worldPos.y);

    float3 colour = lerp(rockColour, lerp(grassColour, snowColour, heightBlend), slopeBlend).rgb;
    float3 normal = lerp(rockNormal, lerp(grassNormal, snowNormal, heightBlend), slopeBlend);
    const float3 material = lerp(rockMaterial, lerp(grassMaterial, snowMaterial, heightBlend), slopeBlend);

    const float waterBlend = smoothstep(-5.0f, 5.0f, aInput.worldPos.y - planeHeight);

    const float3 beachColour = float3(0.25f, 0.16f, 0.07f);
    //const float3 beachColour = float3(1.0f, 0.85f, 0.5f);
    //const float3 beachColour = float3(0.6f, 1.0f, 1.0f);
    colour = lerp(beachColour, colour, waterBlend);

    if (cameraPosition.y <= planeHeight + 0.1f)
    {
        const float dist = sqrt((aInput.worldPos.x - cameraPosition.x) * (aInput.worldPos.x - cameraPosition.x) + (aInput.worldPos.y - cameraPosition.y) * (aInput.worldPos.y - cameraPosition.y) + (aInput.worldPos.z - cameraPosition.z) * (aInput.worldPos.z - cameraPosition.z));

        const float viewBlend = smoothstep(30.0f, 75.0f, dist);
        const float3 darkWater = float3(colour.x * 0.01f, colour.y * 0.02f, colour.z * 0.1f);
        const float3 normWater = float3(colour.x * 0.2f, colour.y * 0.7f, colour.z);

        colour = lerp(normWater, darkWater, viewBlend);
    }

    float3 ambientLight = { 0.0f, 0.0f, 0.0f };
    float3 directionalLight = { 0.0f, 0.0f, 0.0f };
    float3 pointLight = { 0.0f, 0.0f, 0.0f };
    float3 specular = { 0.0f, 0.0f, 0.0f };
    const float ambientOcclusion = normal.b;

    float metalness = 0.0f;
    float roughness = 0.0f;
    float emissive = 0.0f;
	
    if (normalMapEnabled)
    {
        normal = 2.0f * normal - 1.0f;
        normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
        normal = normalize(normal);

        float3x3 TBN = float3x3(
		normalize(aInput.tangent.xyz),
		normalize(-aInput.bitan.xyz),
		normalize(aInput.worldNorm.xyz)
		);

	    // Can save an instruction here by instead doing
	    // normalize(mul(normal, TBN)); It works because
	    // TBN is a 3x3 and therefore TBN^T is the same
	    // as TBN^-1. However, it is considered good form to do this.
        TBN = transpose(TBN);
        normal = normalize(mul(TBN, normal));
    }

    if (materialEnabled)
    {
        metalness = material.r;
        roughness = material.g;
        emissive = material.b;

        specular = lerp((float3) 0.04f, colour.rgb, metalness);
        colour = lerp((float3) 0.0f, colour.rgb, 1 - metalness);
    }

    const float3 toEye = normalize(cameraPosition - aInput.worldPos);

    // Lighting

	// Day/night cycle
    const float3 lightDir = normalize(dLightDirection);
    const float dotProduct = dot(lightDir, float3(0.0f, 1.0f, 0.0f));
    const float blendFactor = (dotProduct + 1.0) / 2.0f;

    // Ambient light
    ambientLight = EvaluateAmbianceDynamicSky(splr, dayTex, nightTex, blendFactor,
    normal, aInput.worldNorm.xyz, toEye, roughness, ambientOcclusion, colour, specular);

	// Directional light
    directionalLight = EvaluateDirectionalLight(colour, specular, normal,
    roughness, dLightColour, -dLightDirection, toEye);

    // Point lights
    for (uint i = 0; i < activeLights; ++i)
    {
        if (!plBuf[i].active)
        {
            continue;
        }

        pointLight += EvaluatePointLight(colour, specular, normal,
        roughness, plBuf[i].pLightColour, plBuf[i].pLightIntensity,
        plBuf[i].radius, plBuf[i].pLightPosition, toEye, aInput.viewPos);
    }

	// Final colour
    const float3 emissiveColour = colour * emissive;
    const float3 finalColour = saturate(ambientLight * ambientLightPower + directionalLight + pointLight + emissiveColour);
    // Tonemap
    return float4(tonemap_s_gamut3_cine(finalColour), 1.0f);

}