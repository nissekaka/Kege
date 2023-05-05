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

    const float3 vToL = pLightPosition - aInput.viewPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    	// Attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));

    const float3 combinedLight = max(0, dot(aInput.normal, -dLightDirection)) * dLightColour + att * max(0.0f, dot(dirToL, aInput.normal)) * pLightColour * pLightIntensity;


	// Use the transformed normal for lighting calculations
    const float4 textureColour = albedoTex.Sample(splr, aInput.texCoord);
    
    return float4(saturate(combinedLight + ambientLight), 1.0f) * textureColour;
}