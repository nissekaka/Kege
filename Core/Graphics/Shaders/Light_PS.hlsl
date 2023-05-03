cbuffer DirectionalLightBuffer : register(b1)
{
    float3 lightDirection;
    float padding1;
    float3 lightColour;
    float ambientLight;
};

cbuffer ModelBuffer : register(b2)
{
    bool normalMapEnabled;
    bool materialEnabled;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 tan : TANGENT;
    float3 bitan : BITANGENT;
};

Texture2D diffuseTexture;
Texture2D normalTexture;
Texture2D materialTexture;

SamplerState splr;

float4 main(PixelInput aInput) : SV_TARGET
{
    if (normalMapEnabled)
    {
        const float3x3 tanBitanNorm = float3x3(normalize(aInput.tan), normalize(aInput.bitan), normalize(aInput.normal));

    	// Sample the normal map texture
        float3 normal = normalTexture.Sample(splr, aInput.texCoord).wyz * 2.0f - 1.0f;
        normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
        normal = normalize(normal);

		// Transform the normal map value into tangent space
        aInput.normal = mul(normal, tanBitanNorm);
    }

	// Use the transformed normal for lighting calculations
    const float diffuseFactor = max(0, dot(aInput.normal, -lightDirection));
    const float4 textureColour = diffuseTexture.Sample(splr, aInput.texCoord);
    float3 specularColour = {0.0f, 0.0f, 0.0f};
	float3 diffuseColour = diffuseFactor * lightColour;

    if (materialEnabled)
    {
		// Sample the material texture
        const float3 material = materialTexture.Sample(splr, aInput.texCoord).xyz;

        float metalness = material.r;
        float roughness = material.g;
        float emissive = material.b;

        specularColour = lerp((float3) 0.04f, diffuseColour.rgb, metalness);
        diffuseColour = lerp((float3) 0.00f, diffuseColour.rgb, 1 - metalness);
        diffuseColour += specularColour;
    }

    return float4(saturate(diffuseColour + ambientLight), 1.0f) * textureColour;
}