#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/graph/compiler/resourceStatesTracker.h>
#include <agz-utils/graphics_api/d3d12/graph/compiler/viewSubresource.h>
#include <agz-utils/graphics_api/d3d12/mipmapGenerator.h>

AGZ_D3D12_GRAPH_BEGIN

namespace
{
    UINT getTotalMipLevelCount(const D3D12_RESOURCE_DESC &rsc)
    {
        if(rsc.MipLevels > 0)
            return rsc.MipLevels;

        if(rsc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
        {
            return (std::max)(
                MipmapGenerator::computeMipmapChainLength(
                    static_cast<UINT>(rsc.Width),
                    static_cast<UINT>(rsc.Height)),
                MipmapGenerator::computeMipmapChainLength(
                    static_cast<UINT>(rsc.DepthOrArraySize), 1));
        }

        return MipmapGenerator::computeMipmapChainLength(
            static_cast<UINT>(rsc.Width),
            static_cast<UINT>(rsc.Height));
    }

    UINT getSubresourceCount(const D3D12_RESOURCE_DESC &desc)
    {
        const UINT arraySize =
            desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D ?
            1 : desc.DepthOrArraySize;

        const UINT mipSliceCount = getTotalMipLevelCount(desc);

        return arraySize * mipSliceCount;
    }

    D3D12_RESOURCE_STATES shaderResourceTypeToState(ShaderResourceType type)
    {
        switch(type)
        {
        case ShaderResourceType::PixelOnly:
            return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        case ShaderResourceType::NonPixelOnly:
            return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        case ShaderResourceType::PixelAndNonPixel:
            return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
                D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        }
        misc::unreachable();
    }

    D3D12_RESOURCE_STATES depthStencilTypeToState(DepthStencilType type)
    {
        switch(type)
        {
        case DepthStencilType::ReadOnly:
            return D3D12_RESOURCE_STATE_DEPTH_READ;
        case DepthStencilType::ReadAndWrite:
            return D3D12_RESOURCE_STATE_DEPTH_WRITE;
        }
        misc::unreachable();
    }

} // namespace anonymous

ResourceStatesTracker::ResourceStatesTracker(const Resource *resource)
    : resource_(resource),
      subrscCount_(getSubresourceCount(resource->getDescription()))
{

}

void ResourceStatesTracker::addResourceState(
    int pass, UINT subresource, D3D12_RESOURCE_STATES state)
{
    if(subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
    {
        for(UINT s = 0; s < subrscCount_; ++s)
            addResourceStateWithDetailedSubresource(pass, s, state);
    }
    else
        addResourceStateWithDetailedSubresource(pass, subresource, state);
}

void ResourceStatesTracker::addDescriptor(
    int pass, const DescriptorInfo &descInfo)
{
    const auto subrscs = viewToSubresources(
        resource_->getDescription(), descInfo.view);

    for(UINT subrsc : subrscs)
    {
        match_variant(
            descInfo.view,
            [&](const D3D12_SHADER_RESOURCE_VIEW_DESC &)
        {
            addResourceStateWithDetailedSubresource(
                pass, subrsc,
                shaderResourceTypeToState(descInfo.shaderResourceType));
        },
            [&](const D3D12_UNORDERED_ACCESS_VIEW_DESC &)
        {
            addResourceStateWithDetailedSubresource(
                pass, subrsc,
                D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        },
            [&](const D3D12_RENDER_TARGET_VIEW_DESC &)
        {
            addResourceStateWithDetailedSubresource(
                pass, subrsc,
                D3D12_RESOURCE_STATE_RENDER_TARGET);
        },
            [&](const D3D12_DEPTH_STENCIL_VIEW_DESC &)
        {
            addResourceStateWithDetailedSubresource(
                pass, subrsc,
                depthStencilTypeToState(descInfo.depthStencilType));
        },
            [&](const std::monostate &)
        {
            misc::unreachable();
        });
    }
}

std::vector<ResourceStatesTracker::Usage>
    ResourceStatesTracker::getUsages() const
{
    std::vector<Usage> usages;

    for(auto &p : states_)
    {
        if(usages.empty() ||
           usages.back().pass != p.first.pass)
        {
            usages.push_back({ p.first.pass, {} });
        }

        usages.back().state.insert({ p.first.subrsc, p.second });
    }

    return usages;
}

bool ResourceStatesTracker::Key::operator<(const Key &rhs) const
{
    return std::tie(pass, subrsc) < std::tie(rhs.pass, rhs.subrsc);
}

void ResourceStatesTracker::addResourceStateWithDetailedSubresource(
    int pass, UINT subresource, D3D12_RESOURCE_STATES state)
{
    assert(0 <= subresource && subresource < subrscCount_);

    const auto it = states_.find({ pass, subresource });
    if(it != states_.end())
        it->second |= state;
    else
        states_.insert({ Key{ pass, subresource }, state });
}

AGZ_D3D12_GRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
