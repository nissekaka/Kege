cbuffer Transform : register(b0)
{
    matrix objectToWorld;
    matrix objectToClip;
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
};

PixelInput main(const VertexInput aInput)
{
    PixelInput output;

    const float3x3 objectRotationMatrix = objectToWorld;
    const float4 position = { aInput.position, 1.0f };
    output.worldPos = mul(objectToWorld, position).xyz;
    output.position = mul(objectToClip, position);
    output.texCoord = aInput.texCoord;
    output.normal = mul(objectRotationMatrix, aInput.normal);
    output.tangent = mul(objectRotationMatrix, aInput.tan);
    output.bitan = mul(objectRotationMatrix, aInput.bitan);
    
    return output;
}