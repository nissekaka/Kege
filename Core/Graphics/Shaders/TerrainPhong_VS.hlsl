cbuffer Transform : register(b0)
{
    matrix modelView;
    matrix modelViewProj;
}

struct VertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 tan : TANGENT;
    float3 bitan : BITANGENT;
};

struct PixelInput
{
    float3 worldPos : WPOSITION;
    float3 worldNorm : WNORMAL;
    float3 viewPos : POSITION;
    float4 position : SV_POSITION;
    float3 viewNormal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 tangent : TANGENT;
    float3 bitan : BITANGENT;
    matrix modelView : MODELVIEW;
};

PixelInput main(VertexInput aInput)
{
    PixelInput output;

    output.worldPos = aInput.position;
    output.worldNorm = aInput.normal;
    output.viewPos = (float3) mul(float4(aInput.position, 1.0f), modelView);
    output.viewNormal = mul(aInput.normal, (float3x3) modelView);
    output.tangent = aInput.tan;
    output.bitan = aInput.bitan;
    output.position = mul(float4(aInput.position, 1.0f), modelViewProj);
    output.texCoord = aInput.texCoord;
    output.modelView = modelView;
    
    return output;
}