#include "LightHelper.fx"

Texture2D g_txDiffuse;
TextureCube g_CubeMap;
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

cbuffer cbNeverChanges
{
	Material			gMaterial;
	DirectionalLight	DirLight;
};

cbuffer cbChangesEveryFrame
{
	float3 EyePos;
	matrix World;
	matrix WorldInvTranspose;
	matrix WorldViewProj;
};


//------------------------------------------------------------------------
struct VS_INPUT_1
{
	float3 Pos : POSITION;
};

struct VS_INPUT_2
{
	float3 Pos : POSITION;
	float3 Nor : NORMAL;
};

struct VS_INPUT_3
{
	float3 Pos : POSITION;
	float3 Nor : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VS_OUTPUT_2
{
	float4 Pos : SV_POSITION;
	float3 PosW : POSITION;
};

struct VS_OUTPUT_4
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 PosW : POSITION;
	float3 NorW : NORMAL;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT_4 VS(VS_INPUT_3 input)
{
	VS_OUTPUT_4 output = (VS_OUTPUT_4)0;
	output.Pos = mul(float4(input.Pos, 1.0f), WorldViewProj);
	output.Tex = input.Tex;
	output.PosW = mul(float4(input.Pos, 1.0f), World).xyz;
	output.NorW = mul(input.Nor, (float3x3)WorldInvTranspose);

	return output;
}

// Nor ´æ´¢ÑÕÉ«
VS_OUTPUT_2 VS_SIMPLE(VS_INPUT_3 input)
{
	VS_OUTPUT_2 output = (VS_OUTPUT_2)0;
	output.Pos = mul(float4(input.Pos, 1.0f), WorldViewProj);
	output.PosW = input.Nor;

	return output;
}

VS_OUTPUT_2 VS_SKY(VS_INPUT_1 input)
{
	VS_OUTPUT_2 output = (VS_OUTPUT_2)0;

	// Set z = w so that z/w = 1 (i.e., skydome always on far plane).
	output.Pos = mul(float4(input.Pos, 1.0f), WorldViewProj).xyww;

	// Use local vertex position as cubemap lookup vector.
	output.PosW = input.Pos;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT_4 input) : SV_TARGET
{
	float3 toEyeW = normalize(EyePos - input.PosW);

	// Start with a sum of zero. 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Sum the light contribution from each light source.
	float4 A, D, S;

	ComputeDirectionalLight(gMaterial, DirLight, input.NorW, toEyeW, A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;

	float4 outColor = g_txDiffuse.Sample(samLinear, input.Tex) * ( ambient + diffuse ) + spec ;

	// Common to take alpha from diffuse material.
	outColor.a = gMaterial.Diffuse.a;

	return outColor;
}

float4 PS_SIMPLE(VS_OUTPUT_2 input) : SV_TARGET
{

	float4 outColor = float4(input.PosW, 1.0f);

	// Common to take alpha from diffuse material.
	outColor.a = gMaterial.Diffuse.a;

	return outColor;
}

float4 PS_SKY(VS_OUTPUT_2 input) : SV_Target
{
	return g_CubeMap.Sample(samLinear, input.PosW);
}


//--------------------------------------------------------------------------------------
// Setting
//--------------------------------------------------------------------------------------
RasterizerState NoCull
{
	CullMode = None;
};

DepthStencilState LessEqualDSS
{
	// Make sure the depth function is LESS_EQUAL and not just LESS.  
	// Otherwise, the normalized depth values at z = 1 (NDC) will 
	// fail the depth test if the depth buffer was cleared to 1.
	DepthFunc = LESS_EQUAL;
};


//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 RenderSnowman
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
	pass P1
	{
		SetVertexShader(CompileShader(vs_4_0, VS_SIMPLE()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS_SIMPLE()));
	}
}

technique11 RenderHills
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}

technique11 RenderSky
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS_SKY()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS_SKY()));

		SetRasterizerState(NoCull);
		SetDepthStencilState(LessEqualDSS, 0);
	}
}