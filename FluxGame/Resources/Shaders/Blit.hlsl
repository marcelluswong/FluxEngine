#include "Uniforms.hlsl"
#include "Samplers.hlsl"

struct PS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

void VSMain(uint vertexId : SV_VertexID, out PS_OUTPUT output)
{
	output.position.x = (float)(vertexId / 2) * 4.0f - 1.0f;
	output.position.y = (float)(vertexId % 2) * 4.0f - 1.0f;
	output.position.z = 0.0f;
	output.position.w = 1.0f;

	output.texCoord.x = (float)(vertexId / 2) * 2.0f;
	output.texCoord.y = 1.0f - (float)(vertexId % 2) * 2.0f;
}

void PSMain(PS_OUTPUT input, out float4 output : SV_TARGET)
{
	output = float4(tDiffuseTexture.Sample(sDiffuseSampler, input.texCoord).rgb, 1.0f);
}
