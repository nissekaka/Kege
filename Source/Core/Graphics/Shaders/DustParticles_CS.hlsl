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
    float travelRadius;
    float travelSpeed;
    float travelAngle;
    float fadeSpeed;
    float alpha;
};

RWStructuredBuffer<ParticleData> particleData : register(u0);
RWStructuredBuffer<InstanceData> instanceData : register(u1);

// Main compute shader function
[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint updateIndex = DTid.x;

    ParticleData particle = particleData[updateIndex];

    // Update particle properties
    particle.travelAngle -= particle.travelSpeed * deltaTime;

    instanceData[updateIndex].instanceTransform[3][0] = particle.travelRadius * cos(particle.travelAngle) + particle.startPosition.x;
    instanceData[updateIndex].instanceTransform[3][2] = particle.travelRadius * sin(particle.travelAngle) + particle.startPosition.z;

    if (particle.travelAngle > 2 * PI)
    {
        particle.travelAngle -= 2 * PI;
    }

    instanceData[updateIndex].colour.w = clamp(cos(elapsedTime + particle.fadeSpeed) * 0.5 + 0.5, 0.0, particle.alpha);

    // Update ParticleData
    particleData[updateIndex] = particle;
}