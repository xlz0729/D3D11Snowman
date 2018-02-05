//---------------------------------------------------------------------------
// File: main.cpp
//
// Author: Xu Lizi
// Blog: http://blog.csdn.net/xlz836900756
// Github: https://github.com/xlz0729
//---------------------------------------------------------------------------

#include "DXUT.h"
#include "DXUTcamera.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "DDSTextureLoader.h"
#include "d3dx11effect.h"

#include "LightHelper.h"
#include "SnowmanGenerator.h"
#include "BoxGenerator.h"
#include "HillsGenerator.h"
#include "SkyGenerator.h"

#pragma warning( disable : 4100 )

using namespace DirectX;


//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct CBNeverChanges
{
	Material			mMaterial;
	DirectionalLight	mDirectionalLight;
};

struct CBChangeOnResize
{
	XMFLOAT4X4 mProjection;
};

struct CBChangesEveryFrame
{
	XMFLOAT4X4	mWorld;
	XMFLOAT4X4	mWorldViewProj;
};


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
CBaseCamera*			g_Camera = nullptr;
CModelViewerCamera		g_ModelViewerCamera;
SnowmanGenerator		g_Snowman1;
SnowmanGenerator		g_Snowman2;
BoxGenerator			g_Box;
HillsGenerator			g_Hills;
SkyGenerator			g_Sky;

ID3D11SamplerState*				g_pSamplerLinear = nullptr;
ID3DX11Effect*					g_pEffect = nullptr;

ID3DX11EffectVariable*			g_pDirLightVariable = nullptr;

bool firstPersonFlag = false;


bool CollisionDetection(const BoundingBox& snowman, XMMATRIX world)
{
	XMVECTOR det = XMMatrixDeterminant(world);
	XMMATRIX worldInv = XMMatrixInverse(&det, world);
	for (int i = 0; i < 8; ++i)
	{
		XMFLOAT3 tmp;
		XMStoreFloat3(&tmp, XMVector4Transform(snowman.point[i], worldInv));
		if (tmp.x < 3.0f && tmp.x > -3.0f && tmp.y < 6.0f && tmp.y > 0.0f && tmp.z < 3.0f && tmp.z > -3.0f)
			return true;
	}
	return false;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.
// This function is called before a frame is rendered. It is used to process the world state.
// This function performs differently on a slower system than on a faster system.
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
//--------------------------------------------------------------------------------------
	g_Box.OnFrameMove(fTime, fElapsedTime, pUserContext);
	g_Snowman1.OnFrameMove(fElapsedTime, g_Box);
	g_Snowman2.OnFrameMove(fElapsedTime, g_Box);

	if (g_Snowman1.m_Car < 0 && CollisionDetection(g_Snowman1.m_BoundingBox, g_Box.GetWorld()))
	{
		g_Snowman1.m_Car = g_Box.Geton();
	}
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	if (bKeyDown)
	{
		switch (nChar)
		{
			case VK_ESCAPE: // 按下Esc键退出                
				DXUTGetExitCode();
				break;
			case VK_F1:
				firstPersonFlag = !firstPersonFlag;
				if (firstPersonFlag) {
					g_Camera = &g_Snowman1.m_Camera;
				}
				else {
					g_Camera = &g_ModelViewerCamera;
				}
				break;
			case 'F':
				g_Snowman1.GetoffPos(g_Box);
		}
	}
}


