#ifdef AGZ_ENABLE_D3D11

#include <iostream>
#include <thread>

#include <agz/utility/d3d11/ImGui/imgui.h>
#include <agz/utility/d3d11/ImGui/imgui_impl_dx11.h>
#include <agz/utility/d3d11/ImGui/imgui_impl_win32.h>
#include <agz/utility/d3d11/ImGui/input_dispatcher.h>

#include <agz/utility/d3d11/Window.h>

#include "D3DInit.h"

AGZ_D3D11_BEGIN

namespace Impl
{
    LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

    std::unordered_map<HWND, Window*> &HandleToWindow()
    {
        static std::unordered_map<HWND, Window*> ret;
        return ret;
    }

    class ImGuiInputDispatcher :
        public EventHandler<RawKeyDownEvent>,
        public EventHandler<RawKeyUpEvent>,
        public EventHandler<CharInputEvent>,
        public EventHandler<MouseButtonDownEvent>,
        public EventHandler<MouseButtonUpEvent>,
        public EventHandler<WheelScrollEvent>
    {
        static int ToImGuiMouseButton(MouseButton mb) noexcept
        {
            if(mb == MouseButton::Left)  return 0;
            if(mb == MouseButton::Right) return 1;
            return 2;
        }

    public:

        void Handle(const RawKeyDownEvent &e) override
        {
            ImGui::Input::KeyDown(e.vk);
        }

        void Handle(const RawKeyUpEvent &e) override
        {
            ImGui::Input::KeyUp(e.vk);
        }

        void Handle(const CharInputEvent &e) override
        {
            ImGui::Input::Char(e.ch);
        }

        void Handle(const MouseButtonDownEvent &e) override
        {
            ImGui::Input::MouseButtonDown(ToImGuiMouseButton(e.button));
        }

        void Handle(const MouseButtonUpEvent &e) override
        {
            ImGui::Input::MouseButtonUp(ToImGuiMouseButton(e.button));
        }

        void Handle(const WheelScrollEvent &e) override
        {
            ImGui::Input::MouseWheel(e.offset);
        }

        void AttachTo(KeyboardEventManager &keyboard, MouseEventManager &mouse)
        {
            keyboard.Attach(static_cast<EventHandler<RawKeyDownEvent> *>(this));
            keyboard.Attach(static_cast<EventHandler<RawKeyUpEvent> *>(this));
            keyboard.Attach(static_cast<EventHandler<CharInputEvent> *>(this));

            mouse.Attach(static_cast<EventHandler<MouseButtonDownEvent> *>(this));
            mouse.Attach(static_cast<EventHandler<MouseButtonUpEvent> *>(this));
            mouse.Attach(static_cast<EventHandler<WheelScrollEvent> *>(this));
        }
    };

} // namespace Impl

struct WindowImplData
{
    HWND hWindow        = nullptr;
    HINSTANCE hInstance = nullptr;

    std::wstring windowClassName;
    std::wstring windowTitle;

    bool shouldClose = false;
    bool hasFocus    = true;

    int clientLeft   = 0;
    int clientTop    = 0;
    int clientWidth  = 0;
    int clientHeight = 0;

    bool vsync = true;

    IDXGISwapChain      *swapChain     = nullptr;
    ID3D11Device        *device        = nullptr;
    ID3D11DeviceContext *deviceContext = nullptr;

    ID3D11RenderTargetView *renderTargetView   = nullptr;
    ID3D11Texture2D        *depthStencilBuffer = nullptr;
    ID3D11DepthStencilView *depthStencilView   = nullptr;

    int screenbufferSampleCount    = 1;
    int screenbufferSampleQuality  = 0;
    DXGI_FORMAT colorFormat        = DXGI_FORMAT_UNKNOWN;
    DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_UNKNOWN;

    int mouseUpdateInterval = 1;
    int mouseUpdateCounter  = 0;
    std::unique_ptr<MouseEventManager> mouse;
    std::unique_ptr<KeyboardEventManager> keyboard;

    Impl::ImGuiInputDispatcher inputDispatcher;
};

