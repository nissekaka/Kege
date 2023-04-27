cbuffer DirectionalLightBuffer : register(b1)
{
    float3 lightDirection;
    float padding1;
    float3 lightColour;
    float ambientLight;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

Texture2D diffuseTexture : register (t0);
Texture2D normalTexture : register (t1);

SamplerState splr : register(s0);

float4 main(PixelInput aInput) : SV_TARGET
{
    // Calculate the amount of light reflected by the surface
    const float diffuseFactor = max(0, dot(aInput.normal, -lightDirection));

    const float4 textureColour = diffuseTexture.Sample(splr, aInput.texCoord);

    // Combine the diffuse factor with the light color
    const float3 diffuseColour = diffuseFactor * lightColour;

    // Output the final color
    return float4(saturate(diffuseColour + ambientLight), 1.0f) * textureColour;
}