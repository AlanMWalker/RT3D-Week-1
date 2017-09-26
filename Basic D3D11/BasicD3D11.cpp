// *************************************************************************************
// File: BasicD3D11.cpp
//		 Originally based on Tutorial04.cpp from the DirectX11 samples
//		 [Copyright (c) Microsoft Corporation. All rights reserved.]
//
// 1. Initialises and registers a new window using the Win32SDK
// 2. Creates a D3D11 Device and DeviceContext
// 3. Creates a SwapChain and RenderTargetView onto the Back Buffer
//	  and binds this to the OUTPUT MERGER
// 4. Compiles and creates the PIXEL SHADER
// 5. Sets up the viewport for the RASTERISER
// 6. Compiles and creates the VERTEX SHADER and its Constant Buffer
// 7. Creates an InputLayout and binds this to the INPUT ASSEMBLER.
//    Creates sets the Vertex and Input buffers for the INPUT ASSEMBLER
//
//    All of this just so that you can render a rotating cube :-)
//
// *************************************************************************************
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "SimpleMath.h"

#include "include\VertexDefinitions.h"
#include "include\cube.h"

using namespace DirectX::SimpleMath;



// *************************************************************************************
// Global Variables
// *************************************************************************************
HINSTANCE               g_hInst = NULL;
HWND                    g_hWnd = NULL;
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;

ID3D11Device*           g_pD3DDevice = NULL;
ID3D11DeviceContext*    g_pImmediateContext = NULL;

// *************************************************************************************
// Forward declarations
// *************************************************************************************

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice(IDXGISwapChain* &pSwapChain);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
HRESULT InitInputAssembler(ID3DBlob* pVSBlob, ID3D11InputLayout* &pVertexLayout, ID3D11Buffer* &pVertexBuffer, ID3D11Buffer* &pIndexBuffer);
HRESULT InitVertexShader(ID3DBlob* &pVSBlob, ID3D11VertexShader* &pVertexShader, ID3D11Buffer* &pConstantBuffer);
HRESULT InitRasteriser();
HRESULT InitPixelShader(ID3D11PixelShader* &pPixelShader);
HRESULT InitOutputMerger(IDXGISwapChain* pSwapChain, ID3D11RenderTargetView* &pRenderTargetView);

