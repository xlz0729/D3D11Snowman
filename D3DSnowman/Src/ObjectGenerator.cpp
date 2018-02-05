#include "ObjectGenerator.h"
//---------------------------------------------------------------------------
// File: 
//
// Author: Xu Lizi
// Blog: http://blog.csdn.net/xlz836900756
// Github: https://github.com/xlz0729
//---------------------------------------------------------------------------

ObjectGenerator::ObjectGenerator()
{
	m_Position = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_Lookat = { 0.0f, 0.0f, -1.0f, 1.0f };
	m_Up = { 0.0f, 1.0f, 0.0f, 1.0f };
}

ObjectGenerator::~ObjectGenerator()
{
	SafeDestory();
}

void ObjectGenerator::SafeDestory()
{
	BasicGenerator::SafeDestory();
}

HRESULT ObjectGenerator::SetLayout(ID3D11Device * pd3dDevice)
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
