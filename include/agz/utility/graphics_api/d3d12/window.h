#pragma once

#include <agz/utility/graphics_api/d3d12/input.h>
#include <agz/utility/event.h>

AGZ_D3D12_BEGIN

struct WindowCloseEvent     { };
struct WindowGetFocusEvent  { };
struct WindowLostFocusEvent { };
struct WindowResizeEvent    { int width, height; };

using WindowCloseHandler     = event::functional_receiver_t<WindowCloseEvent>;
using WindowGetFocusHandler  = event::functional_receiver_t<WindowGetFocusEvent>;
using WindowLostFocusHandler = event::functional_receiver_t<WindowLostFocusEvent>;
using WindowResizeHandler    = event::functional_receiver_t<WindowResizeEvent>;

using WindowEventSender = event::sender_t<
    WindowCloseEvent,
    WindowGetFocusEvent,
    WindowLostFocusEvent,
    WindowResizeEvent>;

struct WindowDesc
{
    Int2         clientSize = { 640, 480 };
    std::wstring title      = L"Window";
    bool         resizable  = true;
    bool         fullscreen = false;
};

class Window
{
public:

    explicit Window(const WindowDesc &desc);

    ~Window();

    // win32

    HWND getWindowHandle() noexcept;

    bool isFullscreen() const noexcept;

    void doEvents();

    bool isInFocus() const noexcept;

    void waitForFocus();

    void setMaximized();

    Input *getInput() noexcept;

    // size

    int getClientWidth() const noexcept;

    int getClientHeight() const noexcept;

    Int2 getClientSize() const noexcept;

    // close flag

    bool getCloseFlag() const noexcept;

    void setCloseFlag(bool closeFlag) noexcept;

    // event handler

    AGZ_DECL_EVENT_SENDER_HANDLER(eventSender_, WindowCloseEvent)
    AGZ_DECL_EVENT_SENDER_HANDLER(eventSender_, WindowGetFocusEvent)
    AGZ_DECL_EVENT_SENDER_HANDLER(eventSender_, WindowLostFocusEvent)
    AGZ_DECL_EVENT_SENDER_HANDLER(eventSender_, WindowResizeEvent)
    
    // internal use

    void _close();

    void _getFocus();

    void _lostFocus();

    void _resize();

    void _mouseMsg(UINT msg, WPARAM wParam);

    void _keyDown(KeyCode kc);

    void _keyUp(KeyCode kc);

    void _rawKeyDown(uint32_t vk);

    void _rawKeyUp(uint32_t vk);

    void _charInput(uint32_t ch);

private:

    void initWin32Window(const WindowDesc &desc);

    void updateClientSize();

    struct Data;

    std::unique_ptr<Data> data_;

    WindowEventSender eventSender_;
};

AGZ_D3D12_END
