cbuffer Transform : register(b0)
{
    matrix objectToWorld;
    matrix objectToClip;
}

cbuffer Reflection : register(b1)
{
    float planeHeight;
}

struct VertexInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tan : TANGENT;
    float3 bitan : BITANGENT;
};

struct PixelInput
{
    float3 worldPos : POSITION;
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitan : BITANGENT;
    float clipDist : SV_CLIPDISTANCE0;
};

PixelInput main(const VertexInput aInput)
{
    PixelInput output;
    
    const float4 position = { aInput.position, 1.0f };
    output.worldPos = mul(objectToWorld, aInput.position);
    output.position = mul(objectToClip, position);
    output.texCoord = aInput.texCoord;
    output.normal = aInput.normal;
    output.tangent = aInput.tan;
    output.bitan = aInput.bitan;
    output.clipDist = aInput.position.y - planeHeight;
    
    return output;
}