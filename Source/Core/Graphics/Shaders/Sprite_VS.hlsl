cbuffer Transform : register(b0)
{
    matrix objectToWorld;
    matrix objectToClip;
}

struct VertexInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct PixelInput
{
    float3 worldPos : POSITION;
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

PixelInput main(const VertexInput aInput)
{
    PixelInput output;

    matrix identity = { 1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f };
    
    const float4 position = { aInput.position, 1.0f };
    output.worldPos = mul(objectToWorld, position).xyz;
    output.position = mul(objectToClip, position);
    output.texCoord = aInput.texCoord;

    return output;

}