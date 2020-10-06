#pragma once

#include <agz/utility/graphics_api/d3d12/common.h>
#include <agz/utility/event.h>

AGZ_D3D12_BEGIN

struct WindowCloseEvent      { };
struct WindowGetFocusEvent   { };
struct WindowLostFocusEvent  { };
struct WindowPreResizeEvent  { };
struct WindowPostResizeEvent { int width, height; };

using WindowCloseHandler      = event::functional_receiver_t<WindowCloseEvent>;
using WindowGetFocusHandler   = event::functional_receiver_t<WindowGetFocusEvent>;
using WindowLostFocusHandler  = event::functional_receiver_t<WindowLostFocusEvent>;
using WindowPreResizeHandler  = event::functional_receiver_t<WindowPreResizeEvent>;
using WindowPostResizeHandler = event::functional_receiver_t<WindowPostResizeEvent>;

using WindowEventSender = event::sender_t<
    WindowCloseEvent,
    WindowGetFocusEvent,
    WindowLostFocusEvent,
    WindowPreResizeEvent,
    WindowPostResizeEvent>;

struct WindowDesc
{
    Int2         clientSize = { 640, 480 };
    std::wstring title      = L"Window";
    bool         resizable  = true;
};

class Window
{
public:

    explicit Window(const WindowDesc &desc);

    ~Window();

    // win32

    HWND getWindowHandle() noexcept;

    void doEvents();

    bool isInFocus() const noexcept;

    void waitForFocus();

    void setMaximized();

    // close flag

    bool getCloseFlag() const noexcept;

    void setCloseFlag(bool closeFlag) noexcept;

    // event handler

    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(eventSender_, WindowCloseEvent)
    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(eventSender_, WindowGetFocusEvent)
    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(eventSender_, WindowLostFocusEvent)
    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(eventSender_, WindowPreResizeEvent)
    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(eventSender_, WindowPostResizeEvent)

    // internal use

    void _close();

    void _getFocus();

    void _lostFocus();

    void _resize();

private:

    void initWin32Window(const WindowDesc &desc);

    void updateClientSize();

    struct Data;

    std::unique_ptr<Data> data_;

    WindowEventSender eventSender_;
};

AGZ_D3D12_END
