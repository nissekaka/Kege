#include "Light.hlsli"
//#include "ShaderOps.hlsl"
#include "PBRFunctions.hlsli"
#include "Shadows.hlsli"

static const uint MAX_LIGHTS = 50u; // Needs to be the same in PointLight

cbuffer ModelBuffer : register(b0)
{
    bool normalMapEnabled;
    bool materialEnabled;
    uint packedDataPointA;
    uint packedDataPointB;
    uint packedDataSpotA;
    uint packedDataSpotB;
};

cbuffer DirectionalLightBuffer : register(b1)
{
    float3 dLightDirection;
    float padding;
    float3 dLightColour;
    float ambientLightPower;
    float4x4 directionalLightCameraTransform;
};

cbuffer PointLightBuffer : register(b2)
{
    PointlightData plBuf[MAX_LIGHTS];
    uint activePointLights;
}

cbuffer SpotLightBuffer : register(b3)
{
    SpotlightData slBuf[MAX_LIGHTS];
    uint activeSpotLights;
}

struct PixelInput
{
    float3 worldPos : POSITION;
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitan : BITANGENT;
};

struct PixelOutput
{
    float4 colour : SV_TARGET0;
    float4 worldPosition : SV_TARGET1;
};

Texture2D colourTex : register(t2);
Texture2D normalTex : register(t3);
Texture2D materialTex : register(t4);

PixelOutput main(PixelInput aInput)
{
    PixelOutput output;
    output.worldPosition = float4(aInput.worldPos, 1.0f);

    // Unpack light data
    bool nearbyPointLights[MAX_LIGHTS];
    bool nearbySpotLights[MAX_LIGHTS];
    
    for (int pIndex = 0; pIndex < MAX_LIGHTS; ++pIndex)
    {
        if (pIndex < 32)
        {
            nearbyPointLights[pIndex] = (packedDataPointA & (1u << pIndex)) != 0;
        }
        else
        {
            nearbyPointLights[pIndex] = (packedDataPointB & (1u << (pIndex - 32))) != 0;
        }
    }
    
    for (int sIndex = 0; sIndex < MAX_LIGHTS; ++sIndex)
    {
        if (sIndex < 32)
        {
            nearbySpotLights[sIndex] = (packedDataSpotA & (1u << sIndex)) != 0;
        }
        else
        {
            nearbySpotLights[sIndex] = (packedDataSpotB & (1u << (sIndex - 32))) != 0;
        }
    }

    float3 colour = colourTex.Sample(splr, aInput.texCoord).rgb;
    float3 normal = normalTex.Sample(splr, aInput.texCoord).rgb;
    float3 material = materialTex.Sample(splr, aInput.texCoord).rgb;

    float3 ambientLight = { 0.0f, 0.0f, 0.0f };
    float3 directionalLight = { 0.0f, 0.0f, 0.0f };
    float3 pointLight = { 0.0f, 0.0f, 0.0f };
    float3 spotLight = { 0.0f, 0.0f, 0.0f };
    float3 specular = { 0.0f, 0.0f, 0.0f };
    float ambientOcclusion = 0.0f;

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
		normalize(aInput.normal.xyz)
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
        ambientOcclusion = material.r;
        roughness = material.g;
        metalness = material.b;
        //emissive = material.b;

        specular = lerp((float3) 0.04f, colour.rgb, metalness);
        colour = lerp((float3) 0.0f, colour.rgb, 1 - metalness);
    }

    const float3 toEye = normalize(cameraPosition.xyz - aInput.worldPos.xyz);

    // Shadows

    //const float shadowFactor = Shadow(directionalLightCameraTransform, float4(aInput.worldPos, 1.0f));

    // Lighting

	// Day/night cycle
    const float3 lightDir = normalize(dLightDirection);
    const float dotProduct = dot(lightDir, float3(0.0f, 1.0f, 0.0f));
    const float blendFactor = (dotProduct + 1.0) / 2.0f
;
    // Ambient light
    ambientLight = EvaluateAmbianceDynamicSky(splr, dayTex, nightTex, blendFactor,
    normal, aInput.normal.xyz, toEye, roughness, ambientOcclusion, colour, specular);

	// Directional light
    directionalLight = EvaluateDirectionalLight(colour, specular, normal,
    roughness, dLightColour, -dLightDirection, toEye);

    // Point lights
    for (uint i = 0; i < activePointLights; ++i)
    {
        if (!plBuf[i].active || !nearbyPointLights[i])
        {
            continue;
        }

        pointLight += EvaluatePointLight(colour, specular, normal,
        roughness, plBuf[i].colour, plBuf[i].intensity,
        plBuf[i].radius, plBuf[i].position, toEye, aInput.worldPos.xyz);
    }

	// Spot lights
    for (uint j = 0; j < activeSpotLights; ++j)
    {
        if (!slBuf[j].active || !nearbySpotLights[j])
        {
            continue;
        }

        spotLight += EvaluateSpotLight(colour, specular, normal, roughness, slBuf[j].colour, slBuf[j].intensity,
        slBuf[j].range, slBuf[j].position, slBuf[j].direction, slBuf[j].outerAngle, slBuf[j].innerAngle, toEye, aInput.worldPos.xyz);
    }

	// Final colour
    //const float3 emissiveColour = colour * emissive;
    const float3 finalColour = saturate(ambientLight * ambientLightPower + directionalLight /** shadowFactor*/ + pointLight + spotLight);

    output.colour = float4(finalColour, 1.0f);

    return output;
}
