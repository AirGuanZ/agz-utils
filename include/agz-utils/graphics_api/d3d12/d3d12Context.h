#pragma once

#include <agz-utils/graphics_api/d3d12/device.h>
#include <agz-utils/graphics_api/d3d12/descriptorAllocator.h>
#include <agz-utils/graphics_api/d3d12/frameFence.h>
#include <agz-utils/graphics_api/d3d12/imguiIntegration.h>
#include <agz-utils/graphics_api/d3d12/resourceManager.h>
#include <agz-utils/graphics_api/d3d12/swapChain.h>
#include <agz-utils/graphics_api/d3d12/window.h>

AGZ_D3D12_BEGIN

class D3D12Context
{
public:

    D3D12Context(
        const WindowDesc    &windowDesc,
        const SwapChainDesc &swapChainDesc,
        uint32_t             GPUDescHeapSize    = 1024,
        bool                 enableImGui        = true,
        bool                 enableComputeQueue = false);

    ~D3D12Context();

    void startFrame(bool waitForFocus = true);

    void endFrame();

    // window

    HWND getWindowHandle() noexcept;

    void doEvents();

    bool isInFocus() const noexcept;

    void waitForFocus();

    void setMaximized();

    Input *getInput() noexcept;

    int getClientWidth() const noexcept;

    int getClientHeight() const noexcept;

    Int2 getClientSize() const noexcept;

    bool getCloseFlag() const noexcept;

    void setCloseFlag(bool closeFlag) noexcept;

    AGZ_DECL_EVENT_SENDER_HANDLER(window_, WindowCloseEvent)
    AGZ_DECL_EVENT_SENDER_HANDLER(window_, WindowGetFocusEvent)
    AGZ_DECL_EVENT_SENDER_HANDLER(window_, WindowLostFocusEvent)
    AGZ_DECL_EVENT_SENDER_HANDLER(window_, WindowResizeEvent)

    // device

    void waitForIdle();

    IDXGIFactory *getFactory() noexcept;

    IDXGIAdapter *getAdapter() noexcept;

    ID3D12Device *getDevice() noexcept;

    ID3D12CommandQueue *getGraphicsQueue() noexcept;

    ID3D12CommandQueue *getComputeQueue()  noexcept;

    ResourceManager createResourceManager() const;

    ComPtr<ID3D12CommandQueue> createCopyQueue() const;

