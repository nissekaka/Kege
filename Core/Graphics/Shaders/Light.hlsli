struct PointLightData
{
    float3 pLightPosition;
    float padding1;
    float3 pLightColour;
    float padding2;
    float pLightIntensity;
    float attConst;
    float attLin;
    float attQuad;
    bool active;
    float3 padding3;
};
