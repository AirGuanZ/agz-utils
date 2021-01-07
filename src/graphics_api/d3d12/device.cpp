#ifdef AGZ_ENABLE_D3D12

#include <dxgi1_4.h>
#include <dxgidebug.h>

#include <agz-utils/graphics_api/d3d12/device.h>
#include <agz-utils/system.h>

AGZ_D3D12_BEGIN

Device::Device(bool graphicsQueue, bool computeQueue)
{
    ComPtr<IDXGIFactory4> factory;
    AGZ_D3D12_CHECK_HR_MSG(
        "failed to create dxgi factory",
        CreateDXGIFactory(
            IID_PPV_ARGS(factory.GetAddressOf())));
    factory_ = factory;

    factory->EnumAdapters(0, adapter_.GetAddressOf());

    AGZ_D3D12_CHECK_HR_MSG(
        "failed to create d3d12 device",
        D3D12CreateDevice(
            adapter_.Get(), D3D_FEATURE_LEVEL_12_0,
            IID_PPV_ARGS(device_.GetAddressOf())));

    if(graphicsQueue)
    {
        D3D12_COMMAND_QUEUE_DESC desc;
        desc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 0;

        AGZ_D3D12_CHECK_HR_MSG(
            "failed to create d3d12 graphics command queue",
            device_->CreateCommandQueue(
                &desc, IID_PPV_ARGS(graphicsQueue_.GetAddressOf())));
    }

    if(computeQueue)
    {
        D3D12_COMMAND_QUEUE_DESC desc;
        desc.Type     = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 0;

        AGZ_D3D12_CHECK_HR_MSG(
            "failed to create d3d12 graphics command queue",
            device_->CreateCommandQueue(
                &desc, IID_PPV_ARGS(computeQueue_.GetAddressOf())));
    }

    queueWaiter_ = std::make_unique<CommandQueueWaiter>(device_.Get());
}

Device::~Device()
{
    waitForIdle();

    queueWaiter_.reset();

    graphicsQueue_.Reset();
    computeQueue_.Reset();

    device_.Reset();
    adapter_.Reset();

#ifdef AGZ_DEBUG

    ComPtr<IDXGIDebug> dxgiDebug;
    if(SUCCEEDED(DXGIGetDebugInterface1(
        0, IID_PPV_ARGS(dxgiDebug.GetAddressOf()))))
    {
        dxgiDebug->ReportLiveObjects(
            DXGI_DEBUG_ALL,
            DXGI_DEBUG_RLO_FLAGS(
                DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
    }

#else

    AGZ_UNACCESSED(DXGI_DEBUG_RLO_SUMMARY);

#endif
}

void Device::waitForIdle()
{
    if(graphicsQueue_)
        queueWaiter_->waitIdle(graphicsQueue_.Get());
    if(computeQueue_)
        queueWaiter_->waitIdle(computeQueue_.Get());
}

ComPtr<ID3D12CommandQueue> Device::createCopyQueue() const
{
    D3D12_COMMAND_QUEUE_DESC desc;
    desc.Type     = D3D12_COMMAND_LIST_TYPE_COPY;
    desc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

    ComPtr<ID3D12CommandQueue> ret;
    AGZ_D3D12_CHECK_HR(
        device_->CreateCommandQueue(
            &desc, IID_PPV_ARGS(ret.GetAddressOf())));

    return ret;
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