Window::~Window()
{
    Destroy();
}

void Window::Initialize(const WindowDesc &windowDesc)
{
    assert(!IsAvailable());
    assert(windowDesc.clientWidth > 0 && windowDesc.clientHeight > 0);

    // allocate WindowImplData

    data_ = new WindowImplData;
    agz::misc::scope_guard_t data_guard([&] { Destroy(); });

    // h_instance and class name

    data_->hInstance = GetModuleHandle(nullptr);
    data_->windowClassName = L"VRPGWindowClass" + std::to_wstring(size_t(this));

    // register window class

    WNDCLASSEXW wc;
    wc.cbSize        = sizeof(WNDCLASSEXA);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = Impl::WindowProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = data_->hInstance;
    wc.hIcon         = nullptr;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
    wc.lpszMenuName  = nullptr;
    wc.lpszClassName = data_->windowClassName.c_str();
    wc.hIconSm       = nullptr;
    if(!RegisterClassExW(&wc))
    {
        throw VRPGBaseException("failed to register window class");
    }

    // style, rect size and title

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int clientWidth = windowDesc.fullscreen ? screenWidth : windowDesc.clientWidth;
    int clientHeight = windowDesc.fullscreen ? screenHeight : windowDesc.clientHeight;

    DWORD dwStyle = 0;
    dwStyle |= WS_POPUPWINDOW | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
    if(windowDesc.resizable)
        dwStyle |= WS_SIZEBOX;

    RECT winRect = { 0, 0, clientWidth, clientHeight };
    if(!AdjustWindowRect(&winRect, dwStyle, FALSE))
    {
        throw VRPGBaseException("failed to adjust window size");
    }

    int realWinWidth  = winRect.right - winRect.left;
    int realWinHeight = winRect.bottom - winRect.top;
    int realWinLeft   = (screenWidth - realWinWidth) / 2;
    int realWinTop    = (screenHeight - realWinHeight) / 2;

    data_->windowTitle = windowDesc.windowTitle;

    // create window

    data_->hWindow = CreateWindowW(
        data_->windowClassName.c_str(), data_->windowTitle.c_str(),
        dwStyle, realWinLeft, realWinTop, realWinWidth, realWinHeight,
        nullptr, nullptr, data_->hInstance, nullptr);
    if(!data_->hWindow)
    {
        throw VRPGBaseException("failed to create win32 window");
    }

    ShowWindow(data_->hWindow, SW_SHOW);
    UpdateWindow(data_->hWindow);
    SetForegroundWindow(data_->hWindow);
    SetFocus(data_->hWindow);

    // client size

    RECT clientRect;
    GetClientRect(data_->hWindow, &clientRect);
    POINT LT = { clientRect.left, clientRect.top }, RB = { clientRect.right, clientRect.bottom };
    ClientToScreen(data_->hWindow, &LT);
    ClientToScreen(data_->hWindow, &RB);

    data_->clientLeft   = LT.x;
    data_->clientTop    = LT.y;
    data_->clientWidth  = RB.x - LT.x;
    data_->clientHeight = RB.y - LT.y;

    // d3d11 device && device context

    auto [device, device_context] = CreateD3D11Device();
    data_->device = device;
    data_->deviceContext = device_context;

    // dxgi swap chain

    data_->swapChain = CreateD3D11SwapChain(
        data_->hWindow, data_->clientWidth, data_->clientHeight,
        windowDesc.colorFormat, windowDesc.sampleCount, windowDesc.sampleQuality, device);
    if(windowDesc.fullscreen)
        data_->swapChain->SetFullscreenState(TRUE, nullptr);

    // render target view

    data_->renderTargetView = CreateD3D11RenderTargetView(data_->swapChain, data_->device);

    // depth stencil buffer/view

    auto [depthStencilBuffer, depthStencilView] = CreateD3D11DepthStencilBuffer(
        data_->device, data_->clientWidth, data_->clientHeight,
        windowDesc.depthStencilFormat, windowDesc.sampleCount, windowDesc.sampleQuality);

    data_->depthStencilBuffer = depthStencilBuffer;
    data_->depthStencilView   = depthStencilView;
    data_->deviceContext->OMSetRenderTargets(1, &data_->renderTargetView, data_->depthStencilView);

    // input device

    data_->mouse = std::make_unique<MouseEventManager>(data_->hWindow);
    data_->keyboard = std::make_unique<KeyboardEventManager>();

    // imgui

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(data_->hWindow);
    ImGui_ImplDX11_Init(data_->device, data_->deviceContext);
    ImGui::StyleColorsLight();
    data_->inputDispatcher.AttachTo(*data_->keyboard, *data_->mouse);

    // misc

    data_->vsync                     = windowDesc.vsync;
    data_->screenbufferSampleCount   = windowDesc.sampleCount;
    data_->screenbufferSampleQuality = windowDesc.sampleQuality;
    data_->colorFormat               = windowDesc.colorFormat;
    data_->depthStencilFormat        = windowDesc.depthStencilFormat;

    assert(!gDevice && !gDeviceContext);
    gDevice        = data_->device;
    gDeviceContext = data_->deviceContext;

    UseDefaultViewport();
    data_->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    Impl::HandleToWindow().insert(std::make_pair(data_->hWindow, this));

    data_guard.dismiss();
}

