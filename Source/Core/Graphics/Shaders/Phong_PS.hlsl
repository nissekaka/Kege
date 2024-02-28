#include "Light.hlsli"
#include "ShaderOps.hlsli"

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
    PointlightData plBuf[MAX_LIGHTS];
    uint activeLights;
}

struct PixelInput
{
    float3 worldPos : WPOSITION;
    float3 worldNorm : WNORMAL;
    float3 viewPos : POSITION;
    float4 position : SV_POSITION;
    float3 viewNormal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 viewTan : TANGENT;
    float3 viewBitan : BITANGENT;
    matrix modelView : MODELVIEW;
};

Texture2D colourTex : register (t0);
Texture2D normalTex : register(t1);
Texture2D materialTex : register(t2);

SamplerState defaultSampler;


float4 main(PixelInput aInput) : SV_TARGET
{
    const float3 colour = colourTex.Sample(defaultSampler, aInput.texCoord).rgb;
    //const float3 groundColour = { 0.9f, 0.7f, 0.1f };
    //const float3 skyColour = { 0.6f, 0.6f, 0.8f };
    const float3 groundColour = { 0.0f, 0.5f, 1.0f };
    const float3 skyColour = { 1.0f, 0.5f, 0.0f };

	// Sample normal from map if normal mapping enabled
    if (normalMapEnabled)
    {
        const float3 mappedNormal = MapNormal(normalize(aInput.viewTan), normalize(aInput.viewBitan), aInput.viewNormal, aInput.texCoord, normalTex, defaultSampler);
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
        const float3 vToL = plBuf[i].position - aInput.viewPos;
        const float distToL = length(vToL);
        const float3 dirToL = vToL / distToL;

		// Attenuation
        const float att = Attenuate(plBuf[i].radius, plBuf[i].falloff, distToL);
		// Diffuse
        combinedLight += Diffuse(plBuf[i].colour, plBuf[i].intensity, att, dirToL, aInput.viewNormal);;
		// Specular reflected
        specular += Speculate(plBuf[i].colour * plBuf[i].intensity,
        specularIntensity, aInput.viewNormal,
        vToL, aInput.viewPos, att, specularPower);
    }

    //    if (materialEnabled)
    //    {
    //		// Sample the material texture
    //        const float3 material = materialTex.Sample(splr, aInput.texCoord).xyz;

    //        float metalness = material.r;
    //        float roughness = material.g;
    //        float emissive = material.b;

    //		// Calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    //        specular += att * (plBuf[i].pLightColour * plBuf[i].pLightIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(aInput.viewPos))), specularPower);
    //    }
    //    else
    //    {
    //		// Calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    //        specular += att * (plBuf[i].pLightColour * plBuf[i].pLightIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(aInput.viewPos))), specularPower);
    //    }

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