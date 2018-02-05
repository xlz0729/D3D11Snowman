//---------------------------------------------------------------------------
// File: 
//
// Author: Xu Lizi
// Blog: http://blog.csdn.net/xlz836900756
// Github: https://github.com/xlz0729
//---------------------------------------------------------------------------

#pragma once


#include "ObjectGenerator.h"

class BoxGenerator : public ObjectGenerator
{
private:
	const LPCSTR	m_TechniqueName = "RenderSnowman";

	bool	m_Snow[4];

	XMMATRIX	m_Rotation;
	XMMATRIX	m_Ttranslation;

	ID3D11ShaderResourceView*	m_pTextureRV;

	// Ë½ÓÐº¯Êý
	HRESULT TextureLoader(ID3D11Device* g_pd3dDevice);

public:
	BoundingBox m_BoundingBox;

	BoxGenerator();
	~BoxGenerator();

	void SafeDestory();

	void OnFrameMove(double fTime, float fElapsedTime, void* pUserContext) override;

	HRESULT Create(ID3D11Device* g_pd3dDevice, ID3DX11Effect* g_pEffect) override;

	void Draw(ID3D11DeviceContext * g_pd3dImmediateContext, const CBaseCamera* g_Camera) override;

	LRESULT HandleMessages(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) override { return FALSE; }

	XMMATRIX GetWorld() { return m_World; }
	XMMATRIX GetTransport() { return m_Ttranslation; }
	XMMATRIX GetRotation() { return m_Rotation; }

	int Geton();

	void Getoff(int pos);
};