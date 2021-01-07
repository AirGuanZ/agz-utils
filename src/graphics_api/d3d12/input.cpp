#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/input.h>

AGZ_D3D12_BEGIN

void Input::updateSingleKey(bool pressed, KeyCode keycode)
{
    const bool lastPressed = isPressed(keycode);
    if(pressed && !lastPressed)
        _msgDown(keycode);
    else if(!pressed && lastPressed)
        _msgUp(keycode);
}

void Input::showCursor(bool show)
{
    showCursor_ = show;
    ::ShowCursor(show);
}

void Input::clearState()
{
    for(int i = 0; i < 3; ++i)
    {
        isMBPressed_         [i] = false;
        isMBPressedLastFrame_[i] = false;
        isMBDown_            [i] = false;
        isMBUp_              [i] = false;
    }

    curX_ = curY_ = 0;
    relX_ = relY_ = 0;

    isLocked_ = false;
    showCursor(true);
}

void Input::_startUpdatingKeyboard()
{
    isKeyPressedLastFrame_ = isKeyPressedThisFrame_;
}

void Input::_endUpdatingKeyboard()
{
    const bool leftShiftPressed  = (GetAsyncKeyState(VK_LSHIFT)   & 0x8000) != 0;
    const bool rightShiftPressed = (GetAsyncKeyState(VK_RSHIFT)   & 0x8000) != 0;
    const bool leftCtrlPressed   = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0;
    const bool rightCtrlPressed  = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;

    updateSingleKey(leftShiftPressed,  KEY_LSHIFT);
    updateSingleKey(rightShiftPressed, KEY_RSHIFT);
    updateSingleKey(leftCtrlPressed,   KEY_LCTRL);
    updateSingleKey(rightCtrlPressed,  KEY_RCTRL);
}

void Input::_updateMouse()
{
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(hWindow_, &cursorPos);

    relX_ = int(cursorPos.x - curX_);
    relY_ = int(cursorPos.y - curY_);
    curX_ = int(cursorPos.x);
    curY_ = int(cursorPos.y);

    if(relX_ || relY_)
    {
        eventSender_.send(CursorMoveEvent{
            curX_, curY_, relX_, relY_});
    }

    if(isLocked_)
    {
        POINT lockPos = { static_cast<LONG>(lockX_), static_cast<LONG>(lockY_) };
        ClientToScreen(hWindow_, &lockPos);
        SetCursorPos(lockPos.x, lockPos.y);

        GetCursorPos(&cursorPos);
        ScreenToClient(hWindow_, &cursorPos);
        curX_ = int(cursorPos.x);
        curY_ = int(cursorPos.y);
    }

    for(int i = 0; i < 3; ++i)
    {
        isMBDown_            [i] = !isMBPressedLastFrame_[i] && isMBPressed_[i];
        isMBUp_              [i] = isMBPressedLastFrame_[i] && !isMBPressed_[i];
        isMBPressedLastFrame_[i] = isMBPressed_[i];
    }
}

void Input::_msgMouse(UINT msg, WPARAM wParam)
{
    switch(msg)
    {
    case WM_LBUTTONDOWN:
        isMBPressed_[int(MouseButton::Left)] = true;
        eventSender_.send(MouseButtonDownEvent{ MouseButton::Left });
        break;
    case WM_MBUTTONDOWN:
        isMBPressed_[int(MouseButton::Middle)] = true;
        eventSender_.send(MouseButtonDownEvent{ MouseButton::Middle });
        break;
    case WM_RBUTTONDOWN:
        isMBPressed_[int(MouseButton::Right)] = true;
        eventSender_.send(MouseButtonDownEvent{ MouseButton::Right });
        break;
    case WM_LBUTTONUP:
        isMBPressed_[int(MouseButton::Left)] = false;
        eventSender_.send(MouseButtonUpEvent{ MouseButton::Left });
        break;
    case WM_MBUTTONUP:
        isMBPressed_[int(MouseButton::Middle)] = false;
        eventSender_.send(MouseButtonUpEvent{ MouseButton::Middle });
        break;
    case WM_RBUTTONUP:
        isMBPressed_[int(MouseButton::Right)] = false;
        eventSender_.send(MouseButtonUpEvent{ MouseButton::Right });
        break;
    case WM_MOUSEWHEEL:
        eventSender_.send(WheelScrollEvent{ GET_WHEEL_DELTA_WPARAM(wParam) });
        break;
    default:
        break;
    }
}

void Input::_msgDown(KeyCode keycode)
{
    if(!isPressed(keycode))
    {
        isKeyPressedThisFrame_[keycode] = true;
        eventSender_.send(KeyDownEvent{ keycode });
    }
}

void Input::_msgUp(KeyCode keycode)
{
    if(isPressed(keycode))
    {
        isKeyPressedThisFrame_[keycode] = false;
        eventSender_.send(KeyUpEvent{ keycode });
    }
}

void Input::_msgChar(uint32_t ch)
{
    eventSender_.send(CharInputEvent{ ch });
}

void Input::_msgRawDown(uint32_t vk)
{
    eventSender_.send(RawKeyDownEvent{ vk });
}

void Input::_msgRawUp(uint32_t vk)
{
    eventSender_.send(RawKeyUpEvent{ vk });
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
