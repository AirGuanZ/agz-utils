#ifdef AGZ_ENABLE_D3D12

#include <dxgi1_3.h>
#include <dxgidebug.h>

#include <agz-utils/graphics_api/d3d12/debug.h>

AGZ_D3D12_BEGIN

void enableDebugLayer(bool gpuValidation)
{
    ComPtr<ID3D12Debug> debugController;
    if(SUCCEEDED(D3D12GetDebugInterface(
        IID_PPV_ARGS(debugController.GetAddressOf()))))
    {
        debugController->EnableDebugLayer();

        if(gpuValidation)
        {
            ComPtr<ID3D12Debug1> debug1;
            AGZ_D3D12_CHECK_HR(
                debugController->QueryInterface(
                    IID_PPV_ARGS(debug1.GetAddressOf())));
            debug1->SetEnableGPUBasedValidation(TRUE);
        }
    }
    else
        OutputDebugStringA("WARNING: Direct3D Debug Device is not available\n");

    ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
    if(SUCCEEDED(DXGIGetDebugInterface1(
        0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
    {
        dxgiInfoQueue->SetBreakOnSeverity(
            DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
        dxgiInfoQueue->SetBreakOnSeverity(
            DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
    }
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
