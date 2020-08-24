#ifdef AGZ_ENABLE_D3D11

#include <thread>

#include <dxgi1_4.h>
#include <dxgidebug.h>

#include <agz/utility/graphics_api/d3d11/imgui/imgui.h>
#include <agz/utility/graphics_api/d3d11/imgui/imgui_impl_dx11.h>
#include <agz/utility/graphics_api/d3d11/imgui/imgui_impl_win32.h>
#include <agz/utility/graphics_api/d3d11/imgui/inputDispatcher.h>
#include <agz/utility/graphics_api/d3d11/device.h>
#include <agz/utility/graphics_api/d3d11/deviceContext.h>
#include <agz/utility/graphics_api/d3d11/window.h>
#include <agz/utility/system.h>

AGZ_D3D11_BEGIN

namespace
{

    class ImGuiInputDispatcher :
        public event::receiver_t<RawKeyDownEvent>,
        public event::receiver_t<RawKeyUpEvent>,
        public event::receiver_t<CharInputEvent>,
        public event::receiver_t<MouseButtonDownEvent>,
        public event::receiver_t<MouseButtonUpEvent>,
        public event::receiver_t<WheelScrollEvent>
    {
        static int ToImGuiMouseButton(MouseButton mb) noexcept
        {
            if(mb == MouseButton::Left)  return 0;
            if(mb == MouseButton::Right) return 1;
            return 2;
        }

    public:

        void handle(const RawKeyDownEvent &e) override
        {
            ImGui::Input::keyDown(e.vk);
        }

        void handle(const RawKeyUpEvent &e) override
        {
            ImGui::Input::keyUp(e.vk);
        }

        void handle(const CharInputEvent &e) override
        {
            ImGui::Input::charInput(e.ch);
        }

        void handle(const MouseButtonDownEvent &e) override
        {
            ImGui::Input::mouseButtonDown(ToImGuiMouseButton(e.button));
        }

        void handle(const MouseButtonUpEvent &e) override
        {
            ImGui::Input::mouseButtonUp(ToImGuiMouseButton(e.button));
        }

        void handle(const WheelScrollEvent &e) override
        {
            ImGui::Input::mouseWheel(e.offset);
        }

        void attachTo(Keyboard &keyboard, Mouse &mouse)
        {
            keyboard.attach(static_cast<receiver_t<RawKeyDownEvent> *>(this));
            keyboard.attach(static_cast<receiver_t<RawKeyUpEvent> *>(this));
            keyboard.attach(static_cast<receiver_t<CharInputEvent> *>(this));

            mouse.attach(static_cast<receiver_t<MouseButtonDownEvent> *>(this));
            mouse.attach(static_cast<receiver_t<MouseButtonUpEvent> *>(this));
            mouse.attach(static_cast<receiver_t<WheelScrollEvent> *>(this));
        }
    };

} // namespace anonymous

struct Window::Impl
{
    HWND      hWindow   = nullptr;
    HINSTANCE hInstance = nullptr;

    std::wstring className;

    bool shouldClose = false;
    bool hasFocus    = true;

    DWORD style = 0;

    int clientWidth  = 0;
    int clientHeight = 0;

    bool vsync = true;

    ComPtr<IDXGISwapChain>      swapChain;
    ComPtr<ID3D11Device>        device;
    ComPtr<ID3D11DeviceContext> deviceContext;

    ComPtr<ID3D11RenderTargetView> rtv;
    ComPtr<ID3D11Texture2D>        dsb;
    ComPtr<ID3D11DepthStencilView> dsv;

    int sampleCount   = 1;
    int sampleQuality = 0;

    DXGI_FORMAT backbufferFormat   = DXGI_FORMAT_UNKNOWN;
    DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_UNKNOWN;

    std::unique_ptr<Mouse>    mouse;
    std::unique_ptr<Keyboard> keyboard;

    ImGuiInputDispatcher imguiInptDispatcher;
};

namespace impl
{

    Window *gWindow = nullptr;

