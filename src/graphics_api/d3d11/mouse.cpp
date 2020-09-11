#include <Windows.h>

#include <agz/utility/graphics_api/d3d11/mouse.h>

AGZ_D3D11_BEGIN

Mouse::Mouse(HWND hWindow)
    : hWindow_(hWindow)
{
    
}

Mouse::~Mouse()
{
    if(!showCursor_)
        showCursor(true);
}

void Mouse::setCursorLock(bool locked, int lockPositionX, int lockPositionY)
{
    isCursorLocked_ = locked;
    lockPositionX_ = lockPositionX;
    lockPositionY_ = lockPositionY;
}

void Mouse::showCursor(bool show)
{
    showCursor_ = show;
    ShowCursor(show ? TRUE : FALSE);
}

void Mouse::clearState()
{
    isButtonPressedLastFrame_[0] = isButtonPressedLastFrame_[1] = isButtonPressedLastFrame_[2] = false;
    isButtonPressed_[0]          = isButtonPressed_[1]          = isButtonPressed_[2]          = false;
    isButtonDown_[0]             = isButtonDown_[1]             = isButtonDown_[2]             = false;
    isButtonUp_[0]               = isButtonUp_[1]               = isButtonUp_[2]               = false;
    
    cursorX_         = cursorY_         = 0;
    lastCursorX_     = lastCursorY_     = 0;
    relativeCursorX_ = relativeCursorY_ = 0;

    isCursorLocked_ = false;
    if(!showCursor_)
    {
        ShowCursor(true);
    }
}

void Mouse::_update()
{
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(hWindow_, &cursorPos);

    relativeCursorX_ = int(cursorPos.x - cursorX_);
    relativeCursorY_ = int(cursorPos.y - cursorY_);
    cursorX_ = int(cursorPos.x);
    cursorY_ = int(cursorPos.y);

    if(relativeCursorX_ || relativeCursorY_)
    {
        eventMgr_.send(CursorMoveEvent{
            cursorX_, cursorY_, relativeCursorX_, relativeCursorY_ });
    }

    if(isCursorLocked_)
    {
        POINT lockPos = { static_cast<LONG>(lockPositionX_), static_cast<LONG>(lockPositionY_) };
        ClientToScreen(hWindow_, &lockPos);
        SetCursorPos(lockPos.x, lockPos.y);

        GetCursorPos(&cursorPos);
        ScreenToClient(hWindow_, &cursorPos);
        cursorX_ = int(cursorPos.x);
        cursorY_ = int(cursorPos.y);
    }

    for(int i = 0; i < 3; ++i)
    {
        isButtonDown_[i] = !isButtonPressedLastFrame_[i] && isButtonPressed_[i];
        isButtonUp_[i]   = isButtonPressedLastFrame_[i] && !isButtonPressed_[i];
        isButtonPressedLastFrame_[i] = isButtonPressed_[i];
    }
}

void Mouse::_msg(UINT msg, WPARAM wParam)
{
    switch(msg)
    {
    case WM_LBUTTONDOWN:
        isButtonPressed_[int(MouseButton::Left)] = true;
        eventMgr_.send(MouseButtonDownEvent{ MouseButton::Left });
        break;
    case WM_MBUTTONDOWN:
        isButtonPressed_[int(MouseButton::Middle)] = true;
        eventMgr_.send(MouseButtonDownEvent{ MouseButton::Middle });
        break;
    case WM_RBUTTONDOWN:
        isButtonPressed_[int(MouseButton::Right)] = true;
        eventMgr_.send(MouseButtonDownEvent{ MouseButton::Right });
        break;
    case WM_LBUTTONUP:
        isButtonPressed_[int(MouseButton::Left)] = false;
        eventMgr_.send(MouseButtonUpEvent{ MouseButton::Left });
        break;
    case WM_MBUTTONUP:
        isButtonPressed_[int(MouseButton::Middle)] = false;
        eventMgr_.send(MouseButtonUpEvent{ MouseButton::Middle });
        break;
    case WM_RBUTTONUP:
        isButtonPressed_[int(MouseButton::Right)] = false;
        eventMgr_.send(MouseButtonUpEvent{ MouseButton::Right });
        break;
    case WM_MOUSEWHEEL:
        eventMgr_.send(WheelScrollEvent{ GET_WHEEL_DELTA_WPARAM(wParam) });
        break;
    default:
        break;
    }
}

AGZ_D3D11_END
