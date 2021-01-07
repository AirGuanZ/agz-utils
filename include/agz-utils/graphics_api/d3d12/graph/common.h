#pragma once

#include <functional>

#include <agz-utils/graphics_api/d3d12/common.h>
#include <agz-utils/misc.h>

#define AGZ_D3D12_GRAPH_BEGIN AGZ_D3D12_BEGIN namespace rg {
#define AGZ_D3D12_GRAPH_END   } AGZ_D3D12_END

AGZ_D3D12_GRAPH_BEGIN

class DescriptorTable;
class ExternalResource;
class InternalResource;
class Resource;
class Pass;

class PassContext;

struct DescriptorSlot;
struct DescriptorRangeSlot;
class GraphRuntime;
class PassRuntime;

using PassCallback = std::function<void(PassContext &)>;
using RawGraphicsCommandList = ID3D12GraphicsCommandList4;

using ResourceView = misc::variant_t<
    std::monostate,
    D3D12_SHADER_RESOURCE_VIEW_DESC,
    D3D12_UNORDERED_ACCESS_VIEW_DESC,
    D3D12_RENDER_TARGET_VIEW_DESC,
    D3D12_DEPTH_STENCIL_VIEW_DESC>;

bool operator<(const ResourceView &lhs, const ResourceView &rhs) noexcept;

AGZ_D3D12_GRAPH_END
