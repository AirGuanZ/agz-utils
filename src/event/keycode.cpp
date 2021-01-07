#include <agz-utils/event/keycode.h>

#ifdef AGZ_OS_WIN32

#include <Windows.h>

namespace agz::event::keycode
{

namespace
{
    struct keycode_table_t
    {
        keycode_t table[256] = {};

        keycode_table_t()
        {
            using namespace keycode_constants;

            for(auto &kc : table)
            {
                kc = KEY_UNKNOWN;
            }

            for(int i = 'A'; i <= 'Z'; ++i)
            {
                table[i] = keycode_t(i);
            }

            for(int i = '0'; i <= '9'; ++i)
            {
                table[i] = keycode_t(i);
            }

            table[VK_TAB]      = KEY_TAB;
            table[VK_RETURN]   = KEY_ENTER;
            table[VK_ESCAPE]   = KEY_ESCAPE;
            table[VK_SPACE]    = KEY_SPACE;
            table[VK_END]      = KEY_END;
            table[VK_HOME]     = KEY_HOME;
            table[VK_LEFT]     = KEY_LEFT;
            table[VK_UP]       = KEY_UP;
            table[VK_RIGHT]    = KEY_RIGHT;
            table[VK_DOWN]     = KEY_DOWN;
            table[VK_INSERT]   = KEY_INSERT;
            table[VK_DELETE]   = KEY_DELETE;
            table[VK_NUMPAD0]  = KEY_NUMPAD_0;
            table[VK_NUMPAD1]  = KEY_NUMPAD_1;
            table[VK_NUMPAD2]  = KEY_NUMPAD_2;
            table[VK_NUMPAD3]  = KEY_NUMPAD_3;
            table[VK_NUMPAD4]  = KEY_NUMPAD_4;
            table[VK_NUMPAD5]  = KEY_NUMPAD_5;
            table[VK_NUMPAD6]  = KEY_NUMPAD_6;
            table[VK_NUMPAD7]  = KEY_NUMPAD_7;
            table[VK_NUMPAD8]  = KEY_NUMPAD_8;
            table[VK_NUMPAD9]  = KEY_NUMPAD_9;
            table[VK_MULTIPLY] = KEY_NUMPAD_MUL;
            table[VK_ADD]      = KEY_NUMPAD_ADD;
            table[VK_SUBTRACT] = KEY_NUMPAD_SUB;
            table[VK_DECIMAL]  = KEY_NUMPAD_DECIMAL;
            table[VK_DIVIDE]   = KEY_NUMPAD_DIV;
            table[VK_F1]       = KEY_F1;
            table[VK_F2]       = KEY_F2;
            table[VK_F3]       = KEY_F3;
            table[VK_F4]       = KEY_F4;
            table[VK_F5]       = KEY_F5;
            table[VK_F6]       = KEY_F6;
            table[VK_F7]       = KEY_F7;
            table[VK_F8]       = KEY_F8;
            table[VK_F9]       = KEY_F9;
            table[VK_F10]      = KEY_F10;
            table[VK_F11]      = KEY_F11;
            table[VK_F12]      = KEY_F12;
        }
    } vk_table;
} // namespace anonymous

keycode_t win_vk_to_keycode(int VK) noexcept
{
    return vk_table.table[VK];
}

} // namespace agz::event::keycode

#endif
