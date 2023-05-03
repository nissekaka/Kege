Texture2D diffuseTexture;

SamplerState diffuseSampler;

float4 main(float2 aTexCoord : TEXCOORD) : SV_TARGET
{
    return diffuseTexture.Sample(diffuseSampler, aTexCoord);
}