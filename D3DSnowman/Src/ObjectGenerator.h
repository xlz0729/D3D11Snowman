//---------------------------------------------------------------------------
// File: 
//
// Author: Xu Lizi
// Blog: http://blog.csdn.net/xlz836900756
// Github: https://github.com/xlz0729
//---------------------------------------------------------------------------

#pragma once

#include "BasicGenerator.h"

struct BoundingBox
{
	XMVECTOR point[8];
};

class ObjectGenerator : public BasicGenerator
{
public:
	ObjectGenerator();
	~ObjectGenerator();

	virtual void Draw(ID3D11DeviceContext * pd3dImmediateContext, const CBaseCamera* g_Camera) = 0;

	virtual HRESULT Create(ID3D11Device* pd3dDevice, ID3DX11Effect* pEffect) = 0;

	virtual void OnFrameMove(double fTime, float fElapsedTime, void* pUserContext) = 0;

	virtual LRESULT HandleMessages(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) = 0;

	virtual void SafeDestory();

	virtual HRESULT SetLayout(ID3D11Device * pd3dDevice);

	XMVECTOR GetPosition() { return m_Position; }

	void SetPosition(XMVECTOR pos) { m_Position = pos; }

	XMVECTOR GetLookAt() { return m_Lookat; }

	void SetLookat(XMVECTOR look) { m_Lookat = look; }

	XMVECTOR GetUp() { return m_Up; }

	void SetUp(XMVECTOR up) { m_Up = up; }

protected:
	XMVECTOR m_Position;
	XMVECTOR m_Lookat;
	XMVECTOR m_Up;
};