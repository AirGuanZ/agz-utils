#pragma once

#include <bitset>

#include <agz/utility/event.h>
#include <agz/utility/graphics_api/d3d12/common.h>

AGZ_D3D12_BEGIN

using namespace event::keycode::keycode_constants;

using KeyCode = event::keycode::keycode_t;

enum class MouseButton
{
    Left   = 0,
    Middle = 1,
    Right  = 2
};

struct KeyDownEvent    { KeyCode key; };
struct KeyUpEvent      { KeyCode key; };
struct CharInputEvent  { uint32_t ch; };
struct RawKeyDownEvent { uint32_t vk; };
struct RawKeyUpEvent   { uint32_t vk; };

struct MouseButtonDownEvent { MouseButton button; };
struct MouseButtonUpEvent   { MouseButton button; };
struct CursorMoveEvent      { int x, y, relX, relY; };
struct WheelScrollEvent     { int offset; };

using KeyDownHandler    = event::functional_receiver_t<KeyDownEvent>;
using KeyUpHandler      = event::functional_receiver_t<KeyUpEvent>;
using CharInputHandler  = event::functional_receiver_t<CharInputEvent>;
using RawKeyDownHandler = event::functional_receiver_t<RawKeyDownEvent>;
using RawKeyUpHandler   = event::functional_receiver_t<RawKeyUpEvent>;

using MouseButtonDownHandler = event::functional_receiver_t<MouseButtonDownEvent>;
using MouseButtonUpHandler   = event::functional_receiver_t<MouseButtonUpEvent>;
using CursorMoveHandler      = event::functional_receiver_t<CursorMoveEvent>;
using WheelScrollHandler     = event::functional_receiver_t<WheelScrollEvent>;

class Input
{
    using EventSender = event::sender_t<
        KeyDownEvent,
        KeyUpEvent,
        CharInputEvent,
        RawKeyDownEvent,
        RawKeyUpEvent,
        MouseButtonDownEvent,
        MouseButtonUpEvent,
        CursorMoveEvent,
        WheelScrollEvent>;

    EventSender eventSender_;

    std::bitset<KEY_MAX + 1> isKeyPressedLastFrame_;
    std::bitset<KEY_MAX + 1> isKeyPressedThisFrame_;

    HWND hWindow_ = nullptr;

    bool isMBPressed_         [3] = { false, false, false };
    bool isMBPressedLastFrame_[3] = { false, false, false };

    bool isMBDown_[3] = { false, false, false };
    bool isMBUp_  [3] = { false, false, false };

    int curX_ = 0, curY_ = 0;
    int relX_ = 0, relY_ = 0;

    bool isLocked_ = false;
    int lockX_ = 0, lockY_ = 0;

    bool showCursor_ = true;

    void updateSingleKey(bool pressed, KeyCode keycode);

public:

    void setWindowHandle(HWND hWindow);

    bool isPressed(KeyCode key) const noexcept;
    bool isDown   (KeyCode key) const noexcept;
    bool isUp     (KeyCode key) const noexcept;

    bool isPressed(MouseButton button) const noexcept;
    bool isDown   (MouseButton button) const noexcept;
    bool isUp     (MouseButton button) const noexcept;

    int getCursorX() const noexcept;
    int getCursorY() const noexcept;

    int getCursorRelativeX() const noexcept;
    int getCursorRelativeY() const noexcept;

    void setCursorLock(bool locked, int lockX = 0, int lockY = 0);
    bool isCursorLocked() const noexcept;

    int getCursorLockX() const noexcept;
    int getCursorLockY() const noexcept;

    void showCursor(bool show);
    bool isCursorVisible() const noexcept;

    void clearState();

    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(eventSender_, KeyDownEvent)
    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(eventSender_, KeyUpEvent)
    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(eventSender_, CharInputEvent)
    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(eventSender_, RawKeyDownEvent)
    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(eventSender_, RawKeyUpEvent)

    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(eventSender_, MouseButtonDownEvent)
    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(eventSender_, MouseButtonUpEvent)
    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(eventSender_, CursorMoveEvent)
    AGZ_D3D12_DECL_EVENT_SENDER_HANDLER(eventSender_, WheelScrollEvent)

    void _startUpdatingKeyboard();

    void _endUpdatingKeyboard();

    void _updateMouse();

    void _msgMouse(UINT msg, WPARAM wParam);

    void _msgDown(KeyCode keycode);

    void _msgUp(KeyCode keycode);

    void _msgChar(uint32_t ch);

    void _msgRawDown(uint32_t vk);

    void _msgRawUp(uint32_t vk);
};

inline void Input::setWindowHandle(HWND hWindow)
{
    hWindow_ = hWindow;
}

inline bool Input::isPressed(KeyCode key) const noexcept
{
    return isKeyPressedThisFrame_[key];
}

inline bool Input::isDown(KeyCode key) const noexcept
{
    return !isKeyPressedLastFrame_[key] && isKeyPressedThisFrame_[key];
}

inline bool Input::isUp(KeyCode key) const noexcept
{
    return isKeyPressedLastFrame_[key] && !isKeyPressedThisFrame_[key];
}

inline bool Input::isPressed(MouseButton button) const noexcept
{
    return isMBPressed_[static_cast<int>(button)];
}

inline bool Input::isDown(MouseButton button) const noexcept
{
    return isMBDown_[static_cast<int>(button)];
}

inline bool Input::isUp(MouseButton button) const noexcept
{
    return isMBUp_[static_cast<int>(button)];
}

inline int Input::getCursorX() const noexcept
{
    return curX_;
}

inline int Input::getCursorY() const noexcept
{
    return curY_;
}

inline int Input::getCursorRelativeX() const noexcept
{
    return relX_;
}

inline int Input::getCursorRelativeY() const noexcept
{
    return relY_;
}

inline void Input::setCursorLock(bool locked, int lockX, int lockY)
{
    isLocked_ = locked;
    lockX_ = lockX;
    lockY_ = lockY;
}

inline bool Input::isCursorLocked() const noexcept
{
    return isLocked_;
}

inline int Input::getCursorLockX() const noexcept
{
    return lockX_;
}

inline int Input::getCursorLockY() const noexcept
{
    return lockY_;
}

inline bool Input::isCursorVisible() const noexcept
{
    return showCursor_;
}

AGZ_D3D12_END
