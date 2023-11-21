cbuffer Transform : register(b0)
{
    matrix objectToWorld;
    matrix objectToClip;
}

cbuffer SkeletonBuffer : register(b1)
{
    float4x4 boneTransforms[64u]; // Array of bone transforms
};

struct VertexInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tan : TANGENT;
    float3 bitan : BITANGENT;
    uint4 boneIndices : BONEINDICES;
    float4 boneWeights : BONEWEIGHT;
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

    	// Accumulate bone transformations based on bone indices and weights
    matrix finalTransform =
    {
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 }
    };

    for (int i = 0; i < 4; ++i)
    {
        uint boneIndex = aInput.boneIndices[i];
        float boneWeight = aInput.boneWeights[3-i];

		// Fetch the bone transform from the skeleton buffer
        matrix boneTransform = boneTransforms[boneIndex];

        // Accumulate the transformed contribution from each bone
        finalTransform += boneWeight * boneTransform;
    }

	// Apply the final bone transform to the vertex position
    const float4 worldPosition = mul(finalTransform, float4(aInput.position, 1.0f));

    const float3x3 objectToWorldRotation = objectToWorld;
    const float4 position = worldPosition;
    output.worldPos = mul(objectToWorld, position).xyz;
    output.position = mul(objectToClip, position);
    output.texCoord = aInput.texCoord;
    output.normal = mul(objectToWorldRotation, aInput.normal);
    output.tangent = mul(objectToWorldRotation, aInput.tan);
    output.bitan = mul(objectToWorldRotation, aInput.bitan);
    
    return output;
}