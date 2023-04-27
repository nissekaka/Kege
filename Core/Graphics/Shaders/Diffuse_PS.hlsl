Texture2D diffuseTexture;

SamplerState splr;

float4 main(float2 aTexCoord : TEXCOORD) : SV_TARGET
{
    return diffuseTexture.Sample(splr, aTexCoord);
}