#pragma once

#include <cstdint>

#include <agz/utility/system.h>

namespace agz::event::keycode
{

using keycode_t = int32_t;

namespace keycode_values
{

    constexpr keycode_t KEY_UNKNOWN = -1;
    
    constexpr keycode_t KEY_SPACE      = 32;
    constexpr keycode_t KEY_APOSTROPHE = 39; // '
    constexpr keycode_t KEY_COMMA      = 44; // ,
    constexpr keycode_t KEY_MINUS      = 45; // -
    constexpr keycode_t KEY_PERIOD     = 46; // .
    constexpr keycode_t KEY_SLASH      = 47; // /
    
    // 数字0-9与ASCII '0'-'9'相同
    
    constexpr keycode_t KEY_D0 = 48;
    constexpr keycode_t KEY_D1 = 49;
    constexpr keycode_t KEY_D2 = 50;
    constexpr keycode_t KEY_D3 = 51;
    constexpr keycode_t KEY_D4 = 52;
    constexpr keycode_t KEY_D5 = 53;
    constexpr keycode_t KEY_D6 = 54;
    constexpr keycode_t KEY_D7 = 55;
    constexpr keycode_t KEY_D8 = 56;
    constexpr keycode_t KEY_D9 = 57;
    
    constexpr keycode_t KEY_SEMICOLON = 59; // ;
    constexpr keycode_t KEY_EQUAL     = 61; // =
    
    // 字母A-Z与ASCII 'A'-'Z'相同
    
    constexpr keycode_t KEY_A = 65;
    constexpr keycode_t KEY_B = 66;
    constexpr keycode_t KEY_C = 67;
    constexpr keycode_t KEY_D = 68;
    constexpr keycode_t KEY_E = 69;
    constexpr keycode_t KEY_F = 70;
    constexpr keycode_t KEY_G = 71;
    constexpr keycode_t KEY_H = 72;
    constexpr keycode_t KEY_I = 73;
    constexpr keycode_t KEY_J = 74;
    constexpr keycode_t KEY_K = 75;
    constexpr keycode_t KEY_L = 76;
    constexpr keycode_t KEY_M = 77;
    constexpr keycode_t KEY_N = 78;
    constexpr keycode_t KEY_O = 79;
    constexpr keycode_t KEY_P = 80;
    constexpr keycode_t KEY_Q = 81;
    constexpr keycode_t KEY_R = 82;
    constexpr keycode_t KEY_S = 83;
    constexpr keycode_t KEY_T = 84;
    constexpr keycode_t KEY_U = 85;
    constexpr keycode_t KEY_V = 86;
    constexpr keycode_t KEY_W = 87;
    constexpr keycode_t KEY_X = 88;
    constexpr keycode_t KEY_Y = 89;
    constexpr keycode_t KEY_Z = 90;
    
    constexpr keycode_t KEY_LBRAC        = 91; // [
    constexpr keycode_t KEY_BACKSLASH    = 92; // backslash
    constexpr keycode_t KEY_RBRAC        = 93; // ]
    constexpr keycode_t KEY_GRAVE_ACCENT = 96; // `
    
    constexpr keycode_t KEY_ESCAPE    = 256;
    constexpr keycode_t KEY_ENTER     = 257;
    constexpr keycode_t KEY_TAB       = 258;
    constexpr keycode_t KEY_BACKSPACE = 259;
    constexpr keycode_t KEY_INSERT    = 260;
    constexpr keycode_t KEY_DELETE    = 261;
    
    constexpr keycode_t KEY_RIGHT = 262;
    constexpr keycode_t KEY_LEFT  = 263;
    constexpr keycode_t KEY_DOWN  = 264;
    constexpr keycode_t KEY_UP    = 265;
    
    constexpr keycode_t KEY_HOME      = 268;
    constexpr keycode_t KEY_END       = 269;
    
    constexpr keycode_t KEY_F1  = 290;
    constexpr keycode_t KEY_F2  = 291;
    constexpr keycode_t KEY_F3  = 292;
    constexpr keycode_t KEY_F4  = 293;
    constexpr keycode_t KEY_F5  = 294;
    constexpr keycode_t KEY_F6  = 295;
    constexpr keycode_t KEY_F7  = 296;
    constexpr keycode_t KEY_F8  = 297;
    constexpr keycode_t KEY_F9  = 298;
    constexpr keycode_t KEY_F10 = 299;
    constexpr keycode_t KEY_F11 = 300;
    constexpr keycode_t KEY_F12 = 301;
    
    constexpr keycode_t KEY_NUMPAD_0 = 320;
    constexpr keycode_t KEY_NUMPAD_1 = 321;
    constexpr keycode_t KEY_NUMPAD_2 = 322;
    constexpr keycode_t KEY_NUMPAD_3 = 323;
    constexpr keycode_t KEY_NUMPAD_4 = 324;
    constexpr keycode_t KEY_NUMPAD_5 = 325;
    constexpr keycode_t KEY_NUMPAD_6 = 326;
    constexpr keycode_t KEY_NUMPAD_7 = 327;
    constexpr keycode_t KEY_NUMPAD_8 = 328;
    constexpr keycode_t KEY_NUMPAD_9 = 329;
    
    constexpr keycode_t KEY_NUMPAD_DECIMAL = 330;
    constexpr keycode_t KEY_NUMPAD_DIV     = 331;
    constexpr keycode_t KEY_NUMPAD_MUL     = 332;
    constexpr keycode_t KEY_NUMPAD_SUB     = 333;
    constexpr keycode_t KEY_NUMPAD_ADD     = 334;
    constexpr keycode_t KEY_NUMPAD_ENTER   = 335;
    
    constexpr keycode_t KEY_LSHIFT = 340;
    constexpr keycode_t KEY_LCTRL  = 341;
    constexpr keycode_t KEY_LALT   = 342;
    constexpr keycode_t KEY_RSHIFT = 344;
    constexpr keycode_t KEY_RCTRL  = 345;
    constexpr keycode_t KEY_RALT   = 346;
    
    constexpr keycode_t KEY_MAX = 346;

} // namespace keycode_values

#ifdef AGZ_OS_WIN32

keycode_t win_vk_to_keycode(int VK) noexcept;

#endif

} // namespace agz::event::keycode
