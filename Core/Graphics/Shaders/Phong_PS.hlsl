cbuffer DirectionalLightBuffer : register(b1)
{
    float3 dLightDirection;
    float padding;
    float3 dLightColour;
    float ambientLight;
};

cbuffer PointLightBuffer : register(b2)
{
    float3 pLightPosition;
    float padding1;
    float3 pLightColour;
    float padding2;
    float pLightIntensity;
    float attConst;
    float attLin;
    float attQuad;
}

cbuffer ModelBuffer : register(b3)
{
    bool normalMapEnabled;
    bool materialEnabled;
    float specularIntensity;
    float specularPower;
};

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

	// Fragment to light vector data
    const float3 vToL = pLightPosition - aInput.viewPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
	// Attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));

    const float3 combinedLight = max(0, dot(aInput.normal, -dLightDirection)) * dLightColour + att * max(0.0f, dot(dirToL, aInput.normal)) * pLightColour * pLightIntensity;

	// Reflected light vector
    const float3 w = aInput.normal * dot(vToL, aInput.normal);
    const float3 r = w * 2.0f - vToL;

    float3 specular;
    if (materialEnabled)
    {
		// Sample the material texture
        const float3 material = materialTex.Sample(splr, aInput.texCoord).xyz;

        float metalness = material.r;
        float roughness = material.g;
        float emissive = material.b;

    	// Calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
        specular = att * (pLightColour * pLightIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(aInput.viewPos))), specularPower);
    }
    else
    {
	    // Calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
        specular = att * (pLightColour * pLightIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(aInput.viewPos))), specularPower);
    }
	// Final color
    return float4(saturate((combinedLight + ambientLight) * albedoTex.Sample(splr, aInput.texCoord).rgb + specular), 1.0f);
}