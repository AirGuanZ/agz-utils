#pragma once

#include <agz-utils/event.h>
#include <agz-utils/graphics_api/gl/common.h>

struct GLFWwindow;

AGZ_GL_BEGIN

enum mouse_button_t
{
    MB_Left   = 0,
    MB_Middle = 1,
    MB_Right  = 2
};

struct mouse_button_down_event_t { mouse_button_t button;   };
struct mouse_button_up_event_t   { mouse_button_t button;   };
struct cursor_move_event_t       { double x, y, relX, relY; };
struct wheel_scroll_event_t      { int offset;              };

using mouse_button_down_handler_t = event::functional_receiver_t<mouse_button_down_event_t>;
using mouse_button_up_handler_t   = event::functional_receiver_t<mouse_button_up_event_t>;
using cursor_move_handler_t       = event::functional_receiver_t<cursor_move_event_t>;
using wheel_scroll_handler_t      = event::functional_receiver_t<wheel_scroll_event_t>;

class mouse_t
{
    using event_mgr_t = event::sender_t<
        mouse_button_down_event_t,
        mouse_button_up_event_t,
        cursor_move_event_t,
        wheel_scroll_event_t>;

    event_mgr_t event_mgr_;

    GLFWwindow *glfw_window_ = nullptr;

    bool last_frame_pressed_[3] = { false, false, false };
    bool this_frame_pressed_[3] = { false, false, false };
    bool down_[3]               = { false, false, false };
    bool up_[3]                 = { false, false, false };

    double this_x_ = 0, this_y_ = 0;
    double rel_x_  = 0, rel_y_  = 0;

    bool is_locked_ = false;
    double lock_x_ = 0, lock_y_ = 0;

    bool show_ = true;

public:

    ~mouse_t();

    bool is_pressed(mouse_button_t button) const noexcept;
    bool is_down   (mouse_button_t button) const noexcept;;
    bool is_up     (mouse_button_t button) const noexcept;

    double get_cursor_x() const noexcept;
    double get_cursor_y() const noexcept;

    double get_relative_cursor_x() const noexcept;
    double get_relative_cursor_y() const noexcept;

    double get_cursor_lock_x() const noexcept;
    double get_cursor_lock_y() const noexcept;
    void set_cursor_lock(bool locked, double lock_x, double lock_y);
    bool is_cursor_locked() const noexcept;

    void show_cursor(bool show);
    bool is_cursor_visible() const noexcept;

    void clear_state();

    AGZ_GL_DECL_EVENT_MGR_HANDLER(event_mgr_, mouse_button_down_event_t)
    AGZ_GL_DECL_EVENT_MGR_HANDLER(event_mgr_, mouse_button_up_event_t)
    AGZ_GL_DECL_EVENT_MGR_HANDLER(event_mgr_, cursor_move_event_t)
    AGZ_GL_DECL_EVENT_MGR_HANDLER(event_mgr_, wheel_scroll_event_t)

    void _set_window(GLFWwindow *window);

    void _update();

    void _mb_down(mouse_button_t button);

    void _mb_up(mouse_button_t button);

    void _scroll(int offset);
};

inline mouse_t::~mouse_t()
{
    if(!show_)
        show_cursor(true);
}

inline bool mouse_t::is_pressed(mouse_button_t button) const noexcept
{
    return this_frame_pressed_[button];
}

inline bool mouse_t::is_down(mouse_button_t button) const noexcept
{
    return down_[button];
}

inline bool mouse_t::is_up(mouse_button_t button) const noexcept
{
    return up_[button];
}

inline double mouse_t::get_cursor_x() const noexcept
{
    return this_x_;
}

inline double mouse_t::get_cursor_y() const noexcept
{
    return this_y_;
}

inline double mouse_t::get_relative_cursor_x() const noexcept
{
    return rel_x_;
}

inline double mouse_t::get_relative_cursor_y() const noexcept
{
    return rel_y_;
}

inline double mouse_t::get_cursor_lock_x() const noexcept
{
    return lock_x_;
}

inline double mouse_t::get_cursor_lock_y() const noexcept
{
    return lock_y_;
}

inline void mouse_t::set_cursor_lock(bool locked, double lock_x, double lock_y)
{
    is_locked_ = locked;
    lock_x_ = lock_x;
    lock_y_ = lock_y;
}

inline bool mouse_t::is_cursor_locked() const noexcept
{
    return is_locked_;
}

inline bool mouse_t::is_cursor_visible() const noexcept
{
    return show_;
}

inline void mouse_t::clear_state()
{
    this_frame_pressed_[0] = this_frame_pressed_[1] = this_frame_pressed_[2] = false;
    last_frame_pressed_[0] = last_frame_pressed_[1] = last_frame_pressed_[2] = false;
    down_[0] = down_[1] = down_[2] = false;
    up_[0]   = up_[1]   = up_[2]   = false;

    this_x_ = this_y_ = 0;
    rel_x_  = rel_y_  = 0;

    is_locked_ = false;
    lock_x_ = lock_y_ = 0;

    if(!show_)
        show_cursor(true);
}

inline void mouse_t::_set_window(GLFWwindow *window)
{
    glfw_window_ = window;
}

inline void mouse_t::_mb_down(mouse_button_t button)
{
    this_frame_pressed_[button] = true;
    event_mgr_.send(mouse_button_down_event_t{ button });
}

inline void mouse_t::_mb_up(mouse_button_t button)
{
    this_frame_pressed_[button] = false;
    event_mgr_.send(mouse_button_up_event_t{ button });
}

inline void mouse_t::_scroll(int offset)
{
    event_mgr_.send(wheel_scroll_event_t{ offset });
}

AGZ_GL_END
