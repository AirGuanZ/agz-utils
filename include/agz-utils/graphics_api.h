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

#ifdef AGZ_ENABLE_GL

#include "./graphics_api/gl/buffer.h"
#include "./graphics_api/gl/immediate2d.h"
#include "./graphics_api/gl/keyboard.h"
#include "./graphics_api/gl/mouse.h"
#include "./graphics_api/gl/shader.h"
#include "./graphics_api/gl/vertex_array_object.h"
#include "./graphics_api/gl/window.h"

#endif // #ifdef AGZ_ENABLE_GL

#ifdef AGZ_ENABLE_D3D11

#include "./graphics_api/imgui/imgui.h"
#include "./graphics_api/imgui/imfilebrowser.h"

#include "./graphics_api/d3d11/buffer.h"
#include "./graphics_api/d3d11/demo.h"
#include "./graphics_api/d3d11/device.h"
#include "./graphics_api/d3d11/deviceContext.h"
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

#ifdef AGZ_ENABLE_D3D12

#include "./graphics_api/imgui/imgui.h"
#include "./graphics_api/imgui/imfilebrowser.h"

#include "./graphics_api/d3d12/raytracing/shaderTable.h"

#include "./graphics_api/d3d12/graph/graph.h"

#include "./graphics_api/d3d12/buffer.h"
#include "./graphics_api/d3d12/commandList.h"
#include "./graphics_api/d3d12/d3d12Context.h"
#include "./graphics_api/d3d12/debug.h"
#include "./graphics_api/d3d12/descriptorAllocator.h"
#include "./graphics_api/d3d12/descriptorHeap.h"
#include "./graphics_api/d3d12/descriptorHeapRaw.h"
#include "./graphics_api/d3d12/device.h"
#include "./graphics_api/d3d12/frameFence.h"
#include "./graphics_api/d3d12/imguiIntegration.h"
#include "./graphics_api/d3d12/immediateCommandList.h"
#include "./graphics_api/d3d12/input.h"
#include "./graphics_api/d3d12/mipmapGenerator.h"
#include "./graphics_api/d3d12/pipeline.h"
#include "./graphics_api/d3d12/queueWaiter.h"
#include "./graphics_api/d3d12/resourceManager.h"
#include "./graphics_api/d3d12/resourceUploader.h"
#include "./graphics_api/d3d12/rootSignature.h"
#include "./graphics_api/d3d12/shaderCompiler.h"
#include "./graphics_api/d3d12/swapChain.h"
#include "./graphics_api/d3d12/textureLoader.h"
#include "./graphics_api/d3d12/viewDescHelper.h"
#include "./graphics_api/d3d12/window.h"

#endif // #ifdef AGZ_ENABLE_D3D12

#if defined(AGZ_ENABLE_GL) || defined(AGZ_ENABLE_D3D11)

#include "./graphics_api/imgui/imgui.h"
#include "./graphics_api/imgui/imfilebrowser.h"

#endif // #if defined(AGZ_ENABLE_GL) || defined(AGZ_ENABLE_D3D11)
