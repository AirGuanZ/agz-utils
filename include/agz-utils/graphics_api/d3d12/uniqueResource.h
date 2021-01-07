#pragma once

#include <D3D12MemAlloc.h>

#include <agz-utils/graphics_api/d3d12/common.h>
#include <agz-utils/misc.h>

AGZ_D3D12_BEGIN

namespace detail
{

    struct Resource
    {
        ComPtr<ID3D12Resource> resource;
        D3D12MA::Allocation   *allocation = nullptr;
    };

    struct ResourceDeleter
    {
        void operator()(Resource &rsc) const
        {
            rsc.resource.Reset();
            rsc.allocation->Release();
        }
    };

} // namespace detail

using UniqueResource = misc::unique_resource_t<
    detail::Resource, detail::ResourceDeleter>;

using SharedResource = std::shared_ptr<detail::Resource>;

AGZ_D3D12_END
