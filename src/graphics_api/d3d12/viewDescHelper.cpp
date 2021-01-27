#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/viewDescHelper.h>

AGZ_D3D12_BEGIN

namespace
{

    DXGI_FORMAT rscToDSVFormat(DXGI_FORMAT rsc)
    {
        switch(rsc)
        {
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
            return DXGI_FORMAT_D32_FLOAT;
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
        default:
            throw D3D12Exception(
                "unsupported resource format for creating depth stencil view");
        }
    }

    DXGI_FORMAT rscToSRVFormatForDepthBuffer(DXGI_FORMAT rsc)
    {
        switch(rsc)
        {
        case DXGI_FORMAT_R32_TYPELESS:
            return DXGI_FORMAT_R32_FLOAT;
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
            return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        default:
            throw D3D12Exception(
                "unsupported depth format for creating shader resource view");
        }
    }

} // namespace anonymous

D3D12_DEPTH_STENCIL_VIEW_DESC getDefaultDSVDesc(
    const D3D12_RESOURCE_DESC &rscDesc,
    bool                       readOnlyDepth,
    bool                       readOnlyStencil)
{
    D3D12_DEPTH_STENCIL_VIEW_DESC DSV;

    DSV.Format = rscToDSVFormat(rscDesc.Format);

    DSV.Flags = D3D12_DSV_FLAG_NONE;
    if(readOnlyDepth)
        DSV.Flags |= D3D12_DSV_FLAG_READ_ONLY_DEPTH;
    if(readOnlyStencil)
        DSV.Flags |= D3D12_DSV_FLAG_READ_ONLY_STENCIL;

    if(rscDesc.SampleDesc.Count > 1)
    {
        DSV.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
        DSV.Texture2DMS.UnusedField_NothingToDefine = 0;
    }
    else
    {
        DSV.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        DSV.Texture2D.MipSlice = 0;
    }

    return DSV;
}

D3D12_SHADER_RESOURCE_VIEW_DESC getDefaultSRVDescForDepthBuffer(
    const D3D12_RESOURCE_DESC &rscDesc)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC SRV;
    SRV.Format = rscToSRVFormatForDepthBuffer(rscDesc.Format);
    SRV.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    if(rscDesc.SampleDesc.Count > 1)
    {
        SRV.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
        SRV.Texture2DMS.UnusedField_NothingToDefine = 0;
    }
    else
    {
        SRV.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        SRV.Texture2D.PlaneSlice          = 0;
        SRV.Texture2D.MipLevels           = 1;
        SRV.Texture2D.MostDetailedMip     = 0;
        SRV.Texture2D.ResourceMinLODClamp = 0;
    }

    return SRV;
}

D3D12_RENDER_TARGET_VIEW_DESC getDefaultRTVDesc(
    const D3D12_RESOURCE_DESC &rscDesc)
{
    D3D12_RENDER_TARGET_VIEW_DESC RTV;

    RTV.Format = DXGI_FORMAT_UNKNOWN;

    if(rscDesc.SampleDesc.Count > 1)
    {
        RTV.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
        RTV.Texture2DMS.UnusedField_NothingToDefine = 0;
    }
    else
    {
        RTV.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        RTV.Texture2D.PlaneSlice = 0;
        RTV.Texture2D.MipSlice   = 0;
    }

    return RTV;
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
