#ifdef AGZ_ENABLE_GL

#include <map>

#include <agz/utility/graphics_api/gl/window.h>
#include <agz/utility/system.h>

#include <agz/utility/graphics_api/gl/imgui/imgui_impl_glfw.h>
#include <agz/utility/graphics_api/gl/imgui/imgui_impl_opengl3.h>
#include <agz/utility/graphics_api/imgui/imgui.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

AGZ_GL_BEGIN

struct window_t::impl_t
{
    GLFWwindow *window = nullptr;

    bool vsync = false;
    bool close = false;
    bool focus = false;

    vec2i framebuffer_size;

    keyboard_t keyboard;
    mouse_t    mouse;

    bool imgui = false;
};

namespace
{
    struct glfw_keycode_table_t
    {
        keycode_t keys[GLFW_KEY_LAST + 1];

        glfw_keycode_table_t()
            : keys{}
        {
            for(auto &k : keys)
                k = KEY_UNKNOWN;

            keys[GLFW_KEY_SPACE]      = KEY_SPACE;
            keys[GLFW_KEY_APOSTROPHE] = KEY_APOSTROPHE;
            keys[GLFW_KEY_COMMA]      = KEY_COMMA;
            keys[GLFW_KEY_MINUS]      = KEY_MINUS;
            keys[GLFW_KEY_PERIOD]     = KEY_PERIOD;
            keys[GLFW_KEY_SLASH]      = KEY_SLASH;

            keys[GLFW_KEY_SEMICOLON] = KEY_SEMICOLON;
            keys[GLFW_KEY_EQUAL]     = KEY_EQUAL;

            keys[GLFW_KEY_LEFT_BRACKET]  = KEY_LBRAC;
            keys[GLFW_KEY_BACKSLASH]     = KEY_BACKSLASH;
            keys[GLFW_KEY_RIGHT_BRACKET] = KEY_RBRAC;
            keys[GLFW_KEY_GRAVE_ACCENT]  = KEY_GRAVE_ACCENT;
            keys[GLFW_KEY_ESCAPE]        = KEY_ESCAPE;

            keys[GLFW_KEY_ENTER]     = KEY_ENTER;
            keys[GLFW_KEY_TAB]       = KEY_TAB;
            keys[GLFW_KEY_BACKSPACE] = KEY_BACKSPACE;
            keys[GLFW_KEY_INSERT]    = KEY_INSERT;
            keys[GLFW_KEY_DELETE]    = KEY_DELETE;

            keys[GLFW_KEY_RIGHT] = KEY_RIGHT;
            keys[GLFW_KEY_LEFT]  = KEY_LEFT;
            keys[GLFW_KEY_DOWN]  = KEY_DOWN;
            keys[GLFW_KEY_UP]    = KEY_UP;

            keys[GLFW_KEY_HOME] = KEY_HOME;
            keys[GLFW_KEY_END]  = KEY_END;

            keys[GLFW_KEY_F1]  = KEY_F1;
            keys[GLFW_KEY_F2]  = KEY_F2;
            keys[GLFW_KEY_F3]  = KEY_F3;
            keys[GLFW_KEY_F4]  = KEY_F4;
            keys[GLFW_KEY_F5]  = KEY_F5;
            keys[GLFW_KEY_F6]  = KEY_F6;
            keys[GLFW_KEY_F7]  = KEY_F7;
            keys[GLFW_KEY_F8]  = KEY_F8;
            keys[GLFW_KEY_F9]  = KEY_F9;
            keys[GLFW_KEY_F10] = KEY_F10;
            keys[GLFW_KEY_F11] = KEY_F11;
            keys[GLFW_KEY_F12] = KEY_F12;

            keys[GLFW_KEY_KP_0] = KEY_NUMPAD_0;
            keys[GLFW_KEY_KP_1] = KEY_NUMPAD_1;
            keys[GLFW_KEY_KP_2] = KEY_NUMPAD_2;
            keys[GLFW_KEY_KP_3] = KEY_NUMPAD_3;
            keys[GLFW_KEY_KP_4] = KEY_NUMPAD_4;
            keys[GLFW_KEY_KP_5] = KEY_NUMPAD_5;
            keys[GLFW_KEY_KP_6] = KEY_NUMPAD_6;
            keys[GLFW_KEY_KP_7] = KEY_NUMPAD_7;
            keys[GLFW_KEY_KP_8] = KEY_NUMPAD_8;
            keys[GLFW_KEY_KP_9] = KEY_NUMPAD_9;

            keys[GLFW_KEY_KP_DECIMAL]  = KEY_NUMPAD_DECIMAL;
            keys[GLFW_KEY_KP_DIVIDE]   = KEY_NUMPAD_DIV;
            keys[GLFW_KEY_KP_MULTIPLY] = KEY_NUMPAD_MUL;
            keys[GLFW_KEY_KP_SUBTRACT] = KEY_NUMPAD_SUB;
            keys[GLFW_KEY_KP_ADD]      = KEY_NUMPAD_ADD;
            keys[GLFW_KEY_KP_ENTER]    = KEY_NUMPAD_ENTER;

            keys[GLFW_KEY_LEFT_SHIFT]    = KEY_LSHIFT;
            keys[GLFW_KEY_LEFT_CONTROL]  = KEY_LCTRL;
            keys[GLFW_KEY_LEFT_ALT]      = KEY_LALT;
            keys[GLFW_KEY_RIGHT_SHIFT]   = KEY_RSHIFT;
            keys[GLFW_KEY_RIGHT_CONTROL] = KEY_RCTRL;
            keys[GLFW_KEY_RIGHT_ALT]     = KEY_RALT;

            for(int i = 0; i < 9; ++i)
                keys['0' + i] = KEY_D0 + i;

            for(int i = 0; i < 26; ++i)
                keys['A' + i] = KEY_A + i;
        }
    };

