//---------------------------------------------------------------------------
// File: 
//
// Author: Xu Lizi
// Blog: http://blog.csdn.net/xlz836900756
// Github: https://github.com/xlz0729
//---------------------------------------------------------------------------

#include "BoxGenerator.h"

HRESULT BoxGenerator::TextureLoader(ID3D11Device * g_pd3dDevice)
{
	HRESULT hr = S_OK;

	// Load the Texture
	V_RETURN(DXUTCreateShaderResourceViewFromFile(g_pd3dDevice, L"Texture/box.dds", &m_pTextureRV));

	return hr;
}

BoxGenerator::BoxGenerator()
{
	m_Position = { 20.0f, 0.0f, 0.0f, 1.0f };
	m_Lookat = { 0.0f, 0.0f, -1.0f, 1.0f };
	m_Snow[0] = false;
	m_Snow[1] = false;
	m_Snow[2] = false;
	m_Snow[3] = false;
	m_Rotation = XMMatrixIdentity();
	m_Ttranslation = XMMatrixIdentity();

	m_BoundingBox.point[0] = XMVECTOR{ -3.0f, 6.0f, -3.0f, 1.0f };
	m_BoundingBox.point[1] = XMVECTOR{ 3.0f, 6.0f, -3.0f, 1.0f };
	m_BoundingBox.point[2] = XMVECTOR{ 3.0f, 6.0f, 3.0f, 1.0f };
	m_BoundingBox.point[3] = XMVECTOR{ -3.0f, 6.0f, 3.0f, 1.0f };

	m_BoundingBox.point[4] = XMVECTOR{ -3.0f, 0.0f, -3.0f, 1.0f };
	m_BoundingBox.point[5] = XMVECTOR{ 3.0f, 0.0f, -3.0f, 1.0f };
	m_BoundingBox.point[6] = XMVECTOR{ 3.0f, 0.0f, 3.0f, 1.0f };
	m_BoundingBox.point[7] = XMVECTOR{ -3.0f, 0.0f, 3.0f, 1.0f };

	m_pTextureRV = nullptr;
}

BoxGenerator::~BoxGenerator()
{
	SafeDestory();
}

void BoxGenerator::SafeDestory()
{
	SAFE_RELEASE(m_pTextureRV);
	ObjectGenerator::SafeDestory();
}

void BoxGenerator::OnFrameMove(double fTime, float fElapsedTime, void * pUserContext)
{
	m_Rotation = XMMatrixRotationY(5.0f * XMConvertToRadians((float)fTime));

	m_Ttranslation = XMMatrixTranslationFromVector(m_Position);

	m_World = m_Ttranslation * m_Rotation;

	m_BoundingBox.point[0] = XMVector4Transform(XMVECTOR{ -3.0f, 6.0f, -3.0f, 1.0f }, m_World);
	m_BoundingBox.point[1] = XMVector4Transform(XMVECTOR{ 3.0f, 6.0f, -3.0f, 1.0f }, m_World);
	m_BoundingBox.point[2] = XMVector4Transform(XMVECTOR{ 3.0f, 6.0f, 3.0f, 1.0f }, m_World);
	m_BoundingBox.point[3] = XMVector4Transform(XMVECTOR{ -3.0f, 6.0f, 3.0f, 1.0f }, m_World);

	m_BoundingBox.point[4] = XMVector4Transform(XMVECTOR{ -3.0f, 0.0f, -3.0f, 1.0f }, m_World);
	m_BoundingBox.point[5] = XMVector4Transform(XMVECTOR{ 3.0f, 0.0f, -3.0f, 1.0f }, m_World);
	m_BoundingBox.point[6] = XMVector4Transform(XMVECTOR{ 3.0f, 0.0f, 3.0f, 1.0f }, m_World);
	m_BoundingBox.point[7] = XMVector4Transform(XMVECTOR{ -3.0f, 0.0f, 3.0f, 1.0f }, m_World);
}

HRESULT BoxGenerator::Create(ID3D11Device * g_pd3dDevice, ID3DX11Effect * g_pEffect)
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

	GeometryGenerator geoGen;

	GeometryGenerator::MeshData box;

	geoGen.CreateBox(6.0f, 6.0f, 6.0f, box);

	// Set Drawing infomation
	UINT totalVertexCount = 0;
	UINT totalIndexCount = 0;

	m_DrawInfos.push_back(DrawInfo(box.Indices.size(), totalIndexCount, totalVertexCount, 0));
	totalVertexCount += box.Vertices.size();
	totalIndexCount += box.Indices.size();

	// Create vertices
	std::vector<Vertex3F> vertices(totalVertexCount);
	UINT k = 0;
	for (auto i : box.Vertices)
	{
		vertices[k].Pos = XMFLOAT3(i.Position.x, i.Position.y + 3.0f, i.Position.z);
		vertices[k].Nor = i.Normal;
		vertices[k].Tex = i.TexC;
		++k;
	}

	std::vector<UINT> indices;
	indices.reserve(totalIndexCount);

	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());

	// Set vertex buffer
	V_RETURN(SetVertexBuffer(g_pd3dDevice, &vertices[0], totalVertexCount, sizeof(Vertex3F)));

	// Set index buffer
	V_RETURN(SetIndexBuffer(g_pd3dDevice, &indices[0], totalIndexCount, sizeof(DWORD)));

	return hr;
}

void BoxGenerator::Draw(ID3D11DeviceContext * g_pd3dImmediateContext, const CBaseCamera * g_Camera)
{
	// Set view matrix and project matrix
	XMMATRIX view = g_Camera->GetViewMatrix();
	XMMATRIX proj = g_Camera->GetProjMatrix();
	XMMATRIX worldViewProjection = m_World * view * proj;

	// Compute Inverse-transpose
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

	m_pTextureVariable->SetResource(m_pTextureRV);
	m_pTechnique->GetPassByIndex(0)->Apply(0, g_pd3dImmediateContext);
	g_pd3dImmediateContext->DrawIndexed(m_DrawInfos[0].IndexCount, m_DrawInfos[0].IndexOffset, m_DrawInfos[0].VertexOffset);
}


int BoxGenerator::Geton()
{
	for (int i = 0; i < 4; ++i)
	{
		if (!m_Snow[i]) {
			m_Snow[i] = true;
			return i;
		}
	}
	return -1;
}

void BoxGenerator::Getoff(int pos)
{
	if (pos >= 0 && pos < 4)
	{
		m_Snow[pos] = false;
	}
}
