#pragma once

#include <functional>

#include <agz-utils/graphics_api/d3d12/descriptorAllocator.h>
#include <agz-utils/misc.h>

#define AGZ_D3D12_RENDERGRAPH_BEGIN namespace agz::d3d12::old_rg {
#define AGZ_D3D12_RENDERGRAPH_END   }

AGZ_D3D12_RENDERGRAPH_BEGIN

class Compiler;
class ExternalResource;
class Graph;
class InternalResource;
class PassContext;
class Resource;
class Runtime;

struct DescriptorSlot;

using ResourceView = misc::variant_t<
    std::monostate,
    D3D12_SHADER_RESOURCE_VIEW_DESC,
    D3D12_UNORDERED_ACCESS_VIEW_DESC,
    D3D12_RENDER_TARGET_VIEW_DESC,
    D3D12_DEPTH_STENCIL_VIEW_DESC>;

using PassCallback = std::function<void(PassContext &)>;

AGZ_D3D12_RENDERGRAPH_END