// *************************************************************************************
// WinMain: The entry point to a windows program
//			Initializes everything and goes into a message processing loop.
//			Note that wWinMain (with an additional w) is just signifying that UNICODE 
//			characters shuld be used for the command line string rather than ASCII.
//
// hInstance		= Handle to the current instance
// hPrevInstance	= Handle to any previous instances
// lpCmdLine		= Pointer to command line argument string
// nCmdShow			= SW_SHOWNORMAL by default
// *************************************************************************************
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	// These macros get rid of compiler warning about the unreferenced parameters to this function
	// which are not required for our program, but often might be.
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// First initialise the window using the Win32 API 
	if (FAILED(InitWindow(hInstance, nCmdShow)))
		return 0;
	//Vector3(0.174f, 0.174f, 0.174f)
	Cube cubeyBaby(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
	// Retrieve the coordinates of a window's client area so that we can create  
	// an appropriate aspect ratio for the projection matrix
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	// Initialize the world matrix
	Matrix mWorld;

	// Initialize the view matrix
	Vector3 Eye = Vector3(0.0f, 2.0f, -5.0f);
	Vector3 At = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 Up = Vector3(0.0f, 1.0f, 0.0f);
	Matrix mView = Matrix::CreateLookAt(Eye, At, Up);

	// Initialize the projection matrix
	Matrix mProjection = Matrix::CreatePerspectiveFieldOfView(3.142 / 2.0, width / (FLOAT)height, 0.01f, 100.0f);

	// Pointers to the D3D11 Device and DevideContext COM objects have been declared as
	// global variables, because they are used everywhere, but the other relevant COM objects
	// are declared locally here so you can see how they are created and used by different 
	// stages of the pipeline.

	IDXGISwapChain*         pSwapChain = NULL;
	ID3D11RenderTargetView* pRenderTargetView = NULL;
	ID3DBlob*				pVSBlob = NULL;
	ID3D11VertexShader*     pVertexShader = NULL;
	ID3D11PixelShader*      pPixelShader = NULL;
	ID3D11InputLayout*      pVertexLayout = NULL;
	ID3D11Buffer*           pVertexBuffer = NULL;
	ID3D11Buffer*           pIndexBuffer = NULL;
	ID3D11Buffer*           pConstantBuffer = NULL;

	// Initialise the DirectX11 devices and create the Swap Chain
	InitDevice(pSwapChain);

	// The Swap Chain is used to create and return a RenderTargetView bound to the Output Merger
	InitOutputMerger(pSwapChain, pRenderTargetView);

	// The shader program is loaded and compiled into a binary blob which is used create and return a Pixel Shader
	InitPixelShader(pPixelShader);

	// Sets up the viewport for the Rasteriser
	InitRasteriser();

	// The shader program is loaded and compiled into a binary blob which is used create and return a Vertex Shader.
	// The vertex shader's binary blob is also returned as this is needed by the Input Assembler to determine if the
	// input layout matches the input signature of the shader code. A Constant Buffer is created and returned to 
	// contain the constant data used for all vertices (primarily the world, view and projection matrices).
	InitVertexShader(pVSBlob, pVertexShader, pConstantBuffer);

	// An InputLayout is created from an element decriptor and bound to the Input Assembler. Vertex and Index
	// buffers  are created for the cubeand set as input to the Input Assembler
	InitInputAssembler(pVSBlob, pVertexLayout, pVertexBuffer, pIndexBuffer);

	// Main message loop
	MSG msg = { 0 };

	// Keep looping until the application is closed
	while (WM_QUIT != msg.message)
	{
		// Look to see if there is a message for this application to process 
		// Returns immediately without blocking if there isn't a message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Translate virtual key message into character codes
			TranslateMessage(&msg);
			// Send the message onto the windows procedure (defined by the windows class during registration)
			DispatchMessage(&msg);
		}
		else
		{
			// Animate the cube by rotating it in the Y axis
			static float r = 0.0f;
			r += 0.001f;
			mWorld = Matrix::CreateRotationY(r);
			cubeyBaby.rotateY(0.001f);
			cubeyBaby.rotateX(0.001f);
			cubeyBaby.rotateZ(0.001f);
			// Clear the back buffer to a dark blue
			float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
			g_pImmediateContext->ClearRenderTargetView(pRenderTargetView, ClearColor);

			// Update the constant buffer with the latest world, view and projection matrix
			ConstantBuffer cb;
			cb.mWorld = cubeyBaby.getWorldMatrix().Transpose();//mWorld.Transpose();
			cb.mView = mView.Transpose();
			cb.mProjection = mProjection.Transpose();
			// This is sending data to the graphics card
			g_pImmediateContext->UpdateSubresource(pConstantBuffer, 0, NULL, &cb, 0, 0);

			// Render the triangles
			g_pImmediateContext->VSSetShader(pVertexShader, NULL, 0);
			g_pImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
			g_pImmediateContext->PSSetShader(pPixelShader, NULL, 0);
			g_pImmediateContext->DrawIndexed(36, 0, 0);        // 36 vertices needed for 12 triangles in a triangle list

			// Present our back buffer to our front buffer
			pSwapChain->Present(0, 0);

		}
	}

	// Release all of the COM objects associated with this application
	if (g_pImmediateContext) g_pImmediateContext->ClearState();
	if (pConstantBuffer) pConstantBuffer->Release();
	if (pVertexBuffer) pVertexBuffer->Release();
	if (pIndexBuffer) pIndexBuffer->Release();
	if (pVertexLayout) pVertexLayout->Release();
	if (pVertexShader) pVertexShader->Release();
	if (pPixelShader) pPixelShader->Release();
	if (pRenderTargetView) pRenderTargetView->Release();
	if (pSwapChain) pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pD3DDevice) g_pD3DDevice->Release();

	return (int)msg.wParam;
}


