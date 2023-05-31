struct PointLightData
{
    float3 pLightPosition;
    float padding1;
    float3 pLightColour;
    float padding2;
    float pLightIntensity;
    float radius;
    float falloff;
    float padding3;
    bool active;
    float3 padding4;
};
