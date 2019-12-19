#ifdef AGZ_ENABLE_D3D11

#include <agz/utility/d3d11/ImGui/input_dispatcher.h>

namespace ImGui::Input
{

void MouseButtonDown(int button)
{
    if(!GetCurrentContext())
        return;
    GetIO().MouseDown[button] = true;
}

// button in { 0, 1, 2 }
void MouseButtonUp(int button)
{
    if(!GetCurrentContext())
        return;
    GetIO().MouseDown[button] = false;
}

void MouseWheel(int offset)
{
    if(!GetCurrentContext())
        return;
    GetIO().MouseWheel += offset;
}

void KeyDown(int vk)
{
    if(!GetCurrentContext())
        return;
    if(vk < 256)
        GetIO().KeysDown[vk] = 1;
}

void KeyUp(int vk)
{
    if(!GetCurrentContext())
        return;
    if(vk < 256)
        GetIO().KeysDown[vk] = 0;
}

void Char(unsigned short ch)
{
    if(!GetCurrentContext())
        return;
    GetIO().AddInputCharacter(ch);
}

} // namespace ImGui::Input

#endif // #ifdef AGZ_ENABLE_D3D11
