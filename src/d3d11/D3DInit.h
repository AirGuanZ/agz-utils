#pragma once

#include <utility>

#include <d3d11.h>

#include <agz/utility/d3d11/Common.h>

AGZ_D3D11_BEGIN

std::pair<ID3D11Device*, ID3D11DeviceContext*> CreateD3D11Device();

IDXGISwapChain *CreateD3D11SwapChain(
    HWND windowHandle, int cliengWidth, int clientHeight,
    DXGI_FORMAT swapchainBufferFormat, int sampleCount, int sampleQuality,
    ID3D11Device *device);

ID3D11RenderTargetView *CreateD3D11RenderTargetView(IDXGISwapChain *swapChain, ID3D11Device *device);

std::pair<ID3D11Texture2D*, ID3D11DepthStencilView*> CreateD3D11DepthStencilBuffer(
    ID3D11Device *device, int clientWidth, int clientHeight,
    DXGI_FORMAT bufferFormat, int sampleCount, int sampleQuality);

AGZ_D3D11_END