// *************************************************************************************
// InitWindow:	Goes through the standard Win32 motions to create a new window
// hInstance		= Handle to the current instance
// nCmdShow			= SW_SHOWNORMAL by default
// *************************************************************************************
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// First we need to create an instance of the WNDCLASSEX structure which defines a 
	// windows "class". This is a template for the style of window that you want to 
	// create, including its behaviour and appearance. Every window must be based on 
	// a windows class which has been previously registered with the system.

	// Create the structure to define the class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);

	// Redraws the entire window if a movement or size adjustment changes the width or height of the client area.
	wcex.style = CS_HREDRAW | CS_VREDRAW;

	// The windows procedure (function pointer) that we want to use to process messages (defined later on in this file)
	wcex.lpfnWndProc = WndProc;

	// Used if extra memory is required by the class or instances of it
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;

	// A handle to the instance that contains the window procedure for the class. 
	wcex.hInstance = hInstance;

	// Handles to icon, cursor, background and menu resources (NULL for default)
	wcex.hIcon = NULL;
	wcex.hIconSm = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;

	// The name we want to use for this class
	wcex.lpszClassName = L"CubeWindowClass";

	// Register the class with the system
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Now we can create an instance of this window class
	g_hInst = hInstance;
	RECT rc = { 0, 0, 640, 480 };
	// Calculates the required size of the window rectangle, based on the desired client-rectangle size
	// Otherwise your client area would end up smaller after some of it gets used for the title bar etc.
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window
	g_hWnd = CreateWindow(L"CubeWindowClass", // The name of the previously registered windows class
		L"Direct3D 11 Basic 3D Application", // The name displayed in the title bar
		WS_OVERLAPPEDWINDOW, //  (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
		CW_USEDEFAULT, // Horizontal position of window
		CW_USEDEFAULT, // Vertical postition of window
		rc.right - rc.left, // Width of the window
		rc.bottom - rc.top, // Height of the window
		NULL, // Handle to any parent window
		NULL, // Handle to any menu
		hInstance, // Handle to the instance associated with the window
		NULL); // Optional parameter to pass through as part of the WM_CREATE message

	if (!g_hWnd)
		return E_FAIL;

	// Show the window (nCmdShow = SW_SHOWNORMAL by default)
	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}


// *************************************************************************************
// InitDevice:	Creates the Direct3D "Device" which encapsulates our video adapter, a 
//				"Device Context" which provides access to GPU functionality and a 
//				"Swap Chain" to draw things to.
// *************************************************************************************
HRESULT InitDevice(IDXGISwapChain* &pSwapChain)
{
	HRESULT hr = S_OK;

	// Retrieve the coordinates of a window's client area so that we can create swap 
	// chain buffers of the same resolution.
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// An array of driver types allows us to specify our preferred driver and a range
	// of fallback preferences when it is not available.
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, // A hardware driver, which implements Direct3D features in hardware
		D3D_DRIVER_TYPE_WARP, // A high-performance software rasterizer supporting a subset of features
		D3D_DRIVER_TYPE_REFERENCE, // A slower, software rasterizer supporting all features
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	// Likewise an array with our preferred feature level and fallbacks
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	// Create the swap chain descriptor structure
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1; // This is the number of back buffers in the swap chain (so we have two including the front buffer)
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 24-bit colour with 4 x 8-bit channels (R,G,B,A)
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1; // 1/60th of a second refresh rate
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	// Go through our preferred driver types until we find one that works
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &pSwapChain, &g_pD3DDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

	return S_OK;
}


// *************************************************************************************
// InitOutputMerger:	Creates a RenderTargetView onto the back buffer and binds it to
//						the OutputMerger stage as the buffer it should draw to
// *************************************************************************************
HRESULT InitOutputMerger(IDXGISwapChain* pSwapChain, ID3D11RenderTargetView* &pRenderTargetView)
{
	HRESULT hr = S_OK;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = g_pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	g_pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);

	return S_OK;
}

