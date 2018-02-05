//---------------------------------------------------------------------------
// File: Snowman.cpp
//
// Author: Xu Lizi
// Blog: http://blog.csdn.net/xlz836900756
// Github: https://github.com/xlz0729
//---------------------------------------------------------------------------

#include "SnowmanGenerator.h"
//---------------------------------------------------------------------------
// 私有函数
//---------------------------------------------------------------------------
HRESULT SnowmanGenerator::TextureLoader(ID3D11Device* g_pd3dDevice)
{
	HRESULT hr = S_OK;

	// Load the Texture
	ID3D11ShaderResourceView* textureRV1;
	V_RETURN(DXUTCreateShaderResourceViewFromFile(g_pd3dDevice, L"Texture/Snow.dds", &textureRV1));
	m_pTextureRVs.push_back(textureRV1);

	ID3D11ShaderResourceView* textureRV2;
	V_RETURN(DXUTCreateShaderResourceViewFromFile(g_pd3dDevice, L"Texture/Scarf.dds", &textureRV2));
	m_pTextureRVs.push_back(textureRV2);

	ID3D11ShaderResourceView* textureRV3;
	V_RETURN(DXUTCreateShaderResourceViewFromFile(g_pd3dDevice, L"Texture/Hat.dds", &textureRV3));
	m_pTextureRVs.push_back(textureRV3);

	return hr;
}


SnowmanGenerator::SnowmanGenerator()
{
	m_Up = { 0.0f, 1.0f, 0.0f, 1.0f };

	m_Car = -1;

	SetViewParams();

	m_Camera.SetEnablePositionMovement(TRUE);
	m_Camera.SetViewParams(m_Position, m_Lookat);

	m_BoundingBox.point[0] = XMVECTOR{ -1.5f, 5.5f, -1.5f, 1.0f };
	m_BoundingBox.point[1] = XMVECTOR{ 1.5f, 5.5f, -1.5f, 1.0f };
	m_BoundingBox.point[2] = XMVECTOR{ 1.5f, 5.5f, 1.5f, 1.0f };
	m_BoundingBox.point[3] = XMVECTOR{ -1.5f, 5.5f, 1.5f, 1.0f };

	m_BoundingBox.point[4] = XMVECTOR{ -1.5f, 0.0f, -1.5f, 1.0f };
	m_BoundingBox.point[5] = XMVECTOR{ 1.5f, 0.0f, -1.5f, 1.0f };
	m_BoundingBox.point[6] = XMVECTOR{ 1.5f, 0.0f, 1.5f, 1.0f };
	m_BoundingBox.point[7] = XMVECTOR{ -1.5f, 0.0f, 1.5f, 1.0f };

	m_pTextureRVs.clear();
}


//---------------------------------------------------------------------------
// 公有函数
//---------------------------------------------------------------------------
SnowmanGenerator::~SnowmanGenerator()
{
	SafeDestory();
}

void SnowmanGenerator::SafeDestory()
{
	for (auto i = m_pTextureRVs.begin(); i != m_pTextureRVs.end(); ++i)
	{
		SAFE_RELEASE(*i);
	}
	ObjectGenerator::SafeDestory();
}

void SnowmanGenerator::OnFrameMove(float fElapsedTime, BoxGenerator& g_Car)
{
	m_Camera.FrameMove(fElapsedTime);
	
	XMMATRIX translation = XMMatrixIdentity();
	if (m_Car >= 0)
	{
		translation = g_Car.GetWorld();
		SetViewParams();
		m_Position = XMVector4Transform(m_Position, translation);
		m_Lookat = XMVector4Transform(m_Lookat, translation);
		m_Camera.SetViewParams(m_Position, m_Lookat);
	}
	else
	{
		m_Position = m_Camera.GetEyePt();
		m_Lookat = m_Camera.GetLookAtPt();
		m_Up = m_Camera.GetWorldUp();
	}
	m_World = m_Camera.GetWorldMatrix();

	m_BoundingBox.point[0] = XMVector4Transform(XMVECTOR{ -1.5f, 5.5f, -1.5f, 1.0f }, m_World);
	m_BoundingBox.point[1] = XMVector4Transform(XMVECTOR{ 1.5f, 5.5f, -1.5f, 1.0f }, m_World);
	m_BoundingBox.point[2] = XMVector4Transform(XMVECTOR{ 1.5f, 5.5f, 1.5f, 1.0f }, m_World);
	m_BoundingBox.point[3] = XMVector4Transform(XMVECTOR{ -1.5f, 5.5f, 1.5f, 1.0f }, m_World);

	m_BoundingBox.point[4] = XMVector4Transform(XMVECTOR{ -1.5f, 0.0f, -1.5f, 1.0f }, m_World);
	m_BoundingBox.point[5] = XMVector4Transform(XMVECTOR{ 1.5f, 0.0f, -1.5f, 1.0f }, m_World);
	m_BoundingBox.point[6] = XMVector4Transform(XMVECTOR{ 1.5f, 0.0f, 1.5f, 1.0f }, m_World);
	m_BoundingBox.point[7] = XMVector4Transform(XMVECTOR{ -1.5f, 0.0f, 1.5f, 1.0f }, m_World);
}

