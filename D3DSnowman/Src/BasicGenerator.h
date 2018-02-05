//---------------------------------------------------------------------------
// File: 
//
// Author: Xu Lizi
// Blog: http://blog.csdn.net/xlz836900756
// Github: https://github.com/xlz0729
//---------------------------------------------------------------------------

#pragma once

#include "GeometryGenerator.h"
#include "DXUT.h"
#include "DXUTcamera.h"
#include "SDKmisc.h"
#include "d3dx11effect.h"
#include "LightHelper.h"

#include <vector>

using namespace DirectX;
struct Vertex1F
{
	XMFLOAT3 Pos;
};

struct Vertex2F
{
	XMFLOAT3 Pos;
	XMFLOAT3 Nor;
};

struct Vertex3F
{
	XMFLOAT3 Pos;
	XMFLOAT3 Nor;
	XMFLOAT2 Tex;
};

struct DrawInfo
{
	UINT	IndexCount;
	UINT	IndexOffset;
	int		VertexOffset;
	int		TextureIndex;

	DrawInfo() = default;
	DrawInfo(UINT ic, UINT io, int vo, int ti) : IndexCount(ic), IndexOffset(io), VertexOffset(vo), TextureIndex(ti) {}
};

class BasicGenerator
{
public:
	BasicGenerator();
	~BasicGenerator();

	virtual void SafeDestory();

	virtual void SetMaterial(const Material& material);

	virtual HRESULT SetVertexBuffer(ID3D11Device * pd3dDevice, const void* pVertices, UINT vertexCount, UINT vertexStride);

	virtual HRESULT SetIndexBuffer(ID3D11Device * pd3dDevice, const void* pIndices, UINT indexCount, UINT indexStride);

	virtual HRESULT SetLayout(ID3D11Device * pd3dDevice) = 0;

	virtual void Draw(ID3D11DeviceContext * pd3dImmediateContext, const CBaseCamera* g_Camera) = 0;

	virtual HRESULT Create(ID3D11Device* pd3dDevice, ID3DX11Effect* pEffect) = 0;

protected:
	UINT		m_TextureCount;
	UINT		m_VertexStride;
	UINT		m_IndexStride;
	XMMATRIX	m_World;
	Material	m_Material;

	ID3D11InputLayout*		m_pInputLayout;
	ID3D11Buffer*			m_pVertexBuffer;
	ID3D11Buffer*			m_pIndexBuffer;
	ID3DX11EffectTechnique*	m_pTechnique;

	ID3DX11EffectVariable*					m_pMaterialVariable;
	ID3DX11EffectMatrixVariable*			m_pWorldVariable;
	ID3DX11EffectMatrixVariable*			m_pWorldInvTransposeVariable;
	ID3DX11EffectVectorVariable*			m_pEyePosVariable;
	ID3DX11EffectMatrixVariable*			m_pWorldViewProjVariable;
	ID3DX11EffectShaderResourceVariable*	m_pTextureVariable;

	std::vector<DrawInfo> m_DrawInfos;
};