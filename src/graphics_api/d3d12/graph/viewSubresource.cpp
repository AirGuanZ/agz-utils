#ifdef AGZ_ENABLE_D3D12

#include <d3dx12.h>

#include <agz-utils/graphics_api/d3d12/graph/viewSubresource.h>
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

    std::vector<UINT> getMipLevels(
        const D3D12_RESOURCE_DESC &rsc, UINT mostDetailed, UINT mipLevels)
    {
        const UINT end =
            mipLevels > 0 ? (mostDetailed + mipLevels)
                          : getTotalMipLevelCount(rsc);

        std::vector<UINT> ret;
        for(UINT m = mostDetailed; m < end; ++m)
            ret.push_back(m);

        return ret;
    }

    // SRV

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_BUFFER_SRV &srv)
    {
        return { 0 };
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX1D_SRV &srv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        std::vector<UINT> ret;
        for(UINT m : getMipLevels(rsc, srv.MostDetailedMip, srv.MipLevels))
        {
            ret.push_back(D3D12CalcSubresource(
                m, 0, 0, mipLevels, rsc.DepthOrArraySize));
        }

        return ret;
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX1D_ARRAY_SRV &srv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        std::vector<UINT> ret;
        for(UINT m : getMipLevels(rsc, srv.MostDetailedMip, srv.MipLevels))
        {
            for(UINT a = 0; a < srv.ArraySize; ++a)
            {
                ret.push_back(D3D12CalcSubresource(
                    m,
                    a + srv.FirstArraySlice, 0,
                    mipLevels, rsc.DepthOrArraySize));
            }
        }

        return ret;
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX2D_SRV &srv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        std::vector<UINT> ret;
        for(UINT m : getMipLevels(rsc, srv.MostDetailedMip, srv.MipLevels))
        {
            ret.push_back(D3D12CalcSubresource(
                m, 0, 0, mipLevels, rsc.DepthOrArraySize));
        }

        return ret;
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX2D_ARRAY_SRV &srv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        std::vector<UINT> ret;
        for(UINT m : getMipLevels(rsc, srv.MostDetailedMip, srv.MipLevels))
        {
            for(UINT a = 0; a < srv.ArraySize; ++a)
            {
                ret.push_back(D3D12CalcSubresource(
                    m, a + srv.FirstArraySlice, 0,
                    mipLevels, rsc.DepthOrArraySize));
            }
        }

        return ret;
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX3D_SRV &srv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        std::vector<UINT> ret;
        for(UINT m : getMipLevels(rsc, srv.MostDetailedMip, srv.MipLevels))
        {
            ret.push_back(D3D12CalcSubresource(
                m, 0, 0, mipLevels, 1));
        }

        return ret;
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEXCUBE_SRV &srv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        std::vector<UINT> ret;
        for(UINT m : getMipLevels(rsc, srv.MostDetailedMip, srv.MipLevels))
        {
            for(UINT a = 0; a < 6; ++a)
            {
                ret.push_back(D3D12CalcSubresource(
                    m, a, 0, mipLevels, rsc.DepthOrArraySize));
            }
        }

        return ret;
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEXCUBE_ARRAY_SRV &srv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        std::vector<UINT> ret;
        for(UINT m : getMipLevels(rsc, srv.MostDetailedMip, srv.MipLevels))
        {
            for(UINT c = 0; c < srv.NumCubes; ++c)
            {
                for(int a = 0; a < 6; ++a)
                {
                    ret.push_back(D3D12CalcSubresource(
                        m, srv.First2DArrayFace + 6 * c + a, 0,
                        mipLevels, rsc.DepthOrArraySize));
                }
            }
        }

        return ret;
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX2DMS_SRV &srv)
    {
        return { 0 };
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX2DMS_ARRAY_SRV &srv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        std::vector<UINT> ret;
        for(UINT a = 0; a < srv.ArraySize; ++a)
        {
            ret.push_back(D3D12CalcSubresource(
                0, a + srv.FirstArraySlice, 0,
                mipLevels, rsc.DepthOrArraySize));
        }

        return ret;
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC             &rsc,
        const D3D12_SHADER_RESOURCE_VIEW_DESC &srv)
    {
        switch(srv.ViewDimension)
        {
        case D3D12_SRV_DIMENSION_UNKNOWN:
            return {};
        case D3D12_SRV_DIMENSION_BUFFER:
            return viewToSubresources(rsc, srv.Buffer);
        case D3D12_SRV_DIMENSION_TEXTURE1D:
            return viewToSubresources(rsc, srv.Texture1D);
        case D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
            return viewToSubresources(rsc, srv.Texture1DArray);
        case D3D12_SRV_DIMENSION_TEXTURE2D:
            return viewToSubresources(rsc, srv.Texture2D);
        case D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
            return viewToSubresources(rsc, srv.Texture2DArray);
        case D3D12_SRV_DIMENSION_TEXTURE2DMS:
            return viewToSubresources(rsc, srv.Texture2DMS);
        case D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY:
            return viewToSubresources(rsc, srv.Texture2DMSArray);
        case D3D12_SRV_DIMENSION_TEXTURE3D:
            return viewToSubresources(rsc, srv.Texture3D);
        case D3D12_SRV_DIMENSION_TEXTURECUBE:
            return viewToSubresources(rsc, srv.TextureCube);
        case D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
            return viewToSubresources(rsc, srv.TextureCubeArray);
        case D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE:
            return {};
        }

        misc::unreachable();
    }

    // UAV

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_BUFFER_UAV &uav)
    {
        return { 0 };
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX1D_UAV &uav)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        return {
            D3D12CalcSubresource(
                uav.MipSlice, 0, 0, mipLevels, rsc.DepthOrArraySize)
        };
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX1D_ARRAY_UAV &uav)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        std::vector<UINT> ret;
        for(UINT a = 0; a < uav.ArraySize; ++a)
        {
            ret.push_back(D3D12CalcSubresource(
                uav.MipSlice, a + uav.FirstArraySlice, 0,
                mipLevels, rsc.DepthOrArraySize));
        }

        return ret;
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX2D_UAV &uav)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        return {
            D3D12CalcSubresource(
                uav.MipSlice, 0, 0, mipLevels, rsc.DepthOrArraySize)
        };
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX2D_ARRAY_UAV &uav)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        std::vector<UINT> ret;
        for(UINT a = 0; a < uav.ArraySize; ++a)
        {
            ret.push_back(D3D12CalcSubresource(
                uav.MipSlice, a + uav.FirstArraySlice, 0,
                mipLevels, rsc.DepthOrArraySize));
        }

        return ret;
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX3D_UAV &uav)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        return {
            D3D12CalcSubresource(
                uav.MipSlice, 0, 0, mipLevels, rsc.DepthOrArraySize)
        };
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC              &rsc,
        const D3D12_UNORDERED_ACCESS_VIEW_DESC &uav)
    {
        switch(uav.ViewDimension)
        {
        case D3D12_UAV_DIMENSION_UNKNOWN:
            return {};
        case D3D12_UAV_DIMENSION_BUFFER:
            return viewToSubresources(rsc, uav.Buffer);
        case D3D12_UAV_DIMENSION_TEXTURE1D:
            return viewToSubresources(rsc, uav.Texture1D);
        case D3D12_UAV_DIMENSION_TEXTURE1DARRAY:
            return viewToSubresources(rsc, uav.Texture1DArray);
        case D3D12_UAV_DIMENSION_TEXTURE2D:
            return viewToSubresources(rsc, uav.Texture2D);
        case D3D12_UAV_DIMENSION_TEXTURE2DARRAY:
            return viewToSubresources(rsc, uav.Texture2DArray);
        case D3D12_UAV_DIMENSION_TEXTURE3D:
            return viewToSubresources(rsc, uav.Texture3D);
        }

        misc::unreachable();
    }

    // RTV

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_BUFFER_RTV &rtv)
    {
        return { 0 };
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX1D_RTV &rtv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        return {
            D3D12CalcSubresource(
                rtv.MipSlice, 0, 0, mipLevels, rsc.DepthOrArraySize)
        };
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX1D_ARRAY_RTV &rtv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        std::vector<UINT> ret;
        for(UINT a = 0; a < rtv.ArraySize; ++a)
        {
            ret.push_back(D3D12CalcSubresource(
                rtv.MipSlice, a + rtv.FirstArraySlice, 0,
                mipLevels, rtv.ArraySize));
        }

        return ret;
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX2D_RTV &rtv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        return {
            D3D12CalcSubresource(
                rtv.MipSlice, 0, 0, mipLevels, rsc.DepthOrArraySize)
        };
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX2DMS_RTV &rtv)
    {
        return { 0 };
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX2D_ARRAY_RTV &rtv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        std::vector<UINT> ret;
        for(UINT a = 0; a < rtv.ArraySize; ++a)
        {
            ret.push_back(D3D12CalcSubresource(
                rtv.MipSlice, a + rtv.FirstArraySlice, 0,
                mipLevels, rsc.DepthOrArraySize));
        }

        return ret;
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX2DMS_ARRAY_RTV &rtv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        std::vector<UINT> ret;
        for(UINT a = 0; a < rtv.ArraySize; ++a)
        {
            ret.push_back(D3D12CalcSubresource(
                0, a + rtv.FirstArraySlice, 0,
                mipLevels, rsc.DepthOrArraySize));
        }

        return ret;
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX3D_RTV &rtv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        return {
            D3D12CalcSubresource(
                rtv.MipSlice, 0, 0, mipLevels, rsc.DepthOrArraySize)
        };
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC           &rsc,
        const D3D12_RENDER_TARGET_VIEW_DESC &rtv)
    {
        switch(rtv.ViewDimension)
        {
        case D3D12_RTV_DIMENSION_UNKNOWN:
            return {};
        case D3D12_RTV_DIMENSION_BUFFER:
            return viewToSubresources(rsc, rtv.Buffer);
        case D3D12_RTV_DIMENSION_TEXTURE1D:
            return viewToSubresources(rsc, rtv.Texture1D);
        case D3D12_RTV_DIMENSION_TEXTURE1DARRAY:
            return viewToSubresources(rsc, rtv.Texture1DArray);
        case D3D12_RTV_DIMENSION_TEXTURE2D:
            return viewToSubresources(rsc, rtv.Texture2D);
        case D3D12_RTV_DIMENSION_TEXTURE2DARRAY:
            return viewToSubresources(rsc, rtv.Texture2DArray);
        case D3D12_RTV_DIMENSION_TEXTURE2DMS:
            return viewToSubresources(rsc, rtv.Texture2DMS);
        case D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY:
            return viewToSubresources(rsc, rtv.Texture2DMSArray);
        case D3D12_RTV_DIMENSION_TEXTURE3D:
            return viewToSubresources(rsc, rtv.Texture3D);
        }

        misc::unreachable();
    }

    // DSV

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX1D_DSV &dsv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        return {
            D3D12CalcSubresource(
                dsv.MipSlice, 0, 0, mipLevels, rsc.DepthOrArraySize)
        };
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX1D_ARRAY_DSV &dsv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        std::vector<UINT> ret;
        for(UINT a = 0; a < dsv.ArraySize; ++a)
        {
            ret.push_back(D3D12CalcSubresource(
                dsv.MipSlice, a + dsv.FirstArraySlice, 0,
                mipLevels, dsv.ArraySize));
        }

        return ret;
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX2D_DSV &dsv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        return {
            D3D12CalcSubresource(
                dsv.MipSlice, 0, 0, mipLevels, rsc.DepthOrArraySize)
        };
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX2DMS_DSV &dsv)
    {
        return { 0 };
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX2D_ARRAY_DSV &dsv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        std::vector<UINT> ret;
        for(UINT a = 0; a < dsv.ArraySize; ++a)
        {
            ret.push_back(D3D12CalcSubresource(
                dsv.MipSlice, a + dsv.FirstArraySlice, 0,
                mipLevels, rsc.DepthOrArraySize));
        }

        return ret;
    }

    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC &rsc, const D3D12_TEX2DMS_ARRAY_DSV &dsv)
    {
        const UINT mipLevels = getTotalMipLevelCount(rsc);

        std::vector<UINT> ret;
        for(UINT a = 0; a < dsv.ArraySize; ++a)
        {
            ret.push_back(D3D12CalcSubresource(
                0, a + dsv.FirstArraySlice, 0,
                mipLevels, rsc.DepthOrArraySize));
        }

        return ret;
    }
    
    std::vector<UINT> viewToSubresources(
        const D3D12_RESOURCE_DESC           &rsc,
        const D3D12_DEPTH_STENCIL_VIEW_DESC &dsv)
    {
        switch(dsv.ViewDimension)
        {
        case D3D12_DSV_DIMENSION_UNKNOWN:
            return {};
        case D3D12_DSV_DIMENSION_TEXTURE1D:
            return viewToSubresources(rsc, dsv.Texture1D);
        case D3D12_DSV_DIMENSION_TEXTURE1DARRAY:
            return viewToSubresources(rsc, dsv.Texture1DArray);
        case D3D12_DSV_DIMENSION_TEXTURE2D:
            return viewToSubresources(rsc, dsv.Texture2D);
        case D3D12_DSV_DIMENSION_TEXTURE2DARRAY:
            return viewToSubresources(rsc, dsv.Texture2DArray);
        case D3D12_DSV_DIMENSION_TEXTURE2DMS:
            return viewToSubresources(rsc, dsv.Texture2DMS);
        case D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY:
            return viewToSubresources(rsc, dsv.Texture2DMSArray);
        }

        misc::unreachable();
    }

} // namespace anonymous

std::vector<UINT> viewToSubresources(
    const D3D12_RESOURCE_DESC &rsc, const ResourceView &view)
{
    return match_variant(
        view,
        [&](const D3D12_SHADER_RESOURCE_VIEW_DESC &srv)
    {
        return viewToSubresources(rsc, srv);
    },
        [&](const D3D12_UNORDERED_ACCESS_VIEW_DESC &uav)
    {
        return viewToSubresources(rsc, uav);
    },
        [&](const D3D12_RENDER_TARGET_VIEW_DESC &rtv)
    {
        return viewToSubresources(rsc, rtv);
    },
        [&](const D3D12_DEPTH_STENCIL_VIEW_DESC &dsv)
    {
        return viewToSubresources(rsc, dsv);
    },
        [&](const std::monostate &)
    {
        return std::vector<UINT>{};
    });
}

AGZ_D3D12_GRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