bool Window::IsAvailable() const noexcept
{
    return data_ != nullptr;
}

void Window::Destroy()
{
    if(!data_)
    {
        return;
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    if(ImGui::GetCurrentContext())
        ImGui::DestroyContext();

    gDevice        = nullptr;
    gDeviceContext = nullptr;

    ReleaseCOMObjects(data_->depthStencilBuffer, data_->depthStencilView);
    ReleaseCOMObjects(data_->renderTargetView);

    if(data_->deviceContext)
    {
        data_->deviceContext->ClearState();
    }

    if(data_->swapChain)
    {
        data_->swapChain->SetFullscreenState(FALSE, nullptr);
    }
    ReleaseCOMObjects(data_->swapChain, data_->deviceContext);

#ifdef AGZ_DEBUG
    if(data_->device)
    {
        ID3D11Debug *debug = nullptr;
        HRESULT hr = data_->device->QueryInterface<ID3D11Debug>(&debug);
        if(SUCCEEDED(hr))
        {
            debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
        }
        ReleaseCOMObjects(debug);
    }
#endif
    ReleaseCOMObjects(data_->device);

    if(data_->hWindow)
    {
        Impl::HandleToWindow().erase(data_->hWindow);
        DestroyWindow(data_->hWindow);
    }
    if(!data_->windowClassName.empty())
    {
        UnregisterClassW(data_->windowClassName.c_str(), data_->hInstance);
    }

    eventMgr_.DetachAllTypes();

    delete data_;
    data_ = nullptr;
}

void Window::ImGuiNewFrame() const
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Window::ImGuiRender() const
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

int Window::GetClientSizeX() const noexcept
{
    assert(IsAvailable());
    return data_->clientWidth;
}

int Window::GetClientSizeY() const noexcept
{
    assert(IsAvailable());
    return data_->clientHeight;
}

float Window::GetClientAspectRatio() const noexcept
{
    assert(IsAvailable());
    return float(data_->clientWidth) / data_->clientHeight;
}

void Window::SetVSync(bool vsync) noexcept
{
    assert(IsAvailable());
    data_->vsync = vsync;
}

bool Window::GetVSync() const noexcept
{
    assert(IsAvailable());
    return data_->vsync;
}

void Window::UseDefaultViewport()
{
    assert(IsAvailable());
    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width    = float(data_->clientWidth);
    vp.Height   = float(data_->clientHeight);
    vp.MaxDepth = 1;
    vp.MinDepth = 0;
    data_->deviceContext->RSSetViewports(1, &vp);
}

void Window::ClearDefaultRenderTarget(float r, float g, float b, float a)
{
    const float CLEAR_COLOR[] = { r, g, b, a };
    ClearDefaultRenderTarget(CLEAR_COLOR);
}

void Window::ClearDefaultRenderTarget(const float backgroundColor[4])
{
    assert(IsAvailable());
    data_->deviceContext->ClearRenderTargetView(data_->renderTargetView, backgroundColor);
}

void Window::ClearDefaultDepthStencil()
{
    assert(IsAvailable());
    data_->deviceContext->ClearDepthStencilView(
        data_->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}

void Window::SwapBuffers()
{
    assert(IsAvailable());
    data_->swapChain->Present(data_->vsync ? 1 : 0, 0);
}

void Window::DoEvents(bool updateMouse, bool updateKeyboard)
{
    if(updateKeyboard)
    {
        data_->keyboard->UpdateBeforeDoEvents();
    }

    MSG msg;
    while(PeekMessage(&msg, data_->hWindow, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(updateMouse && ++data_->mouseUpdateCounter >= data_->mouseUpdateInterval)
    {
        data_->mouseUpdateCounter = 0;
        data_->mouse->Update();
    }

    if(updateKeyboard)
    {
        data_->keyboard->UpdateAfterDoEvents();
    }
}

void Window::WaitForFocus()
{
    assert(IsAvailable());

    if(!IsInFocus())
    {
        auto mouse = data_->mouse.get();

        bool showCursor = mouse->IsCursorVisible();
        bool lockCursor = mouse->IsCursorLocked();
        int cursorLockX = mouse->GetCursorLockX();
        int cursorLockY = mouse->GetCursorLockY();

        mouse->ShowCursor(true);
        mouse->SetCursorLock(false, cursorLockX, cursorLockY);
        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            DoEvents();

        } while(!IsInFocus());

        mouse->ShowCursor(showCursor);
        mouse->SetCursorLock(lockCursor, cursorLockX, cursorLockY);
        mouse->Update();
    }
}

void Window::SetMouseUpdateInterval(int interval) noexcept
{
    assert(IsAvailable());
    data_->mouseUpdateInterval = (std::max)(1, interval);
}

MouseEventManager *Window::GetMouse() noexcept
{
    assert(IsAvailable());
    return data_->mouse.get();
}

KeyboardEventManager *Window::GetKeyboard() noexcept
{
    assert(IsAvailable());
    return data_->keyboard.get();
}

ID3D11Device *Window::Device() const noexcept
{
    assert(IsAvailable());
    return data_->device;
}

ID3D11DeviceContext *Window::DeviceContext() const noexcept
{
    assert(IsAvailable());
    return data_->deviceContext;
}

HWND Window::GetNativeWindowHandle() const noexcept
{
    assert(IsAvailable());
    return data_->hWindow;
}

HINSTANCE Window::GetNativeProgramHandle() const noexcept
{
    assert(IsAvailable());
    return data_->hInstance;
}

void Window::SetCloseFlag(bool should_close) noexcept
{
    assert(IsAvailable());
    data_->shouldClose = should_close;
}

bool Window::GetCloseFlag() const noexcept
{
    assert(IsAvailable());
    return data_->shouldClose;
}

bool Window::IsInFocus() const noexcept
{
    assert(IsAvailable());
    return data_->hasFocus;
}

void Window::_resize()
{
    assert(IsAvailable());

    RECT clientRect;
    GetClientRect(data_->hWindow, &clientRect);
    POINT LT = { clientRect.left, clientRect.top }, RB = { clientRect.right, clientRect.bottom };
    ClientToScreen(data_->hWindow, &LT);
    ClientToScreen(data_->hWindow, &RB);

    data_->clientLeft   = LT.x;
    data_->clientTop    = LT.y;
    data_->clientWidth  = RB.x - LT.x;
    data_->clientHeight = RB.y - LT.y;

    data_->deviceContext->ClearState();
    ReleaseCOMObjects(data_->renderTargetView, data_->depthStencilBuffer, data_->depthStencilView);

    HRESULT hr = data_->swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
    if(FAILED(hr))
    {
        std::cerr << "failed to resize dxgi swap chain buffers" << std::endl;
        std::exit(1);
    }

    data_->renderTargetView = CreateD3D11RenderTargetView(data_->swapChain, data_->device);
    if(!data_->renderTargetView)
    {
        std::cerr << "failed to create new render target view after resizing swap chain buffers" << std::endl;
        std::exit(1);
    }

    auto [depthStencilBuffer, depthStencilView] = CreateD3D11DepthStencilBuffer(
        data_->device, data_->clientWidth, data_->clientHeight,
        data_->depthStencilFormat, data_->screenbufferSampleCount, data_->screenbufferSampleQuality);
    if(!depthStencilBuffer)
    {
        std::cerr << "failed to create new depth stencil buffer/view after resizing swap chain buffers" << std::endl;
        std::exit(1);
    }
    data_->depthStencilBuffer = depthStencilBuffer;
    data_->depthStencilView = depthStencilView;
    data_->deviceContext->OMSetRenderTargets(1, &data_->renderTargetView, data_->depthStencilView);

    UseDefaultViewport();
    eventMgr_.InvokeAllHandlers(WindowResizeEvent{ data_->clientWidth, data_->clientHeight});
}

void Window::_close()
{
    assert(IsAvailable());
    data_->shouldClose = true;
    eventMgr_.InvokeAllHandlers(WindowCloseEvent{});
}

void Window::_getFocus()
{
    assert(IsAvailable());
    data_->hasFocus = true;
    eventMgr_.InvokeAllHandlers(WindowGetFocusEvent{});
}

void Window::_lostFocus()
{
    assert(IsAvailable());
    data_->hasFocus = false;
    eventMgr_.InvokeAllHandlers(WindowLostFocusEvent{});
}

void Window::_key_down(KeyCode key)
{
    assert(IsAvailable());
    if(key != KEY_UNKNOWN)
    {
        data_->keyboard->InvokeAllHandlers(KeyDownEvent{ key });
    }
}

void Window::_key_up(KeyCode key)
{
    assert(IsAvailable());
    if(key != KEY_UNKNOWN)
    {
        data_->keyboard->InvokeAllHandlers(KeyUpEvent{ key });
    }
}

void Window::_char_input(uint32_t ch)
{
    assert(IsAvailable());
    data_->keyboard->InvokeAllHandlers(CharInputEvent{ ch });
}

void Window::_raw_key_down(uint32_t vk)
{
    assert(IsAvailable());
    data_->keyboard->InvokeAllHandlers(RawKeyDownEvent{ vk });
}

void Window::_raw_key_up(uint32_t vk)
{
    assert(IsAvailable());
    data_->keyboard->InvokeAllHandlers(RawKeyUpEvent{ vk });
}

namespace Impl
{
    LRESULT CALLBACK WindowProc(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        auto winIt = HandleToWindow().find(hWindow);
        if(winIt == HandleToWindow().end())
        {
            return DefWindowProc(hWindow, msg, wParam, lParam);
        }
        auto win = winIt->second;

        win->GetMouse()->ProcessMessage(msg, wParam);

        switch(msg)
        {
        case WM_SIZE:
            if(wParam != SIZE_MINIMIZED)
            {
                win->_resize();
            }
            break;
        case WM_CLOSE:
            win->_close();
            return 0;
        case WM_SETFOCUS:
            win->_getFocus();
            break;
        case WM_KILLFOCUS:
            win->_lostFocus();
            break;
        case WM_KEYDOWN:
            win->_key_down(VK2KeyCode(int(wParam)));
            [[fallthrough]];
        case WM_SYSKEYDOWN:
            win->_raw_key_down(uint32_t(wParam));
            return 0;
        case WM_KEYUP:
            win->_key_up(VK2KeyCode(int(wParam)));
            [[fallthrough]];
        case WM_SYSKEYUP:
            win->_raw_key_up(uint32_t(wParam));
            return 0;
        case WM_CHAR:
            if(wParam > 0 && wParam < 0x10000)
            {
                win->_char_input(uint32_t(wParam));
            }
            break;
        default:
            break;
        }
        return DefWindowProc(hWindow, msg, wParam, lParam);
    }

} // namespace Impl

AGZ_D3D11_END

#endif // #ifdef AGZ_ENABLE_D3D11
