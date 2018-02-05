//---------------------------------------------------------------------------
// File: HillsGenerator.h
//
// Author: Xu Lizi
// Blog: http://blog.csdn.net/xlz836900756
// Github: https://github.com/xlz0729
//---------------------------------------------------------------------------

#pragma once

#include "BasicGenerator.h"

using namespace DirectX;

class HillsGenerator : public BasicGenerator
{
private:
	const LPCSTR m_TechniqueName = "RenderHills";

	ID3D11ShaderResourceView*				m_pTextureRV;

	HRESULT TextureLoader(ID3D11Device* g_pd3dDevice);

public:
	HillsGenerator();
	~HillsGenerator();

	void SafeDestory();

	HRESULT SetLayout(ID3D11Device * pd3dDevice) override;

	HRESULT Create(ID3D11Device* g_pd3dDevice, ID3DX11Effect* g_pEffect) override;

	void Draw(ID3D11DeviceContext * g_pd3dImmediateContext, const CBaseCamera* g_Camera) override;
};