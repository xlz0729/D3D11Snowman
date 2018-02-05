//---------------------------------------------------------------------------
// File: HillsGenerator.cpp
//
// Author: Xu Lizi
// Blog: http://blog.csdn.net/xlz836900756
// Github: https://github.com/xlz0729
//---------------------------------------------------------------------------

#include "HillsGenerator.h"

//---------------------------------------------------------------------------
// 私有函数
//---------------------------------------------------------------------------
HRESULT HillsGenerator::TextureLoader(ID3D11Device* g_pd3dDevice)
{
	HRESULT hr = S_OK;

	// Load the Texture
	V_RETURN(DXUTCreateShaderResourceViewFromFile(g_pd3dDevice, L"Texture/Snow.dds", &m_pTextureRV));

	return hr;
}


HillsGenerator::HillsGenerator()
{
	m_pTextureRV = nullptr;
}

//---------------------------------------------------------------------------
// 公有函数
//---------------------------------------------------------------------------
HillsGenerator::~HillsGenerator()
{
	SafeDestory();
}

void HillsGenerator::SafeDestory()
{
	SAFE_RELEASE(m_pTextureRV);
	BasicGenerator::SafeDestory();
}

HRESULT HillsGenerator::SetLayout(ID3D11Device * pd3dDevice)
{
	HRESULT hr = S_OK;

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	D3DX11_PASS_DESC PassDesc;
	V_RETURN(m_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc));
	V_RETURN(pd3dDevice->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pInputLayout));

	return hr;
}

HRESULT HillsGenerator::Create(ID3D11Device * g_pd3dDevice, ID3DX11Effect * g_pEffect)
{
	HRESULT hr = S_OK;

	// Obtain the technique
	m_pTechnique = g_pEffect->GetTechniqueByName(m_TechniqueName);

	// Obtain the variable
	m_pTextureVariable = g_pEffect->GetVariableByName("g_txDiffuse")->AsShaderResource();
	m_pWorldVariable = g_pEffect->GetVariableByName("World")->AsMatrix();
	m_pWorldInvTransposeVariable = g_pEffect->GetVariableByName("WorldInvTranspose")->AsMatrix();
	m_pEyePosVariable = g_pEffect->GetVariableByName("EyePos")->AsVector();
	m_pWorldViewProjVariable = g_pEffect->GetVariableByName("WorldViewProj")->AsMatrix();
	m_pMaterialVariable = g_pEffect->GetVariableByName("gMaterial");

	// Load texture
	V_RETURN(TextureLoader(g_pd3dDevice));

	// Set layout
	V_RETURN(SetLayout(g_pd3dDevice));

	GeometryGenerator::MeshData grid;

	GeometryGenerator geoGen;

	geoGen.CreateGrid(1000.0f, 1000.0f, 50, 50, grid);

	int gridVertexCount = grid.Vertices.size();
	int gridIndexCount = grid.Indices.size();

	m_DrawInfos.push_back(DrawInfo(gridIndexCount, 0, 0, 0));

	// Create vertices
	std::vector<Vertex3F> vertices(gridVertexCount);
	for (UINT i = 0; i < gridVertexCount; ++i)
	{
		vertices[i].Pos = grid.Vertices[i].Position;
		vertices[i].Nor = grid.Vertices[i].Normal;
		vertices[i].Tex = grid.Vertices[i].TexC;
	}

	// Create indices
	std::vector<UINT> indices;
	indices.reserve(gridIndexCount);

	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());

	// Set vertex buffer
	V_RETURN(SetVertexBuffer(g_pd3dDevice, &vertices[0], gridVertexCount, sizeof(Vertex3F)));

	// Set index buffer
	V_RETURN(SetIndexBuffer(g_pd3dDevice, &indices[0], gridIndexCount, sizeof(DWORD)));

	return hr;
}

void HillsGenerator::Draw(ID3D11DeviceContext * g_pd3dImmediateContext, const CBaseCamera* g_Camera)
{
	// Set view matrix and project matrix
	XMMATRIX view = g_Camera->GetViewMatrix();
	XMMATRIX proj = g_Camera->GetProjMatrix();
	XMMATRIX worldViewProjection = m_World * view * proj;

	XMMATRIX A = m_World;
	A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR det = XMMatrixDeterminant(A);
	XMMATRIX worldInvTranspose = XMMatrixTranspose(XMMatrixInverse(&det, A));

	XMFLOAT3 eyePos;
	XMStoreFloat3(&eyePos, g_Camera->GetEyePt());

	// Update constant buffer that define in father class
	m_pEyePosVariable->SetFloatVector((float*)&eyePos);
	m_pWorldVariable->SetMatrix((float*)&m_World);
	m_pWorldInvTransposeVariable->SetMatrix((float*)&worldInvTranspose);
	m_pWorldViewProjVariable->SetMatrix((float*)&worldViewProjection);
	m_pMaterialVariable->SetRawValue(&m_Material, 0, sizeof(m_Material));

	// Set vertex buffer
	UINT stride = m_VertexStride;
	UINT offset = 0;
	g_pd3dImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set index buffer
	g_pd3dImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the Vertex Layout
	g_pd3dImmediateContext->IASetInputLayout(m_pInputLayout);

	// Set primitive topology
	g_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (UINT i = 0; i < m_DrawInfos.size(); ++i)
	{
		m_pTextureVariable->SetResource(m_pTextureRV);
		m_pTechnique->GetPassByIndex(0)->Apply(0, g_pd3dImmediateContext);
		g_pd3dImmediateContext->DrawIndexed(m_DrawInfos[i].IndexCount, m_DrawInfos[i].IndexOffset, m_DrawInfos[i].VertexOffset);
	}
}

