#include "common.hlsli"
#include "deferred_common.hlsli"
#include "GBuffer.hlsli"

struct PixelInput
{
    float3 worldPos : POSITION;
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldNormal : WORLDNORMAL;
    float3 tangent : TANGENT;
    float3 bitan : BITANGENT;
};

GBufferOutput main(PixelInput aInput)
{
    float4 albedo = gColourTex.Sample(defaultSampler, aInput.texCoord).rgba;

    if (albedo.a < 0.5f)
    {
        discard;
    }

    float3 normal = float3(gNormalTex.Sample(linearSampler, aInput.texCoord).rg, 1.0f);
    //float3 normal = normalTex.Sample(defaultSampler, scaledUV).wyz;
    //float ambientOcclusion = normal.z;
    normal = 2.0f * normal - 1.0f;
    normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
    normal = normalize(normal);
    normal.y = -normal.y;

    float3x3 TBN = float3x3(normalize(aInput.tangent.xyz),
                            normalize(-aInput.bitan.xyz),
                            normalize(aInput.normal.xyz));
    TBN = transpose(TBN);

    const float3 pixelNormal = normalize(mul(TBN, normal));

    float4 material = gMaterialTex.Sample(linearSampler, aInput.texCoord);
    float ambientOcclusion = material.r;

    GBufferOutput output;
    output.worldPosition = float4(aInput.worldPos, 1.0f);
    output.albedo = float4(albedo.rgb, 1.0f);
    output.normal = float4(0.5f + 0.5f * pixelNormal, 1.0f);
    output.material = float4(material.rgb, 1.0f);
    output.ambientOcclusionAndCustom = float4(ambientOcclusion, aInput.worldNormal); // gba are unused, put whatever data you want here!
    output.ambientOcclusionAndCustom.g = 0.0f;

    //output.rsm = float4(0.0f, 0.0f, 0.0f, 1.0f);
    return output;
}