HRESULT SnowmanGenerator::Create(ID3D11Device* g_pd3dDevice, ID3DX11Effect* g_pEffect)
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

	GeometryGenerator::MeshData bodySphere;
	GeometryGenerator::MeshData headSphere;
	GeometryGenerator::MeshData scarfTorus;
	GeometryGenerator::MeshData hatCone;
	GeometryGenerator::MeshData noseCone;
	GeometryGenerator::MeshData eyeSphere;

	geoGen.CreateGeosphere(1.5f, 45, bodySphere);
	geoGen.CreateGeosphere(0.9f, 45, headSphere);
	geoGen.CreateTorus(1.5f, 0.4f, 45, scarfTorus);
	geoGen.CreateCylinder(0.5f, 0.0f, 1.0f, 40, 40, hatCone);
	geoGen.CreateCylinder(0.2f, 0.0f, 0.5f, 35, 35, noseCone);
	geoGen.CreateGeosphere(0.1f, 30, eyeSphere);

	// Set Drawing infomation
	UINT totalVertexCount = 0;
	UINT totalIndexCount = 0;

	m_DrawInfos.push_back( DrawInfo(bodySphere.Indices.size(), totalIndexCount, totalVertexCount, 0) );
	totalVertexCount += bodySphere.Vertices.size();
	totalIndexCount += bodySphere.Indices.size();

	m_DrawInfos.push_back( DrawInfo(headSphere.Indices.size(), totalIndexCount, totalVertexCount, 0) );
	totalVertexCount += headSphere.Vertices.size();
	totalIndexCount += headSphere.Indices.size();

	m_DrawInfos.push_back( DrawInfo(scarfTorus.Indices.size(), totalIndexCount, totalVertexCount, 1) );
	totalVertexCount += scarfTorus.Vertices.size();
	totalIndexCount += scarfTorus.Indices.size();

	m_DrawInfos.push_back( DrawInfo(hatCone.Indices.size(), totalIndexCount, totalVertexCount, 2) );
	totalVertexCount += hatCone.Vertices.size();
	totalIndexCount += hatCone.Indices.size();

	m_DrawInfos.push_back( DrawInfo(noseCone.Indices.size(), totalIndexCount, totalVertexCount, 2) );
	totalVertexCount += noseCone.Vertices.size();
	totalIndexCount += noseCone.Indices.size();

	m_DrawInfos.push_back( DrawInfo(eyeSphere.Indices.size(), totalIndexCount, totalVertexCount, 2) );
	totalVertexCount += eyeSphere.Vertices.size();
	totalIndexCount += eyeSphere.Indices.size();

	m_DrawInfos.push_back( DrawInfo(eyeSphere.Indices.size(), totalIndexCount, totalVertexCount, 2));
	totalVertexCount += eyeSphere.Vertices.size();
	totalIndexCount += eyeSphere.Indices.size();

	// Create vertices
	std::vector<Vertex3F> vertices(totalVertexCount);
	UINT k = 0;
	for (auto i : bodySphere.Vertices)
	{
		vertices[k].Pos = XMFLOAT3(i.Position.x, i.Position.y + 1.5f, i.Position.z);
		vertices[k].Nor = i.Normal;
		vertices[k].Tex = i.TexC;
		++k;
	}

	for (auto i : headSphere.Vertices)
	{
		vertices[k].Pos = XMFLOAT3(i.Position.x, i.Position.y + 3.5f, i.Position.z);
		vertices[k].Nor = i.Normal;
		vertices[k].Tex = i.TexC;
		++k;
	}

	for (auto i : scarfTorus.Vertices)
	{
		vertices[k].Pos = XMFLOAT3(i.Position.x, i.Position.y + 2.8f, i.Position.z);
		vertices[k].Nor = i.Normal;
		vertices[k].Tex = i.TexC;
		++k;
	}

	for (auto i : hatCone.Vertices)
	{
		vertices[k].Pos = XMFLOAT3(i.Position.x, i.Position.y + 4.75f, i.Position.z);
		vertices[k].Nor = i.Normal;
		vertices[k].Tex = i.TexC;
		++k;
	}

	for (auto i : noseCone.Vertices)
	{
		vertices[k].Pos = XMFLOAT3(i.Position.x, i.Position.z + 3.5f, i.Position.y + 0.9f);
		vertices[k].Nor = i.Normal;
		vertices[k].Tex = i.TexC;
		++k;
	}

	for (auto i : eyeSphere.Vertices)
	{
		vertices[k].Pos = XMFLOAT3(i.Position.x + 0.4f, i.Position.y + 3.85f, i.Position.z + 0.75f);
		vertices[k].Nor = XMFLOAT3(0.0f, 0.0f, 0.0f);
		++k;
	}

	for (auto i : eyeSphere.Vertices)
	{
		vertices[k].Pos = XMFLOAT3(i.Position.x - 0.4f, i.Position.y + 3.85f, i.Position.z + 0.75f);
		vertices[k].Nor = XMFLOAT3(0.0f, 0.0f, 0.0f);
		++k;
	}

	// Create Indices
	std::vector<UINT> indices;
	indices.reserve(totalIndexCount);

	indices.insert(indices.end(), bodySphere.Indices.begin(), bodySphere.Indices.end());
	indices.insert(indices.end(), headSphere.Indices.begin(), headSphere.Indices.end());
	indices.insert(indices.end(), scarfTorus.Indices.begin(), scarfTorus.Indices.end());
	indices.insert(indices.end(), hatCone.Indices.begin(), hatCone.Indices.end());
	indices.insert(indices.end(), noseCone.Indices.begin(), noseCone.Indices.end());
	indices.insert(indices.end(), eyeSphere.Indices.begin(), eyeSphere.Indices.end());
	indices.insert(indices.end(), eyeSphere.Indices.begin(), eyeSphere.Indices.end());

	// Set vertex buffer
	V_RETURN(SetVertexBuffer(g_pd3dDevice, &vertices[0], totalVertexCount, sizeof(Vertex3F)));

	// Set index buffer
	V_RETURN(SetIndexBuffer(g_pd3dDevice, &indices[0], totalIndexCount, sizeof(DWORD)));

	return hr;
}

