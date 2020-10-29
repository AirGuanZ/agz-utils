#pragma once

#include <agz/utility/graphics_api/d3d12/device.h>
#include <agz/utility/graphics_api/d3d12/frameFence.h>
#include <agz/utility/graphics_api/d3d12/imguiIntegration.h>
#include <agz/utility/graphics_api/d3d12/swapChain.h>
#include <agz/utility/graphics_api/d3d12/window.h>

AGZ_D3D12_BEGIN

class D3D12Context
{
public:

    D3D12Context(
        const WindowDesc    &windowDesc,
        const SwapChainDesc &swapChainDesc,
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

    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(window_, WindowCloseEvent)
    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(window_, WindowGetFocusEvent)
    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(window_, WindowLostFocusEvent)
    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(window_, WindowResizeEvent)

    // device

    void waitForIdle();

    IDXGIFactory *getFactory() noexcept;

    IDXGIAdapter *getAdapter() noexcept;

    ID3D12Device *getDevice() noexcept;

    operator ID3D12Device *() noexcept;

    ID3D12CommandQueue *getGraphicsQueue() noexcept;

    ID3D12CommandQueue *getComputeQueue()  noexcept;

    ResourceManager createResourceManager() const;

    // swap chain

    DXGI_FORMAT getFramebufferFormat() const noexcept;

    int getFramebufferCount() const noexcept;

    int getFramebufferIndex() const noexcept;

    const ComPtr<ID3D12Resource> &getFramebuffer() const noexcept;

    D3D12_CPU_DESCRIPTOR_HANDLE getRenderTargetView() const noexcept;

    UINT getRenderTargetViewDescSize() const noexcept;

    void swapFramebuffers();

    int getFramebufferWidth() const noexcept;

    int getFramebufferHeight() const noexcept;

    Int2 getFramebufferSize() const noexcept;

    float getFramebufferWOverH() const noexcept;

    const D3D12_VIEWPORT &getDefaultViewport() const noexcept;

    const D3D12_RECT &getDefaultScissorRect() const noexcept;
    
    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(swapChain_, SwapChainPreResizeEvent)
    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(swapChain_, SwapChainPostResizeEvent)

    // imgui

    void renderImGui(ID3D12GraphicsCommandList *cmdList);

    ID3D12DescriptorHeap *getImGuiGPUHeap() noexcept;

private:

    // warning: check d3d12Context.inl before changing the order of decls

    Window           window_;
    Device           device_;
    SwapChain        swapChain_;
    ImGuiIntegration imgui_;
    FrameFence       frameFence_;
};

AGZ_D3D12_END

#include <agz/utility/graphics_api/d3d12/d3d12Context.inl>
