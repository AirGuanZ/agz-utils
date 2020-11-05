#pragma once

AGZ_D3D12_BEGIN

inline D3D12Context::D3D12Context(
    const WindowDesc    &windowDesc,
    const SwapChainDesc &swapChainDesc,
    uint32_t             GPUDescHeapSize,
    bool                 enableComputeQueue)
    : window_(windowDesc),
      device_(true, enableComputeQueue),
      swapChain_(swapChainDesc, window_, device_),
      descAlloc_(device_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true,
                 GPUDescHeapSize, swapChainDesc.imageCount),
      imgui_(window_, swapChain_, device_, descAlloc_.allocStatic()),
      frameFence_(device_, device_.getGraphicsQueue(), swapChainDesc.imageCount)
{
    
}

inline D3D12Context::~D3D12Context()
{
    device_.waitForIdle();
}

inline void D3D12Context::startFrame(bool imgui, bool waitForFocus)
{
    window_.doEvents();
    if(waitForFocus)
        window_.waitForFocus();

    frameFence_.startFrame(swapChain_.getImageIndex());
    descAlloc_.startFrame(swapChain_.getImageIndex());

    if(imgui)
        imgui_.newFrame();
}

inline void D3D12Context::endFrame()
{
    frameFence_.endFrame();
}

inline HWND D3D12Context::getWindowHandle() noexcept
{
    return window_.getWindowHandle();
}

inline void D3D12Context::doEvents()
{
    window_.doEvents();
}

inline bool D3D12Context::isInFocus() const noexcept
{
    return window_.isInFocus();
}

inline void D3D12Context::waitForFocus()
{
    window_.waitForFocus();
}

inline void D3D12Context::setMaximized()
{
    window_.setMaximized();
}

inline Input *D3D12Context::getInput() noexcept
{
    return window_.getInput();
}

inline int D3D12Context::getClientWidth() const noexcept
{
    return window_.getClientWidth();
}

inline int D3D12Context::getClientHeight() const noexcept
{
    return window_.getClientHeight();
}

inline Int2 D3D12Context::getClientSize() const noexcept
{
    return window_.getClientSize();
}

inline bool D3D12Context::getCloseFlag() const noexcept
{
    return window_.getCloseFlag();
}

inline void D3D12Context::setCloseFlag(bool closeFlag) noexcept
{
    window_.setCloseFlag(closeFlag);
}

inline void D3D12Context::waitForIdle()
{
    device_.waitForIdle();
}

inline IDXGIFactory *D3D12Context::getFactory() noexcept
{
    return device_.getFactory();
}

inline IDXGIAdapter *D3D12Context::getAdapter() noexcept
{
    return device_.getAdapter();
}

inline ID3D12Device *D3D12Context::getDevice() noexcept
{
    return device_.getDevice();
}

inline D3D12Context::operator struct ID3D12Device*() noexcept
{
    return device_;
}

inline ID3D12CommandQueue *D3D12Context::getGraphicsQueue() noexcept
{
    return device_.getGraphicsQueue();
}

inline ID3D12CommandQueue *D3D12Context::getComputeQueue() noexcept
{
    return device_.getComputeQueue();
}

inline ResourceManager D3D12Context::createResourceManager() const
{
    return device_.createResourceManager();
}

inline ComPtr<ID3D12CommandQueue> D3D12Context::createCopyQueue() const
{
    return device_.createCopyQueue();
}

inline DXGI_FORMAT D3D12Context::getFramebufferFormat() const noexcept
{
    return swapChain_.getImageFormat();
}

inline int D3D12Context::getFramebufferCount() const noexcept
{
    return swapChain_.getImageCount();
}

inline int D3D12Context::getFramebufferIndex() const noexcept
{
    return swapChain_.getImageIndex();
}

inline const ComPtr<ID3D12Resource> &D3D12Context::getFramebuffer() const noexcept
{
    return swapChain_.getImage();
}

inline D3D12_CPU_DESCRIPTOR_HANDLE D3D12Context::getRenderTargetView() const noexcept
{
    return swapChain_.getRenderTargetView();
}

inline UINT D3D12Context::getRenderTargetViewDescSize() const noexcept
{
    return swapChain_.getImageDescSize();
}

inline void D3D12Context::swapFramebuffers()
{
    swapChain_.swapBuffers();
}

inline int D3D12Context::getFramebufferWidth() const noexcept
{
    return swapChain_.getWidth();
}

inline int D3D12Context::getFramebufferHeight() const noexcept
{
    return swapChain_.getHeight();
}

inline Int2 D3D12Context::getFramebufferSize() const noexcept
{
    return swapChain_.getSize();
}

inline float D3D12Context::getFramebufferWOverH() const noexcept
{
    return swapChain_.getWOverH();
}

inline const D3D12_VIEWPORT &D3D12Context::getDefaultViewport() const noexcept
{
    return swapChain_.getDefaultViewport();
}

inline const D3D12_RECT &D3D12Context::getDefaultScissorRect() const noexcept
{
    return swapChain_.getDefaultScissorRect();
}

inline Descriptor D3D12Context::allocStatic()
{
    return descAlloc_.allocStatic();
}

inline void D3D12Context::freeStatic(Descriptor descriptor)
{
    descAlloc_.freeStatic(descriptor);
}

inline DescriptorRange D3D12Context::allocStaticRange(uint32_t count)
{
    return descAlloc_.allocStaticRange(count);
}

inline void D3D12Context::freeRangeStatic(const DescriptorRange &range)
{
    descAlloc_.freeRangeStatic(range);
}

inline DescriptorRange D3D12Context::allocTransientRange(uint32_t size)
{
    return descAlloc_.allocTransientRange(size);
}

inline TransientDescriptorAllocator D3D12Context::createTransientAllocator(
    uint32_t initialSize, float sizeIncFactor)
{
    return descAlloc_.createTransientAllocator(initialSize, sizeIncFactor);
}

inline DescriptorAllocator &D3D12Context::getDescriptorAllocator() noexcept
{
    return descAlloc_;
}

inline ID3D12DescriptorHeap *D3D12Context::getGPUDescHeap() const noexcept
{
    return descAlloc_.getHeap();
}

inline void D3D12Context::renderImGui(ID3D12GraphicsCommandList *cmdList)
{
    imgui_.render(cmdList);
}

AGZ_D3D12_END
