#include "Light.hlsli"
//#include "ShaderOps.hlsl"
#include "PBRFunctions.hlsli"
#include "PostprocessTonemapPS.hlsl"

static const uint MAX_LIGHTS = 50u; // Needs to be the same in PointLight

cbuffer ModelBuffer : register(b0)
{
    bool normalMapEnabled;
    bool materialEnabled;
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
    uint activePointLights;
}

cbuffer SpotLightBuffer : register(b3)
{
    SpotLightData slBuf[MAX_LIGHTS];
    uint activeSpotLights;
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

Texture2D albedoTex : register(t2);
Texture2D normalTex : register(t3);
Texture2D materialTex : register(t4);


float4 main(PixelInput aInput) : SV_TARGET
{
    float3 colour = albedoTex.Sample(splr, aInput.texCoord).rgb;
    float3 normal = normalTex.Sample(splr, aInput.texCoord).wyz;
    float3 material = materialTex.Sample(splr, aInput.texCoord).rgb;

    float3 ambientLight = { 0.0f, 0.0f, 0.0f };
    float3 directionalLight = { 0.0f, 0.0f, 0.0f };
    float3 pointLight = { 0.0f, 0.0f, 0.0f };
    float3 spotLight = { 0.0f, 0.0f, 0.0f };
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

    const float3 toEye = normalize(cameraPosition.xyz - aInput.worldPos);

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
    for (uint i = 0; i < activePointLights; ++i)
    {
        if (!plBuf[i].active)
        {
            continue;
        }

        pointLight += EvaluatePointLight(colour, specular, normal,
        roughness, plBuf[i].colour, plBuf[i].intensity,
        plBuf[i].radius, plBuf[i].position, toEye, aInput.viewPos);
    }

	// Spot lights
    for (uint y = 0; y < activeSpotLights; ++y)
    {
        if (!slBuf[y].active)
        {
            continue;
        }

        spotLight += EvaluateSpotLight(colour, specular, normal, roughness, slBuf[y].colour, slBuf[y].intensity,
        slBuf[y].range, slBuf[y].position, slBuf[y].direction, slBuf[y].outerAngle, slBuf[y].innerAngle, toEye, aInput.viewPos);
    }

	// Final colour
    const float3 emissiveColour = colour * emissive;
    const float3 finalColour = saturate(ambientLight * ambientLightPower + directionalLight + pointLight + spotLight + emissiveColour);
    // Tonemap
    return float4(tonemap_s_gamut3_cine(finalColour), 1.0f);

}