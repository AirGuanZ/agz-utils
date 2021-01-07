#if defined(AGZ_ENABLE_D3D11) || defined(AGZ_ENABLE_D3D12)

#include <agz-utils/graphics_api/imgui/imgui.h>
#include <agz-utils/graphics_api/imgui/inputDispatcher.h>

namespace ImGui::Input
{

void mouseButtonDown(int button)
{
    if(!GetCurrentContext())
        return;
    GetIO().MouseDown[button] = true;
}

// button in { 0, 1, 2 }
void mouseButtonUp(int button)
{
    if(!GetCurrentContext())
        return;
    GetIO().MouseDown[button] = false;
}

void mouseWheel(int offset)
{
    if(!GetCurrentContext())
        return;
    GetIO().MouseWheel += offset / 120.0f;
}

void keyDown(int vk)
{
    if(!GetCurrentContext())
        return;
    if(vk < 256)
        GetIO().KeysDown[vk] = 1;
}

void keyUp(int vk)
{
    if(!GetCurrentContext())
        return;
    if(vk < 256)
        GetIO().KeysDown[vk] = 0;
}

void charInput(unsigned short ch)
{
    if(!GetCurrentContext())
        return;
    GetIO().AddInputCharacter(ch);
}

} // namespace ImGui::Input

#endif // #if defined(AGZ_ENABLE_D3D11) || defined(AGZ_ENABLE_D3D12)
