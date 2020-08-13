#pragma once

#ifdef AGZ_ENABLE_OPENGL

#include "./graphics_api/opengl/attrib.h"
#include "./graphics_api/opengl/buffer.h"
#include "./graphics_api/opengl/framebuffer.h"
#include "./graphics_api/opengl/program.h"
#include "./graphics_api/opengl/renderbuffer.h"
#include "./graphics_api/opengl/sampler.h"
#include "./graphics_api/opengl/shader.h"
#include "./graphics_api/opengl/texture2d.h"
#include "./graphics_api/opengl/uniform.h"
#include "./graphics_api/opengl/vertex_array.h"

#endif // #ifdef AGZ_ENABLE_OPENGL

#ifdef AGZ_ENABLE_D3D11

#include "./graphics_api/d3d11/imgui/imgui.h"
#include "./graphics_api/d3d11/imgui/imfilebrowser.h"

#include "./graphics_api/d3d11/buffer.h"
#include "./graphics_api/d3d11/immediate.h"
#include "./graphics_api/d3d11/inputLayout.h"
#include "./graphics_api/d3d11/keyboard.h"
#include "./graphics_api/d3d11/mouse.h"
#include "./graphics_api/d3d11/pipelineState.h"
#include "./graphics_api/d3d11/renderTarget.h"
#include "./graphics_api/d3d11/shader.h"
#include "./graphics_api/d3d11/texture2d.h"
#include "./graphics_api/d3d11/window.h"

#endif // #ifdef AGZ_ENABLE_D3D11
