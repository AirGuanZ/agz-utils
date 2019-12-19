#pragma once

#include <agz/utility/d3d11/Event.h>

AGZ_D3D11_BEGIN

class Window;

enum class MouseButton
{
    Left   = 0,
    Middle = 1,
    Right  = 2
};

struct MouseButtonDownEvent
{
    MouseButton button;
};

struct MouseButtonUpEvent
{
    MouseButton button;
};

struct CursorMoveEvent
{
    int x, y;
    int relX, relY;
};

struct WheelScrollEvent
{
    int offset;
};

using MouseButtonDownHandler = FunctionalEventHandler<MouseButtonDownEvent>;
using MouseButtonUpHandler   = FunctionalEventHandler<MouseButtonUpEvent>;
using CursorMoveHandler      = FunctionalEventHandler<CursorMoveEvent>;
using WheelScrollHandler     = FunctionalEventHandler<WheelScrollEvent>;

class MouseEventManager :
    public EventManager<MouseButtonDownEvent, MouseButtonUpEvent, CursorMoveEvent, WheelScrollEvent>
{
    HWND hWindow_;

    bool isButtonPressedLastFrame_[3] = { false, false, false };
    bool isButtonPressed_         [3] = { false, false, false };
    bool isButtonDown_            [3] = { false, false, false };
    bool isButtonUp_              [3] = { false, false, false };

    int cursorX_         = 0, cursorY_         = 0;
    int lastCursorX_     = 0, lastCursorY_     = 0;
    int relativeCursorX_ = 0, relativeCursorY_ = 0;

    bool isCursorLocked_ = false;
    int lockPositionX_ = 0, lockPositionY_ = 0;

    bool showCursor_ = true;

public:

    explicit MouseEventManager(HWND hWindow) noexcept
        : hWindow_(hWindow)
    {
        
    }

    ~MouseEventManager()
    {
        if(!showCursor_)
        {
            ShowCursor(true);
        }
    }

    bool IsMouseButtonPressed(MouseButton button) const noexcept { return isButtonPressed_[int(button)]; }
    bool IsMouseButtonDown   (MouseButton button) const noexcept { return isButtonDown_[int(button)]; }
    bool IsMouseButtonUp     (MouseButton button) const noexcept { return isButtonUp_[int(button)]; }

    int GetCursorPositionX() const noexcept { return cursorX_; }
    int GetCursorPositionY() const noexcept { return cursorY_; }

    int GetRelativeCursorPositionX() const noexcept { return relativeCursorX_; }
    int GetRelativeCursorPositionY() const noexcept { return relativeCursorY_; }

    void SetCursorLock(bool locked, int lockPositionX, int lockPositionY);
    void ShowCursor(bool show);

    bool IsCursorLocked()  const noexcept { return isCursorLocked_; }
    bool IsCursorVisible() const noexcept { return showCursor_; }

    int GetCursorLockX() const noexcept { return lockPositionX_; }
    int GetCursorLockY() const noexcept { return lockPositionY_; }

    void Update();

    void ClearState();

    void ProcessMessage(UINT msg, WPARAM wParam);
};

AGZ_D3D11_END
