cbuffer Transform : register(b0)
{
    float4x4 transform;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
}

cbuffer DirectionalLightBuffer : register(b1)
{
    float3 lightDirection;
    float padding1;
    float3 lightColour;
    float ambientLight;
};

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
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 tan : TANGENT;
    float3 bitan : BITANGENT;
};

PixelInput main(VertexInput aInput)
{
    PixelInput output;

    // Transform the vertex position to world space
    output.position = mul(float4(aInput.position, 1.0f), transform);
    
    // Transform the normal to world space and normalize it
    output.normal = normalize(mul(aInput.normal, (float3x3) transform));
    output.texCoord = aInput.texCoord;
    output.tan = aInput.tan;
    output.bitan = aInput.bitan;

    // Transform the vertex position to clip space
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    return output;
}