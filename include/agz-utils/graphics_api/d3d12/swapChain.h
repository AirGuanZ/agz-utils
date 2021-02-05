#pragma once

#include <dxgi1_4.h>

#include <agz-utils/graphics_api/d3d12/descriptorHeapRaw.h>
#include <agz-utils/event.h>

AGZ_D3D12_BEGIN

class Device;
class Window;

struct SwapChainPreResizeEvent  { };
struct SwapChainPostResizeEvent { int width, height; };

using SwapChainPreResizeHandler  = event::functional_receiver_t<SwapChainPreResizeEvent>;
using SwapChainPostResizeHandler = event::functional_receiver_t<SwapChainPostResizeEvent>;

struct SwapChainDesc
{
    bool        vsync       = true;
    int         imageCount  = 2;
    DXGI_FORMAT imageFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
};

class SwapChain : public misc::uncopyable_t
{
public:

    SwapChain(
        const SwapChainDesc &desc,
        Window              &window,
        Device              &device);

    ~SwapChain();

    DXGI_FORMAT getImageFormat() const noexcept;

    int getImageCount() const noexcept;

    int getImageIndex() const noexcept;

    const ComPtr<ID3D12Resource> &getImage() const noexcept;

    const ComPtr<ID3D12Resource> &getImage(int index) const noexcept;

    void swapBuffers();

    int getWidth() const noexcept;

    int getHeight() const noexcept;

    Int2 getSize() const noexcept;

    float getWOverH() const noexcept;

    const D3D12_VIEWPORT &getDefaultViewport() const noexcept;

    const D3D12_RECT &getDefaultScissorRect() const noexcept;

    AGZ_DECL_EVENT_SENDER_HANDLER(eventSender_, SwapChainPreResizeEvent)
    AGZ_DECL_EVENT_SENDER_HANDLER(eventSender_, SwapChainPostResizeEvent)

private:

    void _resize(int width, int height);

    void setImageSize(int width, int height) noexcept;

    bool exiting_ = false;

    Device     *device_;
    bool        vsync_;
    DXGI_FORMAT imageFormat_;

    event::sender_t<
        SwapChainPreResizeEvent,
        SwapChainPostResizeEvent> eventSender_;

    ComPtr<IDXGISwapChain3> swapChain_;

    std::vector<ComPtr<ID3D12Resource>> images_;
    int imageIndex_;

    Int2 imageSize_;

    D3D12_VIEWPORT defaultViewport_;
    D3D12_RECT     defaultRect_;
};

AGZ_D3D12_END