    LRESULT CALLBACK windowMessageProc(
        HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if(!gWindow || gWindow->getWindowHandle() != hWindow)
            return DefWindowProc(hWindow, msg, wParam, lParam);

        gWindow->getMouse()->_msg(msg, wParam);

        switch(msg)
        {
        case WM_SIZE:
            if(wParam != SIZE_MINIMIZED)
                gWindow->_resize();
            break;
        case WM_CLOSE:
            gWindow->_close();
            return 0;
        case WM_SETFOCUS:
            gWindow->_getFocus();
            break;
        case WM_KILLFOCUS:
            gWindow->_lostFocus();
            break;
        case WM_KEYDOWN:
            gWindow->_keyDown(
                event::keycode::win_vk_to_keycode(static_cast<int>(wParam)));
            [[fallthrough]];
        case WM_SYSKEYDOWN:
            gWindow->_rawKeyDown(static_cast<uint32_t>(wParam));
            break;
        case WM_KEYUP:
            gWindow->_keyUp(
                event::keycode::win_vk_to_keycode(static_cast<int>(wParam)));
            [[fallthrough]];
        case WM_SYSKEYUP:
            gWindow->_rawKeyUp(static_cast<uint32_t>(wParam));
            break;
        case WM_CHAR:
            if(0 < wParam && wParam < 0x10000)
                gWindow->_charInput(static_cast<uint32_t>(wParam));
            break;
        default:
            break;
        }
        return DefWindowProc(hWindow, msg, wParam, lParam);
    }

    Int2 clientSizeToWindowSize(DWORD style, const Int2 &clientSize)
    {
        RECT winRect = { 0, 0, clientSize.x, clientSize.y };
        if(!AdjustWindowRect(&winRect, style, FALSE))
            throw D3D11Exception("failed to compute window size");
        return { winRect.right - winRect.left, winRect.bottom - winRect.top };
    }

    DWORD getWindowStyle(const WindowDesc &desc) noexcept
    {
        DWORD style = WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX;
        if(desc.resizable)
            style |= WS_SIZEBOX | WS_MAXIMIZEBOX;
        return style;
    }

    ComPtr<ID3D11RenderTargetView> CreateD3D11RenderTargetView(
        IDXGISwapChain *swapChain, ID3D11Device *device)
    {
        ComPtr<ID3D11Texture2D> backgroundBuffer;
        if(FAILED(swapChain->GetBuffer(
            0, IID_PPV_ARGS(backgroundBuffer.GetAddressOf()))))
            throw D3D11Exception("failed to get dxgi swap chain buffer");

        ComPtr<ID3D11RenderTargetView> renderTargetView;
        if(FAILED(device->CreateRenderTargetView(
            backgroundBuffer.Get(), nullptr, renderTargetView.GetAddressOf())))
            throw D3D11Exception("failed to create d3d11 render target view");
    
        return renderTargetView;
    }

    std::pair<ComPtr<ID3D11Texture2D>, ComPtr<ID3D11DepthStencilView>>
        CreateD3D11DepthStencilBuffer(
        ID3D11Device *device, int clientWidth, int clientHeight,
        DXGI_FORMAT bufferFormat, int sampleCount, int sampleQuality)
    {
        D3D11_TEXTURE2D_DESC buffer_desc;
        buffer_desc.ArraySize          = 1;
        buffer_desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
        buffer_desc.CPUAccessFlags     = 0;
        buffer_desc.Format             = bufferFormat;
        buffer_desc.Width              = clientWidth;
        buffer_desc.Height             = clientHeight;
        buffer_desc.MipLevels          = 1;
        buffer_desc.MiscFlags          = 0;
        buffer_desc.SampleDesc.Count   = sampleCount;
        buffer_desc.SampleDesc.Quality = sampleQuality;
        buffer_desc.Usage              = D3D11_USAGE_DEFAULT;

        ComPtr<ID3D11Texture2D> buffer;
        if(FAILED(device->CreateTexture2D(
            &buffer_desc, nullptr, buffer.GetAddressOf())))
            throw D3D11Exception("failed to create d3d11 depth stencil buffer");
        
        D3D11_DEPTH_STENCIL_VIEW_DESC view_desc;
        view_desc.Format             = bufferFormat;
        view_desc.ViewDimension      = sampleCount > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
        view_desc.Flags              = 0;
        view_desc.Texture2D.MipSlice = 0;

        ComPtr<ID3D11DepthStencilView> view;
        if(FAILED(device->CreateDepthStencilView(
            buffer.Get(), &view_desc, view.GetAddressOf())))
            throw D3D11Exception("failed to create d3d11 depth stencil view");

        return { buffer, view };
    }

} // namespace impl

Window::Window(const WindowDesc &desc, bool maximized)
{
    if(impl::gWindow)
        throw D3D11Exception("there can be only one d3d11 window");

    initWin32Window(desc);
    initD3D11(desc);

    if(maximized)
        setMaximized();
}

Window::~Window()
{
    if(!impl_)
        return;

    if(ImGui::GetCurrentContext())
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    device.d3dDevice = nullptr;;
    deviceContext.d3dDeviceContext= nullptr;

    impl_->rtv.Reset();
    impl_->dsb.Reset();
    impl_->dsv.Reset();

    if(impl_->deviceContext)
        impl_->deviceContext->ClearState();

    if(impl_->swapChain)
        impl_->swapChain->SetFullscreenState(FALSE, nullptr);

    impl_->swapChain.Reset();
    impl_->deviceContext.Reset();

    impl_->device.Reset();

#ifdef AGZ_DEBUG
    ComPtr<IDXGIDebug> dxgiDebug;
    if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
    {
        dxgiDebug->ReportLiveObjects(
            DXGI_DEBUG_ALL,
            DXGI_DEBUG_RLO_FLAGS(
                DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
    }
#endif

    if(impl_->hWindow)
    {
        impl::gWindow = nullptr;
        DestroyWindow(impl_->hWindow);
    }

    if(!impl_->className.empty())
        UnregisterClassW(impl_->className.c_str(), impl_->hInstance);
}

HWND Window::getWindowHandle() const noexcept
{
    return impl_->hWindow;
}

ID3D11Device *Window::getDevice() const noexcept
{
    return device.d3dDevice;
}

ID3D11DeviceContext *Window::getDeviceContext() const noexcept
{
    return deviceContext.d3dDeviceContext;;
}

void Window::newImGuiFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Window::renderImGui()
{
    useDefaultRTVAndDSV();
    useDefaultViewport();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Window::doEvents()
{
    impl_->keyboard->_startUpdating();

    MSG msg;
    while(PeekMessage(&msg, impl_->hWindow, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    impl_->keyboard->_endUpdating();
    impl_->mouse->_update();
}

bool Window::isInFocus() const noexcept
{
    return impl_->hasFocus;
}

void Window::waitForFocus()
{
    if(isInFocus())
        return;
    
    auto mouse = getMouse();

    const bool showCursor = mouse->isVisible();
    const bool lockCursor = mouse->isLocked();
    const int  lockX      = mouse->getLockX();
    const int  lockY      = mouse->getLockY();

    mouse->showCursor(true);
    mouse->setCursorLock(false, lockX, lockY);

    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        doEvents();

    } while(!isInFocus());

    mouse->showCursor(showCursor);
    mouse->setCursorLock(lockCursor, lockX, lockY);
    mouse->_update();
}

Mouse *Window::getMouse() const noexcept
{
    return impl_->mouse.get();
}

Keyboard *Window::getKeyboard() const noexcept
{
    return impl_->keyboard.get();
}

void Window::setClientSize(const Int2 &size)
{
    RECT windowRect;
    GetWindowRect(impl_->hWindow, &windowRect);

    const auto [w, h] = impl::clientSizeToWindowSize(
        impl_->style, { size.x, size.y });

    MoveWindow(impl_->hWindow, windowRect.left, windowRect.top, w, h, TRUE);
}

void Window::setClientSize(int w, int h)
{
    setClientSize({ w, h });
}

void Window::setMaximized()
{
    ShowWindow(impl_->hWindow, SW_MAXIMIZE);
}

int Window::getClientWidth() const noexcept
{
    return impl_->clientWidth;
}

int Window::getClientHeight() const noexcept
{
    return impl_->clientHeight;
}

Int2 Window::getClientSize() const noexcept
{
    return { impl_->clientWidth, impl_->clientHeight };
}

float Window::getClientWOverH() const noexcept
{
    return static_cast<float>(impl_->clientWidth) / impl_->clientHeight;
}

bool Window::getCloseFlag() const noexcept
{
    return impl_->shouldClose;
}

void Window::setCloseFlag(bool closeFlag) noexcept
{
    impl_->shouldClose = closeFlag;
}

void Window::swapBuffers()
{
    impl_->swapChain->Present(impl_->vsync ? 1 : 0, 0);
}

bool Window::getVSync() const noexcept
{
    return impl_->vsync;
}

void Window::setVSync(bool vsync) noexcept
{
    impl_->vsync = vsync;
}

D3D11_VIEWPORT Window::getDefaultViewport() const noexcept
{
    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width    = float(impl_->clientWidth);
    vp.Height   = float(impl_->clientHeight);
    vp.MaxDepth = 1;
    vp.MinDepth = 0;
    impl_->deviceContext->RSSetViewports(1, &vp);
    return vp;
}

void Window::useDefaultViewport() const
{
    const auto vp = getDefaultViewport();
    impl_->deviceContext->RSSetViewports(1, &vp);
}

void Window::useDefaultRTVAndDSV()
{
    impl_->deviceContext->OMSetRenderTargets(
        1, impl_->rtv.GetAddressOf(), impl_->dsv.Get());
}

void Window::clearDefaultRenderTarget(const Color4 &color)
{
    impl_->deviceContext->ClearRenderTargetView(
        impl_->rtv.Get(), &color.r);
}

void Window::clearDefaultDepthStencil(float depth, uint8_t stencil)
{
    if(!impl_->dsv)
        throw D3D11Exception("default depth stencil buffer is disabled");

    impl_->deviceContext->ClearDepthStencilView(
        impl_->dsv.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        depth, stencil);
}

void Window::clearDefaultDepth(float depth)
{
    if(!impl_->dsv)
        throw D3D11Exception("default depth stencil buffer is disabled");

    impl_->deviceContext->ClearDepthStencilView(
        impl_->dsv.Get(),
        D3D11_CLEAR_DEPTH,
        depth, 0);
}

void Window::clearDefaultStencil(uint8_t stencil)
{
    if(!impl_->dsv)
        throw D3D11Exception("default depth stencil buffer is disabled");

    impl_->deviceContext->ClearDepthStencilView(
        impl_->dsv.Get(),
        D3D11_CLEAR_STENCIL,
        1, stencil);
}

void Window::_resize()
{
    eventMgr_.send(WindowPreResizeEvent{});

    RECT clientRect;
    GetClientRect(impl_->hWindow, &clientRect);
    POINT LT = { clientRect.left, clientRect.top };
    POINT RB = { clientRect.right, clientRect.bottom };
    ClientToScreen(impl_->hWindow, &LT);
    ClientToScreen(impl_->hWindow, &RB);

    impl_->clientWidth = RB.x - LT.x;
    impl_->clientHeight = RB.y - LT.y;

    impl_->deviceContext->ClearState();
    impl_->rtv.Reset();
    impl_->dsv.Reset();
    impl_->dsb.Reset();

    HRESULT hr = impl_->swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
    if(FAILED(hr))
        throw D3D11Exception("failed to resize dxgi swap chain buffers");

    impl_->rtv = impl::CreateD3D11RenderTargetView(
        impl_->swapChain.Get(), impl_->device.Get());

    if(impl_->depthStencilFormat != DXGI_FORMAT_UNKNOWN)
    {
        auto [depthStencilBuffer, depthStencilView] =
            impl::CreateD3D11DepthStencilBuffer(
                impl_->device.Get(),
                impl_->clientWidth,
                impl_->clientHeight,
                impl_->depthStencilFormat,
                impl_->sampleCount,
                impl_->sampleQuality);

        impl_->dsb = depthStencilBuffer;
        impl_->dsv = depthStencilView;
    }
    
    useDefaultRTVAndDSV();
    useDefaultViewport();

    eventMgr_.send(
        WindowPostResizeEvent{ impl_->clientWidth, impl_->clientHeight });
}

void Window::_close()
{
    impl_->shouldClose = true;
    eventMgr_.send(WindowCloseEvent{});
}

void Window::_getFocus()
{
    impl_->hasFocus = true;
    eventMgr_.send(WindowGetFocusEvent{});
}

void Window::_lostFocus()
{
    impl_->hasFocus = false;
    eventMgr_.send(WindowLostFocusEvent{});
}

void Window::_keyDown(KeyCode kc)
{
    if(kc != KEY_UNKNOWN)
        impl_->keyboard->_msgDown(kc);
}

void Window::_keyUp(KeyCode kc)
{
    if(kc != KEY_UNKNOWN)
        impl_->keyboard->_msgUp(kc);
}

void Window::_rawKeyDown(uint32_t vk)
{
    impl_->keyboard->_msgRawDown(vk);
}

void Window::_rawKeyUp(uint32_t vk)
{
    impl_->keyboard->_msgRawUp(vk);
}

void Window::_charInput(uint32_t ch)
{
    impl_->keyboard->_msgChar(ch);
}

void Window::initWin32Window(const WindowDesc &desc)
{
    impl_ = std::make_unique<Impl>();

    impl_->hInstance = GetModuleHandle(nullptr);
    
    // register window class

    const std::wstring className = L"AGZD3D11ClassName";

    WNDCLASSEXW wc;
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = impl::windowMessageProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = impl_->hInstance;
    wc.hIcon         = nullptr;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
    wc.lpszMenuName  = nullptr;
    wc.lpszClassName = className.c_str();
    wc.hIconSm       = nullptr;

    if(!RegisterClassExW(&wc))
        throw D3D11Exception("failed to register window class");

    impl_->className = className;

    // style & pos & size

    const int scrW = GetSystemMetrics(SM_CXSCREEN);
    const int scrH = GetSystemMetrics(SM_CYSCREEN);

    RECT workAreaRect;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0);

    const int workAreaW = workAreaRect.right - workAreaRect.left;
    const int workAreaH = workAreaRect.bottom - workAreaRect.top;

    const int realClientW = desc.fullscreen ? scrW : desc.clientSize.x;
    const int realClientH = desc.fullscreen ? scrH : desc.clientSize.y;

    impl_->style = impl::getWindowStyle(desc);

    const auto winSize = impl::clientSizeToWindowSize(
        impl_->style, { realClientW, realClientH });

    int left, top;
    if(desc.fullscreen)
    {
        left = (scrW - winSize.x) / 2;
        top  = (scrH - winSize.y) / 2;
    }
    else
    {
        left = workAreaRect.left + (workAreaW - winSize.x) / 2;
        top = workAreaRect.top + (workAreaH - winSize.y) / 2;
    }

    // create window

    impl_->hWindow = CreateWindowW(
        impl_->className.c_str(), desc.title.c_str(),
        impl_->style, left, top, winSize.x, winSize.y,
        nullptr, nullptr, impl_->hInstance, nullptr);
    if(!impl_->hWindow)
        throw D3D11Exception("failed to create win32 window");

    // show & focus

    ShowWindow(impl_->hWindow, SW_SHOW);
    UpdateWindow(impl_->hWindow);

    SetForegroundWindow(impl_->hWindow);
    SetFocus(impl_->hWindow);

    // client size

    RECT clientRect;
    GetClientRect(impl_->hWindow, &clientRect);
    impl_->clientWidth  = clientRect.right - clientRect.left;
    impl_->clientHeight = clientRect.bottom - clientRect.top;

    // mouse & keyboard

    impl_->mouse    = std::make_unique<Mouse>(impl_->hWindow);
    impl_->keyboard = std::make_unique<Keyboard>();
}

void Window::initD3D11(const WindowDesc &desc)
{
    // device & device context

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    UINT createDeviceFlag = 0;
#ifdef AGZ_DEBUG
    createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    if(FAILED(D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        createDeviceFlag,
        &featureLevel, 1,
        D3D11_SDK_VERSION,
        impl_->device.GetAddressOf(),
        nullptr,
        impl_->deviceContext.GetAddressOf())))
        throw D3D11Exception("failed to create d3d11 device & device context");

    // swap chain

    DXGI_MODE_DESC backbufDesc;
    backbufDesc.Width                   = desc.clientSize.x;
    backbufDesc.Height                  = desc.clientSize.y;
    backbufDesc.RefreshRate.Numerator   = 60;
    backbufDesc.RefreshRate.Denominator = 1;
    backbufDesc.Format                  = desc.backbufferFormat;
    backbufDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    backbufDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;

    if(desc.sampleCount > D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT)
        throw D3D11Exception("invalid multisample count value");

    UINT endSampleQuality;
    if(FAILED(impl_->device->CheckMultisampleQualityLevels(
        desc.backbufferFormat, desc.sampleCount, &endSampleQuality)))
        throw D3D11Exception("failed to check multisample quality level");

    if(desc.sampleQuality >= static_cast<int>(endSampleQuality))
        throw D3D11Exception("unsupported sampleCount/sampleQuality values");

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    swapChainDesc.BufferDesc         = backbufDesc;
    swapChainDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount        = 1;
    swapChainDesc.OutputWindow       = impl_->hWindow;
    swapChainDesc.Windowed           = true;
    swapChainDesc.SwapEffect         = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags              = 0;
    swapChainDesc.SampleDesc.Count   = desc.sampleCount;
    swapChainDesc.SampleDesc.Quality = desc.sampleQuality;

    ComPtr<IDXGIDevice> dxgiDevice;
    if(FAILED(impl_->device->QueryInterface(
        IID_PPV_ARGS(dxgiDevice.GetAddressOf()))))
        throw D3D11Exception("failed to get dxgi device");

    ComPtr<IDXGIAdapter> dxgiAdapter;
    if(FAILED(dxgiDevice->GetParent(
        IID_PPV_ARGS(dxgiAdapter.GetAddressOf()))))
        throw D3D11Exception("failed to get dxgi adapter");

    ComPtr<IDXGIFactory> dxgiFactory;
    if(FAILED(dxgiAdapter->GetParent(
        IID_PPV_ARGS(dxgiFactory.GetAddressOf()))))
        throw D3D11Exception("failed to get dxgi factory");

    if(FAILED(dxgiFactory->CreateSwapChain(
        impl_->device.Get(), &swapChainDesc, impl_->swapChain.GetAddressOf())))
        throw D3D11Exception("failed to create swap chain");

    if(desc.fullscreen)
        impl_->swapChain->SetFullscreenState(TRUE, nullptr);

    // rtv

    impl_->rtv = impl::CreateD3D11RenderTargetView(
        impl_->swapChain.Get(), impl_->device.Get());

    // dsb & dsv

    if(desc.depthStencilFormat != DXGI_FORMAT_UNKNOWN)
    {
        auto [dsb, dsv] = impl::CreateD3D11DepthStencilBuffer(
            impl_->device.Get(), impl_->clientWidth, impl_->clientHeight,
            desc.depthStencilFormat, desc.sampleCount, desc.sampleQuality);

        impl_->dsb = dsb;
        impl_->dsv = dsv;
    }

    useDefaultRTVAndDSV();
    useDefaultViewport();

    // imgui

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(impl_->hWindow);
    ImGui_ImplDX11_Init(impl_->device.Get(), impl_->deviceContext.Get());
    ImGui::StyleColorsLight();
    impl_->imguiInptDispatcher.attachTo(*impl_->keyboard, *impl_->mouse);

    // misc

    impl_->vsync              = desc.vsync;
    impl_->sampleCount        = desc.sampleCount;
    impl_->sampleQuality      = desc.sampleQuality;
    impl_->backbufferFormat   = desc.backbufferFormat;
    impl_->depthStencilFormat = desc.depthStencilFormat;

    device.d3dDevice = impl_->device.Get();
    deviceContext.d3dDeviceContext = impl_->deviceContext.Get();

    impl::gWindow = this;
}

AGZ_D3D11_END

#endif // #ifdef AGZ_ENABLE_D3D11
