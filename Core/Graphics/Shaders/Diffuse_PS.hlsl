Texture2D albedo;

SamplerState diffuseSampler;

float4 main(float2 aTexCoord : TEXCOORD) : SV_TARGET
{
    return albedo.Sample(diffuseSampler, aTexCoord);
}