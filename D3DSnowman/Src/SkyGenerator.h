//---------------------------------------------------------------------------
// File: 
//
// Author: Xu Lizi
// Blog: http://blog.csdn.net/xlz836900756
// Github: https://github.com/xlz0729
//---------------------------------------------------------------------------

#pragma once

#include "BasicGenerator.h"

class SkyGenerator : public BasicGenerator
{
private:
	const LPCSTR m_TechniqueName = "RenderSky";

	ID3D11ShaderResourceView*				m_pTextureRV;

	HRESULT TextureLoader(ID3D11Device* g_pd3dDevice);

public:
	SkyGenerator();
	~SkyGenerator();

	void SafeDestory();

	HRESULT SetLayout(ID3D11Device * pd3dDevice) override;

	HRESULT Create(ID3D11Device* pd3dDevice, ID3DX11Effect* pEffect) override;

	void Draw(ID3D11DeviceContext * pd3dImmediateContext, const CBaseCamera* Camera) override;
};