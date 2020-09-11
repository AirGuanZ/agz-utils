#pragma once

#include <bitset>

#include "../../event.h"
#include "common.h"

AGZ_D3D11_BEGIN

using namespace event::keycode::keycode_constants;

using KeyCode = event::keycode::keycode_t;

struct KeyDownEvent { KeyCode key; };
struct KeyUpEvent   { KeyCode key; };

struct CharInputEvent { uint32_t ch; };

struct RawKeyDownEvent { uint32_t vk; };
struct RawKeyUpEvent   { uint32_t vk; };

using KeyDownHandler    = event::functional_receiver_t<KeyDownEvent>;
using KeyUpHandler      = event::functional_receiver_t<KeyUpEvent>;
using CharInputHandler  = event::functional_receiver_t<CharInputEvent>;
using RawKeyDownHandler = event::functional_receiver_t<RawKeyDownEvent>;
using RawKeyUpHandler   = event::functional_receiver_t<RawKeyUpEvent>;

class Keyboard
{
    using EventMgr = event::sender_t<
        KeyDownEvent,
        KeyUpEvent,
        CharInputEvent,
        RawKeyDownEvent,
        RawKeyUpEvent>;

    EventMgr eventMgr_;

    std::bitset<KEY_MAX + 1> isKeyPressedLastFrame_;
    std::bitset<KEY_MAX + 1> isKeyPressedThisFrame_;

    void updateSingleKey(bool pressed, KeyCode keycode);

public:

    bool isDown(KeyCode keycode) const noexcept;

    bool isUp(KeyCode keycode) const noexcept;

    bool isPressed(KeyCode keycode) const noexcept;

    void clearState();

    AGZ_D3D11_DECL_EVENT_MGR_HANDLER(eventMgr_, KeyDownEvent)
    AGZ_D3D11_DECL_EVENT_MGR_HANDLER(eventMgr_, KeyUpEvent)
    AGZ_D3D11_DECL_EVENT_MGR_HANDLER(eventMgr_, CharInputEvent)
    AGZ_D3D11_DECL_EVENT_MGR_HANDLER(eventMgr_, RawKeyDownEvent)
    AGZ_D3D11_DECL_EVENT_MGR_HANDLER(eventMgr_, RawKeyUpEvent)

    void _startUpdating();

    void _endUpdating();

    void _msgDown(KeyCode keycode);

    void _msgUp(KeyCode keycode);

    void _msgChar(uint32_t ch);

    void _msgRawDown(uint32_t vk);

    void _msgRawUp(uint32_t vk);
};

inline void Keyboard::updateSingleKey(bool pressed, KeyCode keycode)
{
    const bool lastPressed = isPressed(keycode);
    if(pressed && !lastPressed)
        _msgDown(keycode);
    else if(!pressed && lastPressed)
        _msgUp(keycode);
}

inline bool Keyboard::isDown(KeyCode keycode) const noexcept
{
    return !isKeyPressedLastFrame_[keycode] && isKeyPressedThisFrame_[keycode];
}

inline bool Keyboard::isUp(KeyCode keycode) const noexcept
{
    return isKeyPressedLastFrame_[keycode] && !isKeyPressedThisFrame_[keycode];
}

inline bool Keyboard::isPressed(KeyCode keycode) const noexcept
{
    return isKeyPressedThisFrame_[keycode];
}

inline void Keyboard::clearState()
{
    isKeyPressedLastFrame_.reset();
    isKeyPressedThisFrame_.reset();
}

inline void Keyboard::_startUpdating()
{
    isKeyPressedLastFrame_ = isKeyPressedThisFrame_;
}

inline void Keyboard::_endUpdating()
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

inline void Keyboard::_msgDown(KeyCode keycode)
{
    if(!isPressed(keycode))
    {
        isKeyPressedThisFrame_[keycode] = true;
        eventMgr_.send(KeyDownEvent{ keycode });
    }
}

inline void Keyboard::_msgUp(KeyCode keycode)
{
    if(isPressed(keycode))
    {
        isKeyPressedThisFrame_[keycode] = false;
        eventMgr_.send(KeyUpEvent{ keycode });
    }
}

inline void Keyboard::_msgChar(uint32_t ch)
{
    eventMgr_.send(CharInputEvent{ ch });
}

inline void Keyboard::_msgRawDown(uint32_t vk)
{
    eventMgr_.send(RawKeyDownEvent{ vk });
}

inline void Keyboard::_msgRawUp(uint32_t vk)
{
    eventMgr_.send(RawKeyUpEvent{ vk });
}

AGZ_D3D11_END
