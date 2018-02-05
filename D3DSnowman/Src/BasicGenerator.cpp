//---------------------------------------------------------------------------
// File: 
//
// Author: Xu Lizi
// Blog: http://blog.csdn.net/xlz836900756
// Github: https://github.com/xlz0729
//---------------------------------------------------------------------------

#include "BasicGenerator.h"

BasicGenerator::BasicGenerator()
{
	m_TextureCount = 0;
	m_VertexStride = 0;
	m_IndexStride = 0;
	m_World = XMMatrixIdentity();

	m_pInputLayout = nullptr;
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_pTechnique = nullptr;

	m_pMaterialVariable = nullptr;
	m_pWorldVariable = nullptr;
	m_pWorldInvTransposeVariable = nullptr;
	m_pEyePosVariable = nullptr;
	m_pWorldViewProjVariable = nullptr;
	m_pTextureVariable = nullptr;
}

BasicGenerator::~BasicGenerator()
{
	SafeDestory();
}

void BasicGenerator::SafeDestory()
{
	DXUTGetGlobalResourceCache().OnDestroyDevice();
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pTechnique);
}

void BasicGenerator::SetMaterial(const Material & material)
{
	m_Material = material;
}

HRESULT BasicGenerator::SetVertexBuffer(ID3D11Device * g_pd3dDevice, const void* pVertices, UINT vertexCount, UINT vertexStride)
{
	HRESULT hr = S_OK;

	m_VertexStride = vertexStride;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = m_VertexStride * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = pVertices;
	V_RETURN(g_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer));

	return hr;
}

HRESULT BasicGenerator::SetIndexBuffer(ID3D11Device * g_pd3dDevice, const void * pIndices, UINT indexCount, UINT indexStride)
{
	HRESULT hr = S_OK;

	m_IndexStride = indexStride;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = m_IndexStride * indexCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = pIndices;
	V_RETURN(g_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer));

	return hr;
}
