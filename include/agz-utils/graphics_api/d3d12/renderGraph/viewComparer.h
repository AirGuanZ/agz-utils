#pragma once

#include <agz-utils/graphics_api/d3d12/renderGraph/common.h>

AGZ_D3D12_RENDERGRAPH_BEGIN

class ResourceViewComparer
{
public:

    bool operator()(const ResourceView &L, const ResourceView &R) const noexcept;
};

AGZ_D3D12_RENDERGRAPH_END
