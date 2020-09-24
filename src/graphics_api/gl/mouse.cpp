#ifdef AGZ_ENABLE_GL

#include <agz/utility/graphics_api/gl/mouse.h>

AGZ_GL_BEGIN

void mouse_t::show_cursor(bool show)
{
    show_ = show;
    glfwSetInputMode(
        glfw_window_, GLFW_CURSOR,
        show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
}

void mouse_t::_update()
{
    double new_x, new_y;
    glfwGetCursorPos(glfw_window_, &new_x, &new_y);

    rel_x_ = new_x - this_x_;
    rel_y_ = new_y - this_y_;
    this_x_ = new_x;
    this_y_ = new_y;

    if(rel_x_ || rel_y_)
        event_mgr_.send(cursor_move_event_t{ new_x, new_y, rel_x_, rel_y_ });

    if(is_locked_)
    {
        glfwSetCursorPos(glfw_window_, lock_x_, lock_y_);
        this_x_ = lock_x_;
        this_y_ = lock_y_;
    }

    for(int i = 0; i < 3; ++i)
    {
        down_[i] = !last_frame_pressed_[i] &&  this_frame_pressed_[i];
        up_[i]   =  last_frame_pressed_[i] && !this_frame_pressed_[i];

        last_frame_pressed_[i] = this_frame_pressed_[i];
    }
}

AGZ_GL_END

#endif // #ifdef AGZ_ENABLE_GL
