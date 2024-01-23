#include "common.hlsli"

Texture2D colourTex : register(t2);

struct PixelInput
{
    float3 worldPos : POSITION;
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitan : BITANGENT;
};

void main(PixelInput aInput)
{
	const float alpha = colourTex.Sample(splr, aInput.texCoord).a;
    if (alpha < 0.5f)
    {
        discard;
    }
}