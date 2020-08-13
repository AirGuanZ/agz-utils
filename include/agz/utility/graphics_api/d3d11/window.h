#pragma once

#include <string>

#include "../../event.h"
#include "keyboard.h"
#include "mouse.h"

AGZ_D3D11_BEGIN

struct WindowCloseEvent      { };
struct WindowGetFocusEvent   { };
struct WindowLostFocusEvent  { };
struct WindowPreResizeEvent  { };
struct WindowPostResizeEvent { int width; int height; };

using WindowCloseHandler      = event::functional_receiver_t<WindowCloseEvent>;
using WindowGetFocusHandler   = event::functional_receiver_t<WindowGetFocusEvent>;
using WindowLostFocusHandler  = event::functional_receiver_t<WindowLostFocusEvent>;
using WindowPreResizeHandler  = event::functional_receiver_t<WindowPreResizeEvent>;
using WindowPostResizeHandler = event::functional_receiver_t<WindowPostResizeEvent>;

using WindowEventManager = event::sender_t<
    WindowCloseEvent,
    WindowGetFocusEvent,
    WindowLostFocusEvent,
    WindowPreResizeEvent,
    WindowPostResizeEvent>;

struct WindowDesc
{
    Int2 clientSize    = { 640, 480 };
    std::wstring title = L"Window";

    bool resizable  = true;
    bool fullscreen = false;
    bool vsync      = true;

    int sampleCount   = 1;
    int sampleQuality = 0;

    DXGI_FORMAT backbufferFormat   = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
};

class Window : public misc::uncopyable_t
{
public:

    explicit Window(const WindowDesc &desc, bool maximized = false);

    ~Window();

    // native handles

    HWND getWindowHandle() const noexcept;

    ID3D11Device *getDevice() const noexcept;

    ID3D11DeviceContext *getDeviceContext() const noexcept;

    // imgui

    void newImGuiFrame();

    void renderImGui();

    // window events

    void doEvents();

    bool isInFocus() const noexcept;

    void waitForFocus();

    // mouse & keyboard

    Mouse *getMouse() const noexcept;

    Keyboard *getKeyboard() const noexcept;

    // size

    void setClientSize(const Int2 &size);

    void setClientSize(int w, int h);

    void setMaximized();

    int getClientWidth() const noexcept;

    int getClientHeight() const noexcept;

    float getClientWOverH() const noexcept;

    Int2 getClientSize() const noexcept;

    // close flag

    bool getCloseFlag() const noexcept;

    void setCloseFlag(bool closeFlag) noexcept;

    // swap chain

    void swapBuffers();

    // vsync

    bool getVSync() const noexcept;

    void setVSync(bool vsync) noexcept;

    // viewport

    D3D11_VIEWPORT getDefaultViewport() const noexcept;

    void useDefaultViewport() const;

    // render target

    void useDefaultRTVAndDSV();

    void clearDefaultRenderTarget(const Color4 &color);

    void clearDefaultDepthStencil(float depth, uint8_t stencil);

    void clearDefaultDepth(float depth);

    void clearDefaultStencil(uint8_t stencil);

    // events
    
    AGZ_D3D11_DECL_EVENT_MGR_HANDLER(eventMgr_, WindowCloseEvent)
    AGZ_D3D11_DECL_EVENT_MGR_HANDLER(eventMgr_, WindowGetFocusEvent)
    AGZ_D3D11_DECL_EVENT_MGR_HANDLER(eventMgr_, WindowLostFocusEvent)
    AGZ_D3D11_DECL_EVENT_MGR_HANDLER(eventMgr_, WindowPreResizeEvent)
    AGZ_D3D11_DECL_EVENT_MGR_HANDLER(eventMgr_, WindowPostResizeEvent)

    // internal

    void _resize();

    void _close();

    void _getFocus();

    void _lostFocus();

    void _keyDown(KeyCode kc);

    void _keyUp(KeyCode kc);

    void _rawKeyDown(uint32_t vk);

    void _rawKeyUp(uint32_t vk);

    void _charInput(uint32_t ch);

private:

    void initWin32Window(const WindowDesc &desc);

    void initD3D11(const WindowDesc &desc);

    struct Impl;

    std::unique_ptr<Impl> impl_;

    WindowEventManager eventMgr_;
};

AGZ_D3D11_END
