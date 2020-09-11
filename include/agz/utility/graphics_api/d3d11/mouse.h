#pragma once

#include "../../event.h"
#include "common.h"

AGZ_D3D11_BEGIN

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

using MouseButtonDownHandler = event::functional_receiver_t<MouseButtonDownEvent>;
using MouseButtonUpHandler   = event::functional_receiver_t<MouseButtonUpEvent>;
using CursorMoveHandler      = event::functional_receiver_t<CursorMoveEvent>;
using WheelScrollHandler     = event::functional_receiver_t<WheelScrollEvent>;

class Mouse
{
    using EventMgr = event::sender_t<
        MouseButtonDownEvent,
        MouseButtonUpEvent,
        CursorMoveEvent,
        WheelScrollEvent>;

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

    EventMgr eventMgr_;

public:

    explicit Mouse(HWND hWindow);

    ~Mouse();

    bool isPressed(MouseButton button) const noexcept { return isButtonPressed_[int(button)]; }
    bool isDown   (MouseButton button) const noexcept { return isButtonDown_[int(button)]; }
    bool isUp     (MouseButton button) const noexcept { return isButtonUp_[int(button)]; }

    int getPositionX() const noexcept { return cursorX_; }
    int getPositionY() const noexcept { return cursorY_; }

    int getRelativePositionX() const noexcept { return relativeCursorX_; }
    int getRelativePositionY() const noexcept { return relativeCursorY_; }

    void setCursorLock(bool locked, int lockPositionX, int lockPositionY);
    void showCursor(bool show);

    bool isLocked()  const noexcept { return isCursorLocked_; }
    bool isVisible() const noexcept { return showCursor_; }

    int getLockX() const noexcept { return lockPositionX_; }
    int getLockY() const noexcept { return lockPositionY_; }

    void clearState();

    AGZ_D3D11_DECL_EVENT_MGR_HANDLER(eventMgr_, MouseButtonDownEvent)
    AGZ_D3D11_DECL_EVENT_MGR_HANDLER(eventMgr_, MouseButtonUpEvent)
    AGZ_D3D11_DECL_EVENT_MGR_HANDLER(eventMgr_, CursorMoveEvent)
    AGZ_D3D11_DECL_EVENT_MGR_HANDLER(eventMgr_, WheelScrollEvent)

    void _update();

    void _msg(UINT msg, WPARAM wParam);
};

AGZ_D3D11_END
