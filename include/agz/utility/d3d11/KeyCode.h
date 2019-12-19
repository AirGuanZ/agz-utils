#pragma once

#include <agz/utility/d3d11/Common.h>

AGZ_D3D11_BEGIN

using KeyCode = int32_t;

constexpr KeyCode KEY_UNKNOWN = -1;

constexpr KeyCode KEY_SPACE      = 32;
constexpr KeyCode KEY_APOSTROPHE = 39; // '
constexpr KeyCode KEY_COMMA      = 44; // ,
constexpr KeyCode KEY_MINUS      = 45; // -
constexpr KeyCode KEY_PERIOD     = 46; // .
constexpr KeyCode KEY_SLASH      = 47; // /

// 数字0-9与ASCII '0'-'9'相同

constexpr KeyCode KEY_D0 = 48;
constexpr KeyCode KEY_D1 = 49;
constexpr KeyCode KEY_D2 = 50;
constexpr KeyCode KEY_D3 = 51;
constexpr KeyCode KEY_D4 = 52;
constexpr KeyCode KEY_D5 = 53;
constexpr KeyCode KEY_D6 = 54;
constexpr KeyCode KEY_D7 = 55;
constexpr KeyCode KEY_D8 = 56;
constexpr KeyCode KEY_D9 = 57;

constexpr KeyCode KEY_SEMICOLON = 59; // ;
constexpr KeyCode KEY_EQUAL     = 61; // =

// 字母A-Z与ASCII 'A'-'Z'相同

constexpr KeyCode KEY_A = 65;
constexpr KeyCode KEY_B = 66;
constexpr KeyCode KEY_C = 67;
constexpr KeyCode KEY_D = 68;
constexpr KeyCode KEY_E = 69;
constexpr KeyCode KEY_F = 70;
constexpr KeyCode KEY_G = 71;
constexpr KeyCode KEY_H = 72;
constexpr KeyCode KEY_I = 73;
constexpr KeyCode KEY_J = 74;
constexpr KeyCode KEY_K = 75;
constexpr KeyCode KEY_L = 76;
constexpr KeyCode KEY_M = 77;
constexpr KeyCode KEY_N = 78;
constexpr KeyCode KEY_O = 79;
constexpr KeyCode KEY_P = 80;
constexpr KeyCode KEY_Q = 81;
constexpr KeyCode KEY_R = 82;
constexpr KeyCode KEY_S = 83;
constexpr KeyCode KEY_T = 84;
constexpr KeyCode KEY_U = 85;
constexpr KeyCode KEY_V = 86;
constexpr KeyCode KEY_W = 87;
constexpr KeyCode KEY_X = 88;
constexpr KeyCode KEY_Y = 89;
constexpr KeyCode KEY_Z = 90;

constexpr KeyCode KEY_LBRAC        = 91; // [
constexpr KeyCode KEY_BACKSLASH    = 92; // backslash
constexpr KeyCode KEY_RBRAC        = 93; // ]
constexpr KeyCode KEY_GRAVE_ACCENT = 96; // `

constexpr KeyCode KEY_ESCAPE    = 256;
constexpr KeyCode KEY_ENTER     = 257;
constexpr KeyCode KEY_TAB       = 258;
constexpr KeyCode KEY_BACKSPACE = 259;
constexpr KeyCode KEY_INSERT    = 260;
constexpr KeyCode KEY_DELETE    = 261;

constexpr KeyCode KEY_RIGHT = 262;
constexpr KeyCode KEY_LEFT  = 263;
constexpr KeyCode KEY_DOWN  = 264;
constexpr KeyCode KEY_UP    = 265;

constexpr KeyCode KEY_HOME      = 268;
constexpr KeyCode KEY_END       = 269;

constexpr KeyCode KEY_F1  = 290;
constexpr KeyCode KEY_F2  = 291;
constexpr KeyCode KEY_F3  = 292;
constexpr KeyCode KEY_F4  = 293;
constexpr KeyCode KEY_F5  = 294;
constexpr KeyCode KEY_F6  = 295;
constexpr KeyCode KEY_F7  = 296;
constexpr KeyCode KEY_F8  = 297;
constexpr KeyCode KEY_F9  = 298;
constexpr KeyCode KEY_F10 = 299;
constexpr KeyCode KEY_F11 = 300;
constexpr KeyCode KEY_F12 = 301;

constexpr KeyCode KEY_NUMPAD_0 = 320;
constexpr KeyCode KEY_NUMPAD_1 = 321;
constexpr KeyCode KEY_NUMPAD_2 = 322;
constexpr KeyCode KEY_NUMPAD_3 = 323;
constexpr KeyCode KEY_NUMPAD_4 = 324;
constexpr KeyCode KEY_NUMPAD_5 = 325;
constexpr KeyCode KEY_NUMPAD_6 = 326;
constexpr KeyCode KEY_NUMPAD_7 = 327;
constexpr KeyCode KEY_NUMPAD_8 = 328;
constexpr KeyCode KEY_NUMPAD_9 = 329;

constexpr KeyCode KEY_NUMPAD_DECIMAL = 330;
constexpr KeyCode KEY_NUMPAD_DIV     = 331;
constexpr KeyCode KEY_NUMPAD_MUL     = 332;
constexpr KeyCode KEY_NUMPAD_SUB     = 333;
constexpr KeyCode KEY_NUMPAD_ADD     = 334;
constexpr KeyCode KEY_NUMPAD_ENTER   = 335;

constexpr KeyCode KEY_LSHIFT = 340;
constexpr KeyCode KEY_LCTRL  = 341;
constexpr KeyCode KEY_LALT   = 342;
constexpr KeyCode KEY_RSHIFT = 344;
constexpr KeyCode KEY_RCTRL  = 345;
constexpr KeyCode KEY_RALT   = 346;

constexpr KeyCode KEY_MAX = 346;

KeyCode VK2KeyCode(int VK) noexcept;

AGZ_D3D11_END
