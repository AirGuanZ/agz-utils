#pragma once

#include <bitset>

#include <agz/utility/d3d11/Event.h>
#include <agz/utility/d3d11/KeyCode.h>

AGZ_D3D11_BEGIN

struct KeyDownEvent
{
    KeyCode key;
};

struct KeyUpEvent
{
    KeyCode key;
};

struct CharInputEvent
{
    uint32_t ch;
};

struct RawKeyDownEvent
{
    uint32_t vk;
};

struct RawKeyUpEvent
{
    uint32_t vk;
};

using KeyDownHandler    = FunctionalEventHandler<KeyDownEvent>;
using KeyUpHandler      = FunctionalEventHandler<KeyUpEvent>;
using CharInputHandler  = FunctionalEventHandler<CharInputEvent>;
using RawKeyDownHandler = FunctionalEventHandler<RawKeyDownEvent>;
using RawKeyUpHandler   = FunctionalEventHandler<RawKeyUpEvent>;

class KeyboardEventManager
    : public EventManager<KeyDownEvent, KeyUpEvent, CharInputEvent, RawKeyDownEvent, RawKeyUpEvent>
{
    friend class Window;
    
    std::bitset<KEY_MAX + 1> isKeyPressedLastFrame_;
    std::bitset<KEY_MAX + 1> isKeyPressed_;

    void UpdateSingleKey(bool pressed, KeyCode keycode) noexcept
    {
        if(pressed && !IsKeyPressed(keycode))
        {
            InvokeAllHandlers(KeyDownEvent{ keycode });
        }
        else if(!pressed && IsKeyPressed(keycode))
        {
            InvokeAllHandlers(KeyUpEvent{ keycode });
        }
    }

    void InvokeAllHandlers(const KeyDownEvent &e)
    {
        if(!IsKeyPressed(e.key))
        {
            isKeyPressed_[e.key] = true;
            EventManager::InvokeAllHandlers(e);
        }
    }

    void InvokeAllHandlers(const KeyUpEvent &e)
    {
        if(IsKeyPressed(e.key))
        {
            isKeyPressed_[e.key] = false;
            EventManager::InvokeAllHandlers(e);
        }
    }

    void InvokeAllHandlers(const CharInputEvent &e)
    {
        EventManager::InvokeAllHandlers(e);
    }

    void InvokeAllHandlers(const RawKeyDownEvent &e)
    {
        EventManager::InvokeAllHandlers(e);
    }

    void InvokeAllHandlers(const RawKeyUpEvent &e)
    {
        EventManager::InvokeAllHandlers(e);
    }

public:

    bool IsKeyDown(KeyCode key) const noexcept
    {
        return !isKeyPressedLastFrame_[key] && isKeyPressed_[key];
    }

    bool IsKeyUp(KeyCode key) const noexcept
    {
        return isKeyPressedLastFrame_[key] && !isKeyPressed_[key];
    }

    bool IsKeyPressed(KeyCode key) const noexcept
    {
        return isKeyPressed_[key];
    }

    void UpdateBeforeDoEvents()
    {
        isKeyPressedLastFrame_ = isKeyPressed_;
    }

    void UpdateAfterDoEvents()
    {
        bool leftShiftPressed  = (GetAsyncKeyState(VK_LSHIFT)   & 0x8000) != 0;
        bool rightShiftPressed = (GetAsyncKeyState(VK_RSHIFT)   & 0x8000) != 0;
        bool leftCtrlPressed   = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0;
        bool rightCtrlPressed  = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;

        UpdateSingleKey(leftShiftPressed,  KEY_LSHIFT);
        UpdateSingleKey(rightShiftPressed, KEY_RSHIFT);
        UpdateSingleKey(leftCtrlPressed,   KEY_LCTRL);
        UpdateSingleKey(rightCtrlPressed,  KEY_RCTRL);
    }

    void ClearState()
    {
        isKeyPressedLastFrame_.reset();
        isKeyPressed_.reset();
    }
};

AGZ_D3D11_END