    std::map<GLFWwindow*, window_t*>  g_glfw_windows;

    bool find_window(GLFWwindow *glfw_window, window_t **win_ptr)
    {
        const auto it = g_glfw_windows.find(glfw_window);
        if(it == g_glfw_windows.end())
            return false;
        *win_ptr = it->second;
        return true;
    }

    void close_callback(GLFWwindow *glfw_window)
    {
        window_t *window;
        if(!find_window(glfw_window, &window))
            return;
        window->_close(glfwWindowShouldClose(glfw_window));
        glfwSetWindowShouldClose(glfw_window, GLFW_FALSE);
    }

    void resize_callback(GLFWwindow *glfw_window, int w, int h)
    {
        window_t *window;
        if(!find_window(glfw_window, &window))
            return;
        window->_resize();
    }

    void focus_callback(GLFWwindow *glfw_window, int focus)
    {
        window_t *window;
        if(!find_window(glfw_window, &window))
            return;
        window->_focus(focus != 0);
    }

    void key_callback(
        GLFWwindow *glfw_window,
        int         key,
        int         scancode,
        int         action,
        int         mods)
    {
        window_t *window;
        if(!find_window(glfw_window, &window))
            return;

        if(window->_imgui())
            ImGui_ImplGlfw_KeyCallback(glfw_window, key, scancode, action, mods);

        static const glfw_keycode_table_t table;
        if(key < 0 || key >= static_cast<int>(array_size(table.keys)))
            return;

        const keycode_t keycode = table.keys[key];
        if(keycode == KEY_UNKNOWN)
            return;

        if(action == GLFW_PRESS)
            window->_key(keycode, true);
        else if(action == GLFW_RELEASE)
            window->_key(keycode, false);
    }

    void char_callback(
        GLFWwindow  *glfw_window,
        unsigned int c)
    {
        window_t *window;
        if(!find_window(glfw_window, &window))
            return;

        if(window->_imgui())
            ImGui_ImplGlfw_CharCallback(glfw_window, c);

        window->_char(c);
    }

    void mouse_button_callback(
        GLFWwindow *glfw_window,
        int         button,
        int         action,
        int         mods)
    {
        window_t *window;
        if(!find_window(glfw_window, &window))
            return;

        if(window->_imgui())
            ImGui_ImplGlfw_MouseButtonCallback(glfw_window, button, action, mods);

        mouse_button_t btn;
        if(button == GLFW_MOUSE_BUTTON_LEFT)
            btn = MB_Left;
        else if(button == GLFW_MOUSE_BUTTON_MIDDLE)
            btn = MB_Middle;
        else if(button == GLFW_MOUSE_BUTTON_RIGHT)
            btn = MB_Right;
        else
            return;

        if(action == GLFW_PRESS)
            window->_mb_down(btn);
        else if(action == GLFW_RELEASE)
            window->_mb_up(btn);
    }

    void scroll_callback(
        GLFWwindow *glfw_window,
        double      xoffset,
        double      yoffset)
    {
        window_t *window;
        if(!find_window(glfw_window, &window))
            return;

        if(window->_imgui())
            ImGui_ImplGlfw_ScrollCallback(glfw_window, xoffset, yoffset);

        if(yoffset != 0)
            window->_wheel_scroll(static_cast<int>(yoffset));
    }
}

window_t::window_t(const window_desc_t &desc, bool maximized)
{
    impl_ = std::make_unique<impl_t>();

    if(g_glfw_windows.empty())
    {
        if(glfwInit() != GLFW_TRUE)
            throw gl_exception_t("failed to initialize glfw");
    }

    misc::fixed_scope_guard_t glfw_guard([&]
    {
        glfwTerminate();
    });

    if(!g_glfw_windows.empty())
        glfw_guard.dismiss();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_RESIZABLE, desc.resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, maximized      ? GLFW_TRUE : GLFW_FALSE);

#ifdef AGZ_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif

    switch(desc.depth_stencil_format)
    {
    case window_desc_t::d0s0:
        glfwWindowHint(GLFW_DEPTH_BITS, 0);
        glfwWindowHint(GLFW_STENCIL_BITS, 0);
        break;
    case window_desc_t::d24s8:
        glfwWindowHint(GLFW_DEPTH_BITS, 24);
        glfwWindowHint(GLFW_STENCIL_BITS, 0);
        break;
    case window_desc_t::d32s0:
        glfwWindowHint(GLFW_DEPTH_BITS, 32);
        glfwWindowHint(GLFW_STENCIL_BITS, 0);
        break;
    }

    glfwWindowHint(GLFW_SAMPLES, desc.multisamples);

    impl_->window = glfwCreateWindow(
        desc.size.x, desc.size.y, desc.title.c_str(),
        desc.fullscreen ? glfwGetPrimaryMonitor() : nullptr,
        nullptr);
    if(!impl_->window)
        throw gl_exception_t("failed to create glfw window");

    misc::fixed_scope_guard_t window_guard([&]
    {
        glfwDestroyWindow(impl_->window);
    });

    glfwMakeContextCurrent(impl_->window);

    glfwFocusWindow(impl_->window);
    impl_->focus = true;

    set_vsync(desc.vsync);

    if(g_glfw_windows.empty())
    {
        if(glewInit() != GLEW_OK)
            throw gl_exception_t("failed to initialize glew");
    }

    g_glfw_windows.insert({ impl_->window, this });

    impl_->mouse._set_window(impl_->window);

    glfwSetWindowCloseCallback    (impl_->window, close_callback);
    glfwSetFramebufferSizeCallback(impl_->window, resize_callback);
    glfwSetWindowFocusCallback    (impl_->window, focus_callback);
    glfwSetKeyCallback            (impl_->window, key_callback);
    glfwSetMouseButtonCallback    (impl_->window, mouse_button_callback);
    glfwSetScrollCallback         (impl_->window, scroll_callback);
    glfwSetCharCallback           (impl_->window, char_callback);

    _resize();

    // imgui

    impl_->imgui = desc.imgui;

    if(desc.imgui)
    {
        ImGui::CreateContext();
        ImGui::StyleColorsLight();

        ImGui_ImplGlfw_InitForOpenGL(impl_->window, false);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    // cancel guards

    window_guard.dismiss();
    glfw_guard.dismiss();
}

window_t::~window_t()
{
    if(impl_->imgui)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(ImGui::GetCurrentContext());
    }

    glfwSetWindowCloseCallback    (impl_->window, nullptr);
    glfwSetFramebufferSizeCallback(impl_->window, nullptr);
    glfwSetWindowFocusCallback    (impl_->window, nullptr);
    glfwSetKeyCallback            (impl_->window, nullptr);
    glfwSetMouseButtonCallback    (impl_->window, nullptr);
    glfwSetScrollCallback         (impl_->window, nullptr);
    glfwSetCharCallback           (impl_->window, nullptr);

    g_glfw_windows.erase(impl_->window);

    glfwDestroyWindow(impl_->window);

    if(g_glfw_windows.empty())
        glfwTerminate();
}

