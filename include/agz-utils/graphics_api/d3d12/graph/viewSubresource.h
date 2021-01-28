#pragma once

#include <agz-utils/graphics_api/d3d12/graph/common.h>

AGZ_D3D12_GRAPH_BEGIN

std::vector<UINT> viewToSubresources(
    const D3D12_RESOURCE_DESC &resource, const ResourceView &view);

AGZ_D3D12_GRAPH_END