void SnowmanGenerator::Draw(ID3D11DeviceContext * g_pd3dImmediateContext, const CBaseCamera* g_Camera)
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

	for (UINT i = 0; i < 5; ++i)
	{
		m_pTextureVariable->SetResource(m_pTextureRVs[m_DrawInfos[i].TextureIndex]);
		m_pTechnique->GetPassByIndex(0)->Apply(0, g_pd3dImmediateContext);
		g_pd3dImmediateContext->DrawIndexed(m_DrawInfos[i].IndexCount, m_DrawInfos[i].IndexOffset, m_DrawInfos[i].VertexOffset);
	}

	for (UINT i = 5; i < m_DrawInfos.size(); ++i)
	{
		m_pTextureVariable->SetResource(m_pTextureRVs[m_DrawInfos[i].TextureIndex]);
		m_pTechnique->GetPassByIndex(1)->Apply(0, g_pd3dImmediateContext);
		g_pd3dImmediateContext->DrawIndexed(m_DrawInfos[i].IndexCount, m_DrawInfos[i].IndexOffset, m_DrawInfos[i].VertexOffset);
	}
}

void SnowmanGenerator::SetViewParams()
{
	switch (m_Car)
	{
		case 0:
		{
			m_Position = XMVECTOR{ -1.5f, 6.1f, -1.5f, 1.0f };
			break;
		}
		case 1:
		{
			m_Position = XMVECTOR{ 1.5f, 6.1f, -1.5f, 1.0f };
			break;
		}
		case 2:
		{
			m_Position = XMVECTOR{ -1.5f, 6.1f, 1.5f, 1.0f };
			break;
		}
		case 3:
		{
			m_Position = XMVECTOR{ 1.5f, 6.1f, 1.5f, 1.0f };
			break;
		}
		default:
		{
			m_Position = XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f };
			break;
		}
	}
	m_Lookat = m_Position + XMVECTOR{ 0.0f, 0.0f, -1.0f, 0.0f };
}

void SnowmanGenerator::GetoffPos(BoxGenerator& g_Car)
{
	XMVECTOR position;
	switch (m_Car)
	{
		case 0:
		{
			m_Position = XMVECTOR{ -5.5f, 0.0f, -2.5f, 1.0f };
			break;
		}
		case 1:
		{
			m_Position = XMVECTOR{ 5.5f, 0.0f, -2.5f, 1.0f };
			break;
		}
		case 2:
		{
			m_Position = XMVECTOR{ -5.5f, 0.0f, 2.5f, 1.0f };
			break;
		}
		case 3:
		{
			m_Position = XMVECTOR{ 5.5f, 0.0f, 2.5f, 1.0f };
			break;
		}
		default:
		{
			m_Position = XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f };
		}
	}
	m_Lookat = m_Position + XMVECTOR{ 0.0f, 0.0f, -1.0f, 0.0f };
	m_Position = XMVector4Transform(m_Position, g_Car.GetWorld());
	m_Lookat = XMVector4Transform(m_Lookat, g_Car.GetWorld());
	m_Camera.SetViewParams(m_Position, m_Lookat);
	g_Car.Getoff(m_Car);
	m_Car = -1;
}
