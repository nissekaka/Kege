cbuffer Transform : register(b0)
{
    matrix model;
    matrix modelViewProj;
}

struct VertexInput
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD;
    float4 normal : NORMAL;
    float4 tan : TANGENT;
    float4 bitan : BITANGENT;
};

struct PixelInput
{
    float4 worldPos : POSITION;
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 bitan : BITANGENT;
};

PixelInput main(const VertexInput aInput)
{
    PixelInput output;

    output.position = aInput.position;
    output.worldPos = aInput.position;
    
    output.texCoord = aInput.texCoord;

    output.normal = aInput.normal;
    output.tangent = aInput.tan;
    output.bitan = aInput.bitan;
    
    return output;
}