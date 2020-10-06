#ifdef AGZ_ENABLE_D3D12

#include <chrono>
#include <map>
#include <thread>

#include <agz/utility/graphics_api/d3d12/window.h>

AGZ_D3D12_BEGIN

namespace detail
{

    LRESULT CALLBACK windowMessageProc(HWND, UINT, WPARAM, LPARAM);

    std::map<HWND, Window *> &handleToWindow()
    {
        static std::map<HWND, Window *> ret;
        return ret;
    }

    DWORD getWindowStyle(const WindowDesc &desc)
    {
        DWORD style = WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX;
        if(desc.resizable)
            style |= WS_SIZEBOX | WS_MAXIMIZEBOX;
        return style;
    }

    Int2 clientSizeToWindowSize(DWORD style, const Int2 &clientSize)
    {
        RECT winRect = { 0, 0, clientSize.x, clientSize.y };
        if(!AdjustWindowRect(&winRect, style, FALSE))
            throw D3D12Exception("failed to compute window size");
        return { winRect.right - winRect.left, winRect.bottom - winRect.top };
    }

} // namespace detail

struct Window::Data
{
    // win32 window

    DWORD style = 0;

    std::wstring className;

    HINSTANCE hInst = nullptr;
    HWND hWnd       = nullptr;

    Int2 clientSize;

    bool shouldClos = false;
    bool isInFocus  = true;
};

Window::Window(const WindowDesc &desc)
{
    initWin32Window(desc);
}

Window::~Window()
{
    if(!data_)
        return;

    if(data_->hWnd)
    {
        detail::handleToWindow().erase(data_->hWnd);
        DestroyWindow(data_->hWnd);
    }

    UnregisterClassW(data_->className.c_str(), data_->hInst);
}

HWND Window::getWindowHandle() noexcept
{
    return data_->hWnd;
}

void Window::doEvents()
{
    MSG msg;
    while(PeekMessage(&msg, data_->hWnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

bool Window::isInFocus() const noexcept
{
    return data_->isInFocus;
}

void Window::waitForFocus()
{
    if(isInFocus())
        return;
    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        doEvents();

    } while(!isInFocus());
}

void Window::setMaximized()
{
    ShowWindow(data_->hWnd, SW_MAXIMIZE);
}

bool Window::getCloseFlag() const noexcept
{
    return data_->shouldClos;
}

void Window::setCloseFlag(bool closeFlag) noexcept
{
    data_->shouldClos = closeFlag;
}

void Window::initWin32Window(const WindowDesc &desc)
{
    data_ = std::make_unique<Data>();

    data_->hInst = GetModuleHandle(nullptr);
    data_->className = L"D3D12WindowClass"
                     + std::to_wstring(reinterpret_cast<size_t>(this));

    // window class

    WNDCLASSEXW wc;
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = detail::windowMessageProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = data_->hInst;
    wc.hIcon         = nullptr;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
    wc.lpszMenuName  = nullptr;
    wc.lpszClassName = data_->className.c_str();
    wc.hIconSm       = nullptr;

    if(!RegisterClassExW(&wc))
        throw D3D12Exception("failed to register window class");

    // window style & rect

    data_->style = detail::getWindowStyle(desc);

    RECT workAreaRect;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &workAreaRect, 0);
    const int workAreaW = workAreaRect.right  - workAreaRect.left;
    const int workAreaH = workAreaRect.bottom - workAreaRect.top;

    const int clientW = desc.clientSize.x;
    const int clientH = desc.clientSize.y;

    const Int2 winSize = detail::clientSizeToWindowSize(
        data_->style, { clientW, clientH });

    Int2 winPos;
    winPos.x = workAreaRect.left  + (workAreaW - winSize.x) / 2;
    winPos.y = workAreaRect.right + (workAreaH - winSize.y) / 2;

    // window

    data_->hWnd = CreateWindowW(
        data_->className.c_str(),
        desc.title.c_str(),
        data_->style,
        winPos.x, winPos.y, winSize.x, winSize.y,
        nullptr, nullptr, data_->hInst, nullptr);

    if(!data_->hWnd)
        throw D3D12Exception("failed to create win32 window");

    // show & focus

    ShowWindow(data_->hWnd, SW_SHOW);
    UpdateWindow(data_->hWnd);
    SetForegroundWindow(data_->hWnd);
    SetFocus(data_->hWnd);

    updateClientSize();

    detail::handleToWindow().insert({ data_->hWnd, this });
}

void Window::updateClientSize()
{
    RECT clientRect;
    GetClientRect(data_->hWnd, &clientRect);
    data_->clientSize.x = clientRect.right  - clientRect.left;
    data_->clientSize.y = clientRect.bottom - clientRect.top;
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
