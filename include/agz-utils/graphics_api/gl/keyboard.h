#pragma once

#include <bitset>

#include <agz-utils/event.h>
#include <agz-utils/graphics_api/gl/common.h>

AGZ_GL_BEGIN

using namespace event::keycode::keycode_constants;

using keycode_t = event::keycode::keycode_t;

struct key_down_event_t   { keycode_t key; };
struct key_up_event_t     { keycode_t key; };
struct char_input_event_t { uint32_t  ch;  };

using key_down_handler_t   = event::functional_receiver_t<key_down_event_t>;
using key_up_handler_t     = event::functional_receiver_t<key_up_event_t>;
using char_input_handler_t = event::functional_receiver_t<char_input_event_t>;

class keyboard_t
{
    using event_mgr_t = event::sender_t<
        key_down_event_t,
        key_up_event_t,
        char_input_event_t>;

    event_mgr_t event_mgr_;

    std::bitset<KEY_MAX + 1> last_frame_pressed_;
    std::bitset<KEY_MAX + 1> this_frame_pressed_;

public:

    bool is_down(keycode_t keycode) const noexcept;

    bool is_up(keycode_t keycode) const noexcept;

    bool is_pressed(keycode_t keycode) const noexcept;

    void clear_state();

    AGZ_GL_DECL_EVENT_MGR_HANDLER(event_mgr_, key_down_event_t)
    AGZ_GL_DECL_EVENT_MGR_HANDLER(event_mgr_, key_up_event_t)
    AGZ_GL_DECL_EVENT_MGR_HANDLER(event_mgr_, char_input_event_t)

    void _frame_update();

    void _key_down(keycode_t keycode);

    void _key_up(keycode_t keycode);

    void _input_char(uint32_t ch);
};

inline bool keyboard_t::is_down(keycode_t keycode) const noexcept
{
    return !last_frame_pressed_[keycode] && this_frame_pressed_[keycode];
}

inline bool keyboard_t::is_up(keycode_t keycode) const noexcept
{
    return last_frame_pressed_[keycode] && !this_frame_pressed_[keycode];
}

inline bool keyboard_t::is_pressed(keycode_t keycode) const noexcept
{
    return this_frame_pressed_[keycode];
}

inline void keyboard_t::clear_state()
{
    last_frame_pressed_.reset();
    this_frame_pressed_.reset();
}

inline void keyboard_t::_frame_update()
{
    last_frame_pressed_ = this_frame_pressed_;
}

inline void keyboard_t::_key_down(keycode_t keycode)
{
    if(!is_pressed(keycode))
    {
        this_frame_pressed_[keycode] = true;
        event_mgr_.send(key_down_event_t{ keycode });
    }
}

inline void keyboard_t::_key_up(keycode_t keycode)
{
    if(is_pressed(keycode))
    {
        this_frame_pressed_[keycode] = false;
        event_mgr_.send(key_up_event_t{ keycode });
    }
}

inline void keyboard_t::_input_char(uint32_t ch)
{
    event_mgr_.send(char_input_event_t{ ch });
}

AGZ_GL_END
