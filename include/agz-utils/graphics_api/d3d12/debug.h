#pragma once

#include <agz-utils/graphics_api/d3d12/common.h>
#include <agz-utils/system.h>

AGZ_D3D12_BEGIN

void enableDebugLayer(bool gpuValidation);

inline void enableDebugLayerInDebugMode(bool gpuValidation)
{
    AGZ_WHEN_DEBUG({ enableDebugLayer(gpuValidation); });
}

AGZ_D3D12_END
