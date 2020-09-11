#pragma once

#include "./imgui.h"

namespace ImGui::Input
{

// button in { 0, 1, 2 }
void mouseButtonDown(int button);

// button in { 0, 1, 2 }
void mouseButtonUp(int button);

void mouseWheel(int offset);

void keyDown(int vk);

void keyUp(int vk);

void charInput(unsigned short ch);

} // namespace ImGui::Input
