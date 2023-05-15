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
    float3 viewPos : POSITION;
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 tan : TANGENT;
    float3 bitan : BITANGENT;
};

PixelInput main(VertexInput aInput)
{
    PixelInput output;

    output.viewPos = (float3) mul(float4(aInput.position, 1.0f), modelView);
    output.normal = mul(aInput.normal, (float3x3) modelView);
    output.tan = mul(aInput.tan, (float3x3) modelView);
    output.bitan = mul(aInput.bitan, (float3x3) modelView);
    output.position = mul(float4(aInput.position, 1.0f), modelViewProj);
    output.texCoord = aInput.texCoord;
    
    return output;
}