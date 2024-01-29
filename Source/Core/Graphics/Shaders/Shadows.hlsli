SamplerState shadowSplr : register(s1);
Texture2D directionalLightShadowMap : register(t14);

float Shadow(const in float4x4 aCameraTransform, const in float4 aWorldPosition)
{
    const float4 directionalLightProjectedPositionTemp = mul(aCameraTransform, aWorldPosition);
    float3 directionLightProjectedPosition = directionalLightProjectedPositionTemp.xyz / directionalLightProjectedPositionTemp.w;

    float shadowFactor = 1.0f;
    if (clamp(directionLightProjectedPosition.x, -1.0f, 1.0f) == directionLightProjectedPosition.x &&
        clamp(directionLightProjectedPosition.y, -1.0f, 1.0f) == directionLightProjectedPosition.y)
    {
        const float computedZ = directionLightProjectedPosition.z;
        const float bias = 0.001f;

        float totalFactor = 0.0f;

        // Filter kernel for PCF eg. (15x15)
        const int numSamples = 15;
        // Offset scale decides how much the shadow edge is moved for "blurring"
        const float offsetScale = 0.004f;

        for (int i = -numSamples / 2; i <= numSamples / 2; ++i)
        {
            for (int j = -numSamples / 2; j <= numSamples / 2; ++j)
            {
                const float2 sampleOffset = float2(i, j) / float(numSamples);
                const float2 sampleUV = 0.5f + float2(0.5f, -0.5f) * (directionLightProjectedPosition.xy + sampleOffset * offsetScale);
                const float shadowMapZ = directionalLightShadowMap.Sample(shadowSplr, sampleUV);

                totalFactor += (computedZ < shadowMapZ + bias) ? 1.0f : 0.0f;
            }
        }

        // Average the results
        shadowFactor = totalFactor / float(numSamples * numSamples);
    }

    return shadowFactor;
}