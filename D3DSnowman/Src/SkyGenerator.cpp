#include "SkyGenerator.h"
//---------------------------------------------------------------------------
// File: 
//
// Author: Xu Lizi
// Blog: http://blog.csdn.net/xlz836900756
// Github: https://github.com/xlz0729
//---------------------------------------------------------------------------

HRESULT SkyGenerator::TextureLoader(ID3D11Device * g_pd3dDevice)
{
	HRESULT hr = S_OK;

	// Load the Texture
	V_RETURN(DXUTCreateShaderResourceViewFromFile(g_pd3dDevice, L"Texture/snowcube1024.dds", &m_pTextureRV));

	return hr;
}

SkyGenerator::SkyGenerator()
{
	m_pTextureRV = nullptr;
}

SkyGenerator::~SkyGenerator()
{
	SafeDestory();
}

void SkyGenerator::SafeDestory()
{
	SAFE_RELEASE(m_pTextureRV);
	BasicGenerator::SafeDestory();
}

HRESULT SkyGenerator::SetLayout(ID3D11Device * pd3dDevice)
{
	HRESULT hr = S_OK;

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	D3DX11_PASS_DESC PassDesc;
	V_RETURN(m_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc));
	V_RETURN(pd3dDevice->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pInputLayout));

	return hr;
}

HRESULT SkyGenerator::Create(ID3D11Device * g_pd3dDevice, ID3DX11Effect * g_pEffect)
{
	HRESULT hr = S_OK;

	// Obtain the technique
	m_pTechnique = g_pEffect->GetTechniqueByName(m_TechniqueName);

	// Obtain the variable
	m_pWorldViewProjVariable = g_pEffect->GetVariableByName("WorldViewProj")->AsMatrix();
	m_pTextureVariable = g_pEffect->GetVariableByName("g_CubeMap")->AsShaderResource();

	// Load texture
	V_RETURN(TextureLoader(g_pd3dDevice));

	// Set layout
	V_RETURN(SetLayout(g_pd3dDevice));

	GeometryGenerator::MeshData sky;

	GeometryGenerator geoGen;

	geoGen.CreateSphere(5000.0f, 30, 30, sky);

	int gridVertexCount = sky.Vertices.size();
	int gridIndexCount = sky.Indices.size();

	m_DrawInfos.push_back(DrawInfo(gridIndexCount, 0, 0, 0));

	// Create vertices
	std::vector<Vertex1F> vertices(gridVertexCount);
	for (UINT i = 0; i < gridVertexCount; ++i)
	{
		vertices[i].Pos = sky.Vertices[i].Position;
	}

	// Create indices
	std::vector<USHORT> indices;
	indices.reserve(gridIndexCount);

	indices.insert(indices.end(), sky.Indices.begin(), sky.Indices.end());

	// Set vertex buffer
	V_RETURN(SetVertexBuffer(g_pd3dDevice, &vertices[0], gridVertexCount, sizeof(Vertex1F)));

	// Set index buffer
	V_RETURN(SetIndexBuffer(g_pd3dDevice, &indices[0], gridIndexCount, sizeof(USHORT)));

	return hr;
}

void SkyGenerator::Draw(ID3D11DeviceContext * g_pd3dImmediateContext, const CBaseCamera* g_Camera)
{
	// Set view matrix and project matrix
	XMMATRIX view = g_Camera->GetViewMatrix();
	XMMATRIX proj = g_Camera->GetProjMatrix();
	XMMATRIX worldViewProjection = view * proj;

	XMFLOAT3 eyePos;
	XMStoreFloat3(&eyePos, g_Camera->GetEyePt());

	// Update constant buffer that define in father class
	m_pWorldViewProjVariable->SetMatrix((float*)&worldViewProjection);
	m_pTextureVariable->SetResource(m_pTextureRV);

	// Set vertex buffer
	UINT stride = m_VertexStride;
	UINT offset = 0;
	g_pd3dImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set index buffer
	g_pd3dImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set the Vertex Layout
	g_pd3dImmediateContext->IASetInputLayout(m_pInputLayout);

	// Set primitive topology
	g_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (UINT i = 0; i < m_DrawInfos.size(); ++i)
	{
		m_pTechnique->GetPassByIndex(0)->Apply(0, g_pd3dImmediateContext);
		g_pd3dImmediateContext->DrawIndexed(m_DrawInfos[i].IndexCount, m_DrawInfos[i].IndexOffset, m_DrawInfos[i].VertexOffset);
	}
}
