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
    float4 position : SV_POSITION;
    float3 viewNormal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 viewTan : TANGENT;
    float3 viewBitan : BITANGENT;
    matrix modelView : MODELVIEW;
};

Texture2D albedoTex;
Texture2D normalTex;
Texture2D materialTex;

SamplerState splr;


float4 main(PixelInput aInput) : SV_TARGET
{
    const float3 colour = albedoTex.Sample(splr, aInput.texCoord).rgb;
    //const float3 groundColour = { 0.9f, 0.7f, 0.1f };
    //const float3 skyColour = { 0.6f, 0.6f, 0.8f };
    const float3 groundColour = { 0.0f, 0.5f, 1.0f };
    const float3 skyColour = { 1.0f, 0.5f, 0.0f };

	// Sample normal from map if normal mapping enabled
    if (normalMapEnabled)
    {
        const float3 mappedNormal = MapNormal(normalize(aInput.viewTan), normalize(aInput.viewBitan), aInput.viewNormal, aInput.texCoord, normalTex, splr);
        aInput.viewNormal = lerp(aInput.viewNormal, mappedNormal, 1.0f);
    }

  //  if (normalMapEnabled)
  //  {
  //      const float3x3 tanBitanNorm = float3x3(normalize(aInput.viewTan), normalize(aInput.viewBitan), normalize(aInput.viewNormal));

  //  	// Sample the normal map texture
  //      float3 normal = normalTex.Sample(splr, aInput.texCoord).wyz * 2.0f - 1.0f;
  //      normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
  //      normal = normalize(normal);

		//// Transform the normal map value into tangent space
  //      aInput.viewNormal = mul(normal, tanBitanNorm);
  //  }

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

    //for (uint i = 0; i < activeLights; ++i)
    //{
    //    if (!plBuf[i].active)
    //    {
    //        continue;
    //    }

    //	// Fragment to light vector data
    //    const float3 vToL = plBuf[i].pLightPosition - aInput.viewPos;
    //    const float distToL = length(vToL);
    //    const float3 dirToL = vToL / distToL;
    //	// Attenuation
    //    const float att = 1.0f / (plBuf[i].attConst + plBuf[i].attLin * distToL + plBuf[i].attQuad * (distToL * distToL));

    //    combinedLight += max(0.0f, dot(dirToL, aInput.viewNormal)) * plBuf[i].pLightColour * plBuf[i].pLightIntensity;

    //	// Reflected light vector
    //    const float3 w = aInput.viewNormal * dot(vToL, aInput.viewNormal);
    //    const float3 r = w * 2.0f - vToL;

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
    //}

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