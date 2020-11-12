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

    bool fullscreen = false;

    DWORD style = 0;

    std::wstring className;

    HINSTANCE hInst = nullptr;
    HWND hWnd       = nullptr;

    Int2 clientSize;

    bool shouldClos = false;
    bool isInFocus  = true;

    Input input;
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

bool Window::isFullscreen() const noexcept
{
    return data_->fullscreen;
}

void Window::doEvents()
{
    data_->input._startUpdatingKeyboard();

    MSG msg;
    while(PeekMessage(&msg, data_->hWnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    data_->input._endUpdatingKeyboard();
    data_->input._updateMouse();
}

bool Window::isInFocus() const noexcept
{
    return data_->isInFocus;
}

void Window::waitForFocus()
{
    if(isInFocus())
        return;

    auto input = getInput();
    const bool showCursor = input->isCursorVisible();
    const bool lockCursor = input->isCursorLocked();
    const int  lockX      = input->getCursorLockX();
    const int  lockY      = input->getCursorLockY();

    input->showCursor(true);
    input->setCursorLock(false, lockX, lockY);

    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        doEvents();

    } while(!isInFocus());

    input->showCursor(showCursor);
    input->setCursorLock(lockCursor, lockX, lockY);

    input->_updateMouse();
}

void Window::setMaximized()
{
    ShowWindow(data_->hWnd, SW_MAXIMIZE);
}

Input *Window::getInput() noexcept
{
    return &data_->input;
}

int Window::getClientWidth() const noexcept
{
    return data_->clientSize.x;
}

int Window::getClientHeight() const noexcept
{
    return data_->clientSize.y;
}

Int2 Window::getClientSize() const noexcept
{
    return data_->clientSize;
}

bool Window::getCloseFlag() const noexcept
{
    return data_->shouldClos;
}

void Window::setCloseFlag(bool closeFlag) noexcept
{
    data_->shouldClos = closeFlag;
}

void Window::_close()
{
    data_->shouldClos = true;
    eventSender_.send(WindowCloseEvent{});
}

void Window::_getFocus()
{
    data_->isInFocus = true;
    eventSender_.send(WindowGetFocusEvent{});
}

void Window::_lostFocus()
{
    data_->isInFocus = false;
    eventSender_.send(WindowLostFocusEvent{});
}

void Window::_resize()
{
    updateClientSize();
    eventSender_.send(
        WindowResizeEvent{ data_->clientSize.x, data_->clientSize.y });
}

void Window::_mouseMsg(UINT msg, WPARAM wParam)
{
    data_->input._msgMouse(msg, wParam);
}

void Window::_keyDown(KeyCode kc)
{
    if(kc != KEY_UNKNOWN)
        data_->input._msgDown(kc);
}

void Window::_keyUp(KeyCode kc)
{
    if(kc != KEY_UNKNOWN)
        data_->input._msgUp(kc);
}

void Window::_rawKeyDown(uint32_t vk)
{
    data_->input._msgRawDown(vk);
}

void Window::_rawKeyUp(uint32_t vk)
{
    data_->input._msgRawUp(vk);
}

void Window::_charInput(uint32_t ch)
{
    data_->input._msgChar(ch);
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

    data_->fullscreen = desc.fullscreen;
    data_->style      = detail::getWindowStyle(desc);

    RECT workAreaRect;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &workAreaRect, 0);
    const int workAreaW = workAreaRect.right  - workAreaRect.left;
    const int workAreaH = workAreaRect.bottom - workAreaRect.top;

    const int scrW = GetSystemMetrics(SM_CXSCREEN);
    const int scrH = GetSystemMetrics(SM_CYSCREEN);

    const int clientW = desc.fullscreen ? scrW : desc.clientSize.x;
    const int clientH = desc.fullscreen ? scrH : desc.clientSize.y;

    const Int2 winSize = detail::clientSizeToWindowSize(
        data_->style, { clientW, clientH });

    const Int2 winPos = {
        workAreaRect.left + (workAreaW - winSize.x) / 2,
        workAreaRect.top  + (workAreaH - winSize.y) / 2
    };

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

    detail::handleToWindow().insert({ data_->hWnd, this });
    data_->input.setWindowHandle(data_->hWnd);

    ShowWindow(data_->hWnd, SW_SHOW);
    UpdateWindow(data_->hWnd);

    SetForegroundWindow(data_->hWnd);
    SetFocus(data_->hWnd);

    updateClientSize();
}

void Window::updateClientSize()
{
    RECT clientRect;
    GetClientRect(data_->hWnd, &clientRect);
    data_->clientSize.x = clientRect.right  - clientRect.left;
    data_->clientSize.y = clientRect.bottom - clientRect.top;
}

LRESULT detail::windowMessageProc(
    HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    const auto winIt = handleToWindow().find(hWnd);
    if(winIt == handleToWindow().end())
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    const auto win = winIt->second;

    win->_mouseMsg(msg, wParam);

    switch(msg)
    {
    case WM_CLOSE:
        win->_close();
        return 0;
    case WM_SETFOCUS:
        win->_getFocus();
        break;
    case WM_KILLFOCUS:
        win->_lostFocus();
        break;
    case WM_SIZE:
        if(wParam != SIZE_MINIMIZED)
            win->_resize();
        break;
    case WM_KEYDOWN:
        win->_keyDown(
            event::keycode::win_vk_to_keycode(static_cast<int>(wParam)));
        [[fallthrough]];
    case WM_SYSKEYDOWN:
        win->_rawKeyDown(static_cast<uint32_t>(wParam));
        break;
    case WM_KEYUP:
        win->_keyUp(
            event::keycode::win_vk_to_keycode(static_cast<int>(wParam)));
        [[fallthrough]];
    case WM_SYSKEYUP:
        win->_rawKeyUp(static_cast<uint32_t>(wParam));
        break;
    case WM_CHAR:
        if(0 < wParam && wParam < 0x10000)
            win->_charInput(static_cast<uint32_t>(wParam));
        break;
    default:
        break;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