// *************************************************************************************
// InitPixelShader:	Loads and compiles the shader program into a binary blob which is
//					then used to create the Pixel Shader  
// *************************************************************************************
HRESULT InitPixelShader(ID3D11PixelShader* &pPixelShader)
{
	HRESULT hr = S_OK;

	// Compile the pixel shader
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(L"basic.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_pD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &pPixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	return S_OK;
}


// *************************************************************************************
// InitPixelShader:	Sets up the viewport for the Rasteriser stage 
// *************************************************************************************
HRESULT InitRasteriser()
{
	// Retrieve the coordinates of a window's client area so that we can create  
	// a viewport of the same resolution.
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	//Set the viewport for the Rasteriser stage
	g_pImmediateContext->RSSetViewports(1, &vp);

	return S_OK;
}

// *************************************************************************************
// InitVertexShader: Loads and compiles the shader program into a binary blob which is 
//					 used create and return a Vertex Shader. The binary blob is also
//					 returned as this is needed by the Input Assembler to determine if 
//					 the input layout matches the input signature of the shader code. 
//					 A Constant Buffer is created and returned to contain the constant 
//					 data common to all vertices.
// *************************************************************************************
HRESULT InitVertexShader(ID3DBlob* &pVSBlob, ID3D11VertexShader* &pVertexShader, ID3D11Buffer* &pConstantBuffer)
{
	HRESULT hr = S_OK;

	// Compile the vertex shader
	pVSBlob = NULL;
	hr = CompileShaderFromFile(L"basic.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = g_pD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &pVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	// Create the constant buffer for passing data to the vertex shader
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pD3DDevice->CreateBuffer(&bd, NULL, &pConstantBuffer);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

// *************************************************************************************
// InitInputAssembler:	Creates an InputLayout for the geometry from an element decriptor 
//						and binds this to the Input Assembler. Creates Vertex and Index 
//						Buffers for the cube and sets these as input to the Input Assembler
// *************************************************************************************
HRESULT InitInputAssembler(ID3DBlob* pVSBlob, ID3D11InputLayout* &pVertexLayout, ID3D11Buffer* &pVertexBuffer, ID3D11Buffer* &pIndexBuffer)
{
	HRESULT hr = S_OK;

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = g_pD3DDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &pVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	g_pImmediateContext->IASetInputLayout(pVertexLayout);

	// Create vertex buffer
	SimpleVertex vertices[] =
	{
		{ Vector3(-1.0f, 1.0f, -1.0f), Vector4(0.25f, 0.35f, 0.0f, 1.0f) },
		{ Vector3(1.0f, 1.0f, -1.0f), Vector4(0.25f, 0.35f, 0.0f, 1.0f) },
		{ Vector3(1.0f, 1.0f, 1.0f), Vector4(0.5f, 0.7f, 0.0f, 1.0f) },
		{ Vector3(-1.0f, 1.0f, 1.0f), Vector4(0.5f, 0.7f, 0.0f, 1.0f) },
		{ Vector3(-1.0f, -1.0f, -1.0f), Vector4(0.25f, 0.35f, 0.0f, 1.0f) },
		{ Vector3(1.0f, -1.0f, -1.0f), Vector4(0.25f, 0.35f, 0.0f, 1.0f) },
		{ Vector3(1.0f, -1.0f, 1.0f), Vector4(0.5f, 0.7f, 0.0f, 1.0f) },
		{ Vector3(-1.0f, -1.0f, 1.0f), Vector4(0.5f, 0.7f, 0.0f, 1.0f) },
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	hr = g_pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer);
	if (FAILED(hr))
		return hr;

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	// Create index buffer
	WORD indices[] =
	{
		0,1,3,	3,1,2, // Face 1
		4,5,0,	0,5,1, // Face 2
		7,4,3,	3,4,0, // Face 3
		5,6,1,	1,6,2, // Face 4
		6,7,2,	2,7,3, // Face 5
		5,4,6,	6,4,7, // Face 6
	};

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	hr = g_pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer);
	if (FAILED(hr))
		return hr;

	// Set index buffer
	g_pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return S_OK;
}

// *************************************************************************************
// CompileShaderFromFile:	Helper for compiling shaders with D3DX11
// *************************************************************************************
HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}


// *************************************************************************************
// WndProc:	The windows callback procedure which is called every time the application 
//			receives a message
// *************************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


