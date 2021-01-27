#pragma once

#include <agz-utils/graphics_api/d3d12/common.h>

AGZ_D3D12_BEGIN

D3D12_DEPTH_STENCIL_VIEW_DESC getDefaultDSVDesc(
    const D3D12_RESOURCE_DESC &rscDesc,
    bool                       readOnlyDepth   = false,
    bool                       readOnlyStencil = false);

D3D12_SHADER_RESOURCE_VIEW_DESC getDefaultSRVDescForDepthBuffer(
    const D3D12_RESOURCE_DESC &rscDesc);

D3D12_RENDER_TARGET_VIEW_DESC getDefaultRTVDesc(
    const D3D12_RESOURCE_DESC &rscDesc);

AGZ_D3D12_END