void window_t::new_imgui_frame()
{
    assert(impl_->imgui);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void window_t::render_imgui()
{
    assert(impl_->imgui);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

keyboard_t *window_t::get_keyboard() noexcept
{
    return &impl_->keyboard;
}

mouse_t *window_t::get_mouse() noexcept
{
    return &impl_->mouse;
}

void window_t::do_events()
{
    impl_->keyboard._frame_update();
    glfwPollEvents();
    impl_->mouse._update();
}

bool window_t::is_in_focus()
{
    return impl_->focus;
}

void window_t::wait_for_focus()
{
    if(is_in_focus())
        return;

    auto mouse = get_mouse();

    const bool   visible = mouse->is_cursor_visible();
    const bool   locked  = mouse->is_cursor_locked();
    const double lock_x  = mouse->get_cursor_lock_x();
    const double lock_y  = mouse->get_cursor_lock_y();

    mouse->show_cursor(true);
    mouse->set_cursor_lock(false, lock_x, lock_y);

    do
    {
        glfwWaitEvents();
    } while(!is_in_focus());

    mouse->show_cursor(visible);
    mouse->set_cursor_lock(locked, lock_x, lock_y);
    mouse->_update();
}

bool window_t::get_close_flag() const noexcept
{
    return impl_->close;
}

void window_t::set_close_flag(bool flag) noexcept
{
    impl_->close = flag;
}

void window_t::set_maximized()
{
    glfwMaximizeWindow(impl_->window);
}

int window_t::get_framebuffer_width() const noexcept
{
    return impl_->framebuffer_size.x;
}

int window_t::get_framebuffer_height() const noexcept
{
    return impl_->framebuffer_size.y;
}

vec2i window_t::get_framebuffer_size() const noexcept
{
    return impl_->framebuffer_size;
}

float window_t::get_framebuffer_w_over_h() const noexcept
{
    return static_cast<float>(impl_->framebuffer_size.x)
                            / impl_->framebuffer_size.y;
}

void window_t::swap_buffers()
{
    glfwSwapBuffers(impl_->window);
}

bool window_t::get_vsync() const noexcept
{
    return impl_->vsync;
}

void window_t::set_vsync(bool vsync) noexcept
{
    impl_->vsync = vsync;
    glfwSwapInterval(vsync ? 1 : 0);
}

void window_t::use_default_viewport() const
{
    glViewport(0, 0, impl_->framebuffer_size.x, impl_->framebuffer_size.y);
}

bool window_t::_imgui() const noexcept
{
    return impl_->imgui;
}

void window_t::_close(bool close_flag)
{
    impl_->close = close_flag;
    if(impl_->close)
        event_mgr_.send(window_close_event_t{});
}

void window_t::_resize()
{
    glfwGetFramebufferSize(
        impl_->window,
        &impl_->framebuffer_size.x,
        &impl_->framebuffer_size.y);

    use_default_viewport();

    event_mgr_.send(window_resize_event_t{
        impl_->framebuffer_size.x,
        impl_->framebuffer_size.y
    });
}

void window_t::_focus(bool focus)
{
    impl_->focus = focus;
    event_mgr_.send(window_focus_event_t{ focus });
}

void window_t::_key(keycode_t keycode, bool pressed)
{
    if(pressed)
        impl_->keyboard._key_down(keycode);
    else
        impl_->keyboard._key_up(keycode);
}

void window_t::_char(uint32_t ch)
{
    impl_->keyboard._input_char(ch);
}

void window_t::_mb_down(mouse_button_t button)
{
    impl_->mouse._mb_down(button);
}

void window_t::_mb_up(mouse_button_t button)
{
    impl_->mouse._mb_up(button);
}

void window_t::_wheel_scroll(int offset)
{
    impl_->mouse._scroll(offset);
}

AGZ_GL_END

#endif // #ifdef AGZ_ENABLE_GL