//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
						  bool* pbNoFurtherProcessing, void* pUserContext )
{
	g_ModelViewerCamera.HandleMessages(hWnd, uMsg, wParam, lParam);
	if (firstPersonFlag) {
		g_Snowman1.m_Camera.HandleMessages(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
	return true;
}


//--------------------------------------------------------------------------------------
// Call if device was removed.  Return true to find a new device, false to quit
//--------------------------------------------------------------------------------------
bool CALLBACK OnDeviceRemoved( void* pUserContext )
{
	return true;
}


//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
									   DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
	return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr = S_OK;

	auto pd3dImmediateContext = DXUTGetD3D11DeviceContext();

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

#if D3D_COMPILER_VERSION >= 46

	// Read the D3DX effect file
	WCHAR str[MAX_PATH];
	V_RETURN(DXUTFindDXSDKMediaFileCch(str, MAX_PATH, L"Shader/D3DSnowman.fx"));

	V_RETURN(D3DX11CompileEffectFromFile(str, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, dwShaderFlags, 0, pd3dDevice, &g_pEffect, nullptr));

#else

	ID3DBlob* pEffectBuffer = nullptr;
	V_RETURN(DXUTCompileFromFile(L"Shader/D3DSnowman.fx", nullptr, "none", "fx_5_0", dwShaderFlags, 0, &pEffectBuffer));
	hr = D3DX11CreateEffectFromMemory(pEffectBuffer->GetBufferPointer(), pEffectBuffer->GetBufferSize(), 0, pd3dDevice, &g_pEffect);
	SAFE_RELEASE(pEffectBuffer);
	if (FAILED(hr))
		return hr;

#endif

	// Obtain the variables
	g_pDirLightVariable = g_pEffect->GetVariableByName("DirLight");

	// Create Snowman
	g_Snowman1.Create(pd3dDevice, g_pEffect);
	// g_Snowman1.m_Car = g_Box.Geton();

	g_Snowman2.Create(pd3dDevice, g_pEffect);
	g_Snowman2.m_Car = g_Box.Geton();

	// Create Box
	g_Box.Create(pd3dDevice, g_pEffect);

	// Create Hills
	g_Hills.Create(pd3dDevice, g_pEffect);

	// Create Sky box
	g_Sky.Create(pd3dDevice, g_pEffect);

	// Initialize the view matrix
	g_ModelViewerCamera.SetViewParams({ 0.0f, 30.0f, -60.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f });
	g_Camera = &g_ModelViewerCamera;

	// Set Directional Lighting
	DirectionalLight mDirLight;
	mDirLight.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);			// 由光源发射的环境光
	mDirLight.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);			// 由光源发射的漫反射光，最后一个参数用于alpha混合
	mDirLight.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);			// 由光源发射的高光
	mDirLight.Direction = XMFLOAT3(0.57735f, -0.57735f, -0.57735f); // 灯光方向

	// Set Material
	Material mMaterial;
	mMaterial.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);			// 材质对环境光的作用
	mMaterial.Diffuse = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);			// 材质对漫反射光的作用
	mMaterial.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);			// 材质对高光的作用，最后一个参数表示圆锥体范围
	mMaterial.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);			// 反射率

	g_Snowman1.SetMaterial(mMaterial);
	g_Snowman2.SetMaterial(mMaterial);
	g_Box.SetMaterial(mMaterial);

	mMaterial.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);			// 材质对环境光的作用
	mMaterial.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);			// 材质对漫反射光的作用
	mMaterial.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);			// 材质对高光的作用，最后一个参数表示圆锥体范围
	mMaterial.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);			// 反射率

	g_Hills.SetMaterial(mMaterial);

	// Update constant buffer chat nerver changes
	g_pDirLightVariable->SetRawValue(&mDirLight, 0, sizeof(mDirLight));

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	V_RETURN(pd3dDevice->CreateSamplerState(&sampDesc, &g_pSamplerLinear));

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
										  const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	// Setup the camera's projection parameters
	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	g_Snowman1.m_Camera.SetProjParams(XM_PI / 4, fAspectRatio, 0.1f, 5000.0f);
	g_ModelViewerCamera.SetProjParams(XM_PI / 4, fAspectRatio, 0.1f, 5000.0f);
	g_ModelViewerCamera.SetWindow(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
	g_ModelViewerCamera.SetButtonMasks(MOUSE_LEFT_BUTTON, MOUSE_WHEEL, MOUSE_MIDDLE_BUTTON);

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
								  double fTime, float fElapsedTime, void* pUserContext )
{
	// Clear the back buffer
	auto pRTV = DXUTGetD3D11RenderTargetView();
	pd3dImmediateContext->ClearRenderTargetView(pRTV, Colors::MidnightBlue);

	// Clear the depth stencil
	auto pDSV = DXUTGetD3D11DepthStencilView();
	pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0, 0);

	g_Hills.Draw(pd3dImmediateContext, g_Camera);
	g_Sky.Draw(pd3dImmediateContext, g_Camera);
	g_Box.Draw(pd3dImmediateContext, g_Camera);
	g_Snowman2.Draw(pd3dImmediateContext, g_Camera);

	if (!firstPersonFlag)
		g_Snowman1.Draw(pd3dImmediateContext, g_Camera);
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice(void* pUserContext)
{
	g_Snowman1.SafeDestory();
	g_Snowman2.SafeDestory();
	g_Box.SafeDestory();
	g_Hills.SafeDestory();
	g_Sky.SafeDestory();
	DXUTGetGlobalResourceCache().OnDestroyDevice();
	SAFE_RELEASE(g_pSamplerLinear);
	SAFE_RELEASE(g_pEffect);
}


int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
	// Enable run-time memory check for debug builds.
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// Set general DXUT callbacks
	DXUTSetCallbackFrameMove( OnFrameMove );
	DXUTSetCallbackKeyboard( OnKeyboard );
	DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
	DXUTSetCallbackDeviceRemoved( OnDeviceRemoved );

	// Set the D3D11 DXUT callbacks. Remove these sets if the app doesn't need to support D3D11
	DXUTSetCallbackD3D11DeviceAcceptable( IsD3D11DeviceAcceptable );
	DXUTSetCallbackD3D11DeviceCreated( OnD3D11CreateDevice );
	DXUTSetCallbackD3D11SwapChainResized( OnD3D11ResizedSwapChain );
	DXUTSetCallbackD3D11FrameRender( OnD3D11FrameRender );
	DXUTSetCallbackD3D11SwapChainReleasing( OnD3D11ReleasingSwapChain );
	DXUTSetCallbackD3D11DeviceDestroyed( OnD3D11DestroyDevice );

	// Perform any application-level initialization here

	DXUTInit(true, true, nullptr); // Parse the command line, show msgboxes on error, no extra command line params
	DXUTSetCursorSettings(true, true); // Show the cursor and clip it when in full screen

	DXUTCreateWindow(L"Snowman");

	// Only require 10-level hardware or later
	DXUTCreateDevice(D3D_FEATURE_LEVEL_10_0, true, 800, 600);
	DXUTMainLoop(); // Enter into the DXUT render loop

					// Perform any application-level cleanup here

	return DXUTGetExitCode();
}
