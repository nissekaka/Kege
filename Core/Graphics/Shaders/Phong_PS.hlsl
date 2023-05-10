#include "Light.hlsli"

static const uint MAX_LIGHTS = 128; // Needs to be the same in PointLight

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
    float3 viewPos : POSITION;
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 tan : TANGENT;
    float3 bitan : BITANGENT;
};

Texture2D albedoTex;
Texture2D normalTex;
Texture2D materialTex;

SamplerState splr;


float4 main(PixelInput aInput) : SV_TARGET
{
    // sample normal from map if normal mapping enabled
    if (normalMapEnabled)
    {
        const float3x3 tanBitanNorm = float3x3(normalize(aInput.tan), normalize(aInput.bitan), normalize(aInput.normal));

    	// Sample the normal map texture
        float3 normal = normalTex.Sample(splr, aInput.texCoord).wyz * 2.0f - 1.0f;
        normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
        normal = normalize(normal);

		// Transform the normal map value into tangent space
        aInput.normal = mul(normal, tanBitanNorm);
    }

    float3 combinedLight = { 0, 0, 0 };
    float3 specular = { 0, 0, 0 };

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
        const float att = 1.0f / (plBuf[i].attConst + plBuf[i].attLin * distToL + plBuf[i].attQuad * (distToL * distToL));

        combinedLight += max(0.0f, dot(dirToL, aInput.normal)) * plBuf[i].pLightColour * plBuf[i].pLightIntensity;

    	// Reflected light vector
        const float3 w = aInput.normal * dot(vToL, aInput.normal);
        const float3 r = w * 2.0f - vToL;

        if (materialEnabled)
        {
    		// Sample the material texture
            const float3 material = materialTex.Sample(splr, aInput.texCoord).xyz;

            float metalness = material.r;
            float roughness = material.g;
            float emissive = material.b;

    		// Calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
            specular += att * (plBuf[i].pLightColour * plBuf[i].pLightIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(aInput.viewPos))), specularPower);
        }
        else
        {
    		// Calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
            specular += att * (plBuf[i].pLightColour * plBuf[i].pLightIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(aInput.viewPos))), specularPower);
        }
    }

    combinedLight += max(0, dot(aInput.normal, -dLightDirection)) * dLightColour;

	// Final color
    return float4(saturate((combinedLight + ambientLight) * albedoTex.Sample(splr, aInput.texCoord).rgb + specular), 1.0f);
}