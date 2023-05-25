struct PixelInput
{
    float3 worldPosition : POSITION;
    float4 position : SV_POSITION;
};

TextureCube skyboxTex : register(t0);

SamplerState splr : register(s0);

float4 main(PixelInput aInput) : SV_TARGET
{
    return skyboxTex.Sample(splr, aInput.worldPosition);
}