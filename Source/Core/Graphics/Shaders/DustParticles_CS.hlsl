#define PI (3.141592653)

cbuffer ParticleConstants : register(b0)
{
    float4 cameraForward;
    float4 cameraRight;
    float4 cameraUp;
    float deltaTime;
    float elapsedTime;
};

struct InstanceData
{
    matrix instanceTransform;
    float4 colour;
};

struct ParticleData
{
    float3 startPosition;
    float travelAngle;
    float4 colour;
    float travelRadius;
    float travelSpeed;
    float fadeSpeed;
    float padding;
};

RWStructuredBuffer<InstanceData> instanceData : register(u0);
RWStructuredBuffer<ParticleData> particleData : register(u1);

// Main compute shader function
[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	const uint updateIndex = DTid.x;

    // Update particle properties
    particleData[updateIndex].travelAngle -= particleData[updateIndex].travelSpeed * deltaTime;

    matrix transform = transpose(instanceData[updateIndex].instanceTransform);

    transform[3][0] = particleData[updateIndex].travelRadius * cos(particleData[updateIndex].travelAngle) + particleData[updateIndex].startPosition.x;
    transform[3][1] = particleData[updateIndex].travelRadius * sin(particleData[updateIndex].travelAngle) + particleData[updateIndex].startPosition.y;
    transform[3][2] = particleData[updateIndex].travelRadius * sin(particleData[updateIndex].travelAngle) + particleData[updateIndex].startPosition.z;
    transform[0] = cameraRight;
    transform[1] = cameraForward;
    transform[2] = cameraUp;

    instanceData[updateIndex].instanceTransform = transpose(transform);

    if (particleData[updateIndex].travelAngle > 2 * PI)
    {
        particleData[updateIndex].travelAngle -= 2 * PI;
    }

    instanceData[updateIndex].colour.w = clamp(cos(elapsedTime + particleData[updateIndex].fadeSpeed) * 0.5 + 0.5, 0.0, particleData[updateIndex].colour.w);
}