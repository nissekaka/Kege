cbuffer CBuf
{
	matrix transform;
};

struct VSOut
{
	float2 tex : TEXCOORD;
	float4 pos : SV_POSITION;
};

VSOut main(float3 aPos : POSITION, float2 aTexCoord : TEXCOORD)
{
	VSOut vso;
	vso.pos = mul(float4(aPos, 1.0f), transform);
	vso.tex = aTexCoord;
	return vso;
}