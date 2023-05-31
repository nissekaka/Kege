cbuffer Transform : register(b0)
{
    matrix modelView;
    matrix modelViewProj;
};

cbuffer SkeletonBuffer : register(b1)
{
    float4x4 boneTransforms[64u]; // Array of bone transforms
};

struct VertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 tan : TANGENT;
    float3 bitan : BITANGENT;
    uint4 boneIndices : BLENDINDICES;
    float4 boneWeights : BLENDWEIGHT;
};

struct PixelInput
{
    float3 worldPos : WPOSITION;
    float3 worldNorm : WNORMAL;
    float3 viewPos : POSITION;
    float4 position : SV_POSITION;
    float3 viewNormal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 viewTan : TANGENT;
    float3 viewBitan : BITANGENT;
    matrix modelView : MODELVIEW;
};

PixelInput main(VertexInput aInput)
{
    PixelInput output;

	// Accumulate bone transformations based on bone indices and weights
    matrix finalTransform;
    for (int i = 0; i < 4; ++i)
    {
        uint boneIndex = aInput.boneIndices[i];
        float boneWeight = aInput.boneWeights[i];

		// Fetch the bone transform from the skeleton buffer
        matrix boneTransform = boneTransforms[boneIndex];

        // Accumulate the transformed contribution from each bone
        finalTransform += boneWeight * boneTransform;
    }

    // Apply the final bone transform to the vertex position
    const float4 worldPosition = mul(finalTransform, float4(aInput.position, 1.0f));

    output.viewPos = (float3) mul(worldPosition, modelView);

    output.worldPos = aInput.position;
    output.worldNorm = aInput.normal;
    output.viewNormal = mul(aInput.normal, (float3x3) modelView);
    output.viewTan = mul(aInput.tan, (float3x3) modelView);
    output.viewBitan = mul(aInput.bitan, (float3x3) modelView);
    output.position = mul(worldPosition, modelViewProj);
    output.texCoord = aInput.texCoord;
    output.modelView = modelView;
    
    return output;
}