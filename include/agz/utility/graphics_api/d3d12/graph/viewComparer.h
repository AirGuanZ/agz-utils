#pragma once

#include <agz/utility/graphics_api/d3d12/graph/common.h>

AGZ_D3D12_GRAPH_BEGIN

class ResourceViewComparer
{
public:

    bool operator()(const ResourceView &L, const ResourceView &R) const noexcept;
};

AGZ_D3D12_GRAPH_END
