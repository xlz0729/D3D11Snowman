//---------------------------------------------------------------------------
// File: Snowman.h
//
// Author: Xu Lizi
// Blog: http://blog.csdn.net/xlz836900756
// Github: https://github.com/xlz0729
//---------------------------------------------------------------------------

#pragma once

#include "DXUT.h"
#include "DXUTcamera.h"
#include "BoxGenerator.h"

#include <math.h>

class SnowmanGenerator : public ObjectGenerator
{
private:
	const LPCSTR	m_TechniqueName = "RenderSnowman";
	const XMVECTOR	m_PosEye = XMVECTOR{ 0.0f, 3.0f, 0.0f, 0.0f };

	std::vector<ID3D11ShaderResourceView*>	m_pTextureRVs;

	// Ë½ÓÐº¯Êý
	HRESULT TextureLoader(ID3D11Device* g_pd3dDevice);

public:
	int						m_Car;
	BoundingBox				m_BoundingBox;
	CFirstPersonCamera		m_Camera;

	SnowmanGenerator();
	~SnowmanGenerator();

	void SafeDestory();

	void OnFrameMove(float fElapsedTime, BoxGenerator& g_Car);

	void OnFrameMove(double fTime, float fElapsedTime, void* pUserContext) override {}
	
	HRESULT Create(ID3D11Device* g_pd3dDevice, ID3DX11Effect* g_pEffect) override;

	void Draw(ID3D11DeviceContext * g_pd3dImmediateContext, const CBaseCamera* g_Camera) override;

	LRESULT HandleMessages(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) override { return FALSE; }

	void SetViewParams();

	void GetoffPos(BoxGenerator& g_Car);
};
