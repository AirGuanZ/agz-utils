#ifdef AGZ_ENABLE_D3D12

#include <agz/utility/graphics_api/d3d12/device.h>
#include <agz/utility/graphics_api/d3d12/swapChain.h>
#include <agz/utility/graphics_api/d3d12/window.h>

AGZ_D3D12_BEGIN

SwapChain::SwapChain(
    const SwapChainDesc &desc,
    Window              &window,
    Device              &device)
    : device_(&device),
      vsync_(desc.vsync),
      imageFormat_(desc.imageFormat),
      defaultViewport_{},
      defaultRect_{}
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc;

    swapChainDesc.BufferDesc.Width                   = window.getClientWidth();
    swapChainDesc.BufferDesc.Height                  = window.getClientHeight();
    swapChainDesc.BufferDesc.Format                  = desc.imageFormat;
    swapChainDesc.BufferDesc.RefreshRate.Numerator   = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

    swapChainDesc.SampleDesc.Count   = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    swapChainDesc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount  = desc.imageCount;
    swapChainDesc.OutputWindow = window.getWindowHandle();
    swapChainDesc.Windowed     = true;
    swapChainDesc.SwapEffect   = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags        = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    IDXGISwapChain *tSwapChain;
    AGZ_D3D12_CHECK_HR_MSG(
        "failed to create dxgi swap chain",
        device.getFactory()->CreateSwapChain(
            device.getGraphicsQueue(), &swapChainDesc, &tSwapChain));

    swapChain_.Attach(static_cast<IDXGISwapChain3 *>(tSwapChain));

    AGZ_SCOPE_GUARD({
        if(desc.fullscreen)
            swapChain_->SetFullscreenState(true, nullptr);
    });

    imageIndex_ = swapChain_->GetCurrentBackBufferIndex();

    RTVHeap_ = RawDescriptorHeap(
        device.getDevice(), desc.imageCount,
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false);

    images_.resize(desc.imageCount);

    for(int i = 0; i < desc.imageCount; ++i)
    {
        AGZ_D3D12_CHECK_HR_MSG(
            "failed to get swap chain buffer",
            swapChain_->GetBuffer(
                i, IID_PPV_ARGS(images_[i].GetAddressOf())));

        device.getDevice()->CreateRenderTargetView(
            images_[i].Get(), nullptr, RTVHeap_.getCPUHandle(i));
    }

    defaultViewport_.TopLeftX = 0;
    defaultViewport_.TopLeftY = 0;
    defaultViewport_.MinDepth = 0;
    defaultViewport_.MaxDepth = 1;

    defaultRect_.left = 0;
    defaultRect_.top  = 0;

    setImageSize(window.getClientWidth(), window.getClientHeight());

    window.attach(std::make_shared<WindowResizeHandler>(
        [this](const WindowResizeEvent &e)
    {
        _resize(e.width, e.height);
    }));
}

SwapChain::~SwapChain()
{
    device_->waitForIdle();

    exiting_ = true;
    swapChain_->SetFullscreenState(FALSE, nullptr);
}

DXGI_FORMAT SwapChain::getImageFormat() const noexcept
{
    return imageFormat_;
}

int SwapChain::getImageCount() const noexcept
{
    return static_cast<int>(images_.size());
}

int SwapChain::getImageIndex() const noexcept
{
    return imageIndex_;
}

const ComPtr<ID3D12Resource> &SwapChain::getImage() const noexcept
{
    return images_[imageIndex_];
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::getRenderTargetView() const noexcept
{
    return RTVHeap_.getCPUHandle(imageIndex_);
}

UINT SwapChain::getImageDescSize() const noexcept
{
    return RTVHeap_.getDescSize();
}

void SwapChain::swapBuffers()
{
    swapChain_->Present(vsync_ ? 1 : 0, 0);
    imageIndex_ = static_cast<int>(swapChain_->GetCurrentBackBufferIndex());
}

int SwapChain::getWidth() const noexcept
{
    return imageSize_.x;
}

int SwapChain::getHeight() const noexcept
{
    return imageSize_.y;
}

Int2 SwapChain::getSize() const noexcept
{
    return imageSize_;
}

float SwapChain::getWOverH() const noexcept
{
    return static_cast<float>(imageSize_.x) / imageSize_.y;
}

const D3D12_VIEWPORT &SwapChain::getDefaultViewport() const noexcept
{
    return defaultViewport_;
}

const D3D12_RECT &SwapChain::getDefaultScissorRect() const noexcept
{
    return defaultRect_;
}

void SwapChain::_resize(int width, int height)
{
    if(exiting_)
        return;

    device_->waitForIdle();

    eventSender_.send(SwapChainPreResizeEvent{});

    setImageSize(width, height);

    for(auto &b : images_)
        b.Reset();

    swapChain_->ResizeBuffers(
        static_cast<UINT>(images_.size()),
        imageSize_.x, imageSize_.y, imageFormat_,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

    imageIndex_ = static_cast<int>(swapChain_->GetCurrentBackBufferIndex());

    for(UINT i = 0; i < images_.size(); ++i)
    {
        AGZ_D3D12_CHECK_HR_MSG(
            "failed to get swap chain buffer",
            swapChain_->GetBuffer(
                i, IID_PPV_ARGS(images_[i].GetAddressOf())));

        device_->getDevice()->CreateRenderTargetView(
            images_[i].Get(), nullptr, RTVHeap_.getCPUHandle(i));
    }

    eventSender_.send(SwapChainPostResizeEvent{ width, height });
}

void SwapChain::setImageSize(int width, int height) noexcept
{
    imageSize_ = { width, height };

    defaultViewport_.Width  = static_cast<float>(width);
    defaultViewport_.Height = static_cast<float>(height);

    defaultRect_.right  = width;
    defaultRect_.bottom = height;
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