    ImmediateCommandList createImmediateCommandList(
        D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const;

    // swap chain

    DXGI_FORMAT getFramebufferFormat() const noexcept;

    int getFramebufferCount() const noexcept;

    int getFramebufferIndex() const noexcept;

    const ComPtr<ID3D12Resource> &getFramebuffer() const noexcept;

    const ComPtr<ID3D12Resource> &getFramebuffer(int index) const noexcept;

    void swapFramebuffers();

    int getFramebufferWidth() const noexcept;

    int getFramebufferHeight() const noexcept;

    Int2 getFramebufferSize() const noexcept;

    float getFramebufferWOverH() const noexcept;

    const D3D12_VIEWPORT &getDefaultViewport() const noexcept;

    const D3D12_RECT &getDefaultScissorRect() const noexcept;
    
    AGZ_DECL_EVENT_SENDER_HANDLER(swapChain_, SwapChainPreResizeEvent)
    AGZ_DECL_EVENT_SENDER_HANDLER(swapChain_, SwapChainPostResizeEvent)

    // gpu heap

    DescriptorAllocator &getDescriptorAllocator() noexcept;

    ID3D12DescriptorHeap *getGPUDescHeap() const noexcept;

    Descriptor allocStatic();

    void freeStatic(Descriptor descriptor);

    DescriptorRange allocStaticRange(uint32_t count);

    void freeRangeStatic(const DescriptorRange &range);

    DescriptorRange allocTransientRange(uint32_t size);

    TransientDescriptorAllocator createTransientAllocator(
        uint32_t initialSize = 64, float sizeIncFactor = 1.5f);

    // imgui

    void renderImGui(ID3D12GraphicsCommandList *cmdList);

    rg::Pass *addImGuiToRenderGraph(
        rg::Graph    &graph,
        rg::Resource *renderTarget,
        int           thread = 0,
        int           queue  = 0);

    // rsc mgr

    ResourceManager &getResourceManager();

    UniqueResource create(
        D3D12_HEAP_TYPE            heapType,
        const D3D12_RESOURCE_DESC &desc,
        D3D12_RESOURCE_STATES      initialState);

    UniqueResource create(
        D3D12_HEAP_TYPE            heapType,
        const D3D12_RESOURCE_DESC &desc,
        D3D12_RESOURCE_STATES      initialState,
        const D3D12_CLEAR_VALUE   &clearValue);

    Buffer createDefaultBuffer(
        size_t                byteSize,
        D3D12_RESOURCE_STATES initState);

    Buffer createUploadBuffer(size_t byteSize);

private:

    // warning: check d3d12Context.inl before changing the order of decls

    Window              window_;
    Device              device_;
    SwapChain           swapChain_;
    DescriptorAllocator descAlloc_;
    ImGuiIntegration    imgui_;
    FrameFence          frameFence_;
    ResourceManager     rscMgr_;
};

inline D3D12Context::D3D12Context(
    const WindowDesc    &windowDesc,
    const SwapChainDesc &swapChainDesc,
    uint32_t             GPUDescHeapSize,
    bool                 enableImGui,
    bool                 enableComputeQueue)
    : window_(windowDesc),
      device_(true, enableComputeQueue),
      swapChain_(swapChainDesc, window_, device_),
      descAlloc_(device_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true,
                 GPUDescHeapSize, swapChainDesc.imageCount),
      frameFence_(device_, device_.getGraphicsQueue(), swapChainDesc.imageCount),
      rscMgr_(device_, device_.getAdapter())
{
    if(enableImGui)
    {
        imgui_.initialize(
            window_, swapChain_, device_, descAlloc_.allocStatic());
    }
}

inline D3D12Context::~D3D12Context()
{
    device_.waitForIdle();
}

inline void D3D12Context::startFrame(bool waitForFocus)
{
    window_.doEvents();
    if(waitForFocus)
        window_.waitForFocus();

    frameFence_.startFrame(swapChain_.getImageIndex());
    descAlloc_.startFrame(swapChain_.getImageIndex());

    if(imgui_.isAvailable())
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

inline ImmediateCommandList D3D12Context::createImmediateCommandList(
    D3D12_COMMAND_LIST_TYPE type) const
{
    return device_.createImmediateCommandList(type);
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

inline const ComPtr<ID3D12Resource> &D3D12Context::getFramebuffer(int index) const noexcept
{
    return swapChain_.getImage(index);
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

inline rg::Pass *D3D12Context::addImGuiToRenderGraph(
    rg::Graph    &graph,
    rg::Resource *renderTarget,
    int           thread,
    int           queue)
{
    return imgui_.addToRenderGraph(graph, renderTarget, thread, queue);
}

inline ResourceManager &D3D12Context::getResourceManager()
{
    return rscMgr_;
}

inline UniqueResource D3D12Context::create(
    D3D12_HEAP_TYPE            heapType,
    const D3D12_RESOURCE_DESC &desc,
    D3D12_RESOURCE_STATES      initialState)
{
    return rscMgr_.create(heapType, desc, initialState);
}

inline UniqueResource D3D12Context::create(
    D3D12_HEAP_TYPE            heapType,
    const D3D12_RESOURCE_DESC &desc,
    D3D12_RESOURCE_STATES      initialState,
    const D3D12_CLEAR_VALUE   &clearValue)
{
    return rscMgr_.create(heapType, desc, initialState, clearValue);
}

inline Buffer D3D12Context::createDefaultBuffer(
    size_t                byteSize,
    D3D12_RESOURCE_STATES initState)
{
    return rscMgr_.createDefaultBuffer(byteSize, initState);
}

inline Buffer D3D12Context::createUploadBuffer(size_t byteSize)
{
    return rscMgr_.createUploadBuffer(byteSize);
}

AGZ_D3D12_END
