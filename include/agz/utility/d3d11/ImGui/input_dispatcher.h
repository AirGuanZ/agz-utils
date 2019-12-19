#pragma once

#include "./imgui.h"

namespace ImGui::Input
{

// button in { 0, 1, 2 }
void MouseButtonDown(int button);

// button in { 0, 1, 2 }
void MouseButtonUp(int button);

void MouseWheel(int offset);

void KeyDown(int vk);

void KeyUp(int vk);

void Char(unsigned short ch);

} // namespace ImGui::Input
