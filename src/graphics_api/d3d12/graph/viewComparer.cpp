#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/graph/viewComparer.h>

AGZ_D3D12_GRAPH_BEGIN

namespace
{

#define FIELDWISE_COMPARE(TYPE, ...)                                            \
    auto constructTie(const TYPE &V) noexcept                                   \
    {                                                                           \
        return std::tie(__VA_ARGS__);                                           \
    }                                                                           \
    bool compare(const TYPE &L, const TYPE &R) noexcept                         \
    {                                                                           \
        return constructTie(L) < constructTie(R);                               \
    }

#define FIELD_COMPARE(FIELD)                                                    \
    if(L.FIELD < R.FIELD)                                                       \
        return true;                                                            \
    if(L.FIELD > R.FIELD)                                                       \
        return false;

FIELDWISE_COMPARE(D3D12_BUFFER_SRV, V.FirstElement, V.NumElements, V.StructureByteStride, V.Flags)
FIELDWISE_COMPARE(D3D12_TEX1D_SRV,  V.MostDetailedMip, V.MipLevels, V.ResourceMinLODClamp)
FIELDWISE_COMPARE(D3D12_TEX1D_ARRAY_SRV, V.MostDetailedMip, V.MipLevels, V.FirstArraySlice, V.ArraySize, V.ResourceMinLODClamp)
FIELDWISE_COMPARE(D3D12_TEX2D_SRV, V.MostDetailedMip, V.MipLevels, V.PlaneSlice, V.ResourceMinLODClamp)
FIELDWISE_COMPARE(D3D12_TEX2D_ARRAY_SRV, V.MostDetailedMip, V.MipLevels, V.FirstArraySlice, V.ArraySize, V.PlaneSlice, V.ResourceMinLODClamp)
FIELDWISE_COMPARE(D3D12_TEX3D_SRV, V.MostDetailedMip, V.MipLevels, V.ResourceMinLODClamp)
FIELDWISE_COMPARE(D3D12_TEXCUBE_SRV, V.MostDetailedMip, V.MipLevels, V.ResourceMinLODClamp)
FIELDWISE_COMPARE(D3D12_TEXCUBE_ARRAY_SRV, V.MostDetailedMip, V.MipLevels, V.First2DArrayFace, V.NumCubes, V.ResourceMinLODClamp)
FIELDWISE_COMPARE(D3D12_TEX2DMS_SRV, V.UnusedField_NothingToDefine)
FIELDWISE_COMPARE(D3D12_TEX2DMS_ARRAY_SRV, V.FirstArraySlice, V.ArraySize)

bool compare(
    const D3D12_SHADER_RESOURCE_VIEW_DESC &L,
    const D3D12_SHADER_RESOURCE_VIEW_DESC &R) noexcept
{
    FIELD_COMPARE(Format)
    FIELD_COMPARE(ViewDimension)
    FIELD_COMPARE(Shader4ComponentMapping)

    switch(L.ViewDimension)
    {
    case D3D12_SRV_DIMENSION_UNKNOWN:
        return false;
    case D3D12_SRV_DIMENSION_BUFFER:
        return compare(L.Buffer, R.Buffer);
    case D3D12_SRV_DIMENSION_TEXTURE1D:
        return compare(L.Texture1D, R.Texture1D);
    case D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
        return compare(L.Texture1DArray, R.Texture1DArray);
    case D3D12_SRV_DIMENSION_TEXTURE2D:
        return compare(L.Texture2D, R.Texture2D);
    case D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
        return compare(L.Texture2DArray, R.Texture2DArray);
    case D3D12_SRV_DIMENSION_TEXTURE2DMS:
        return compare(L.Texture2DMS, R.Texture2DMS);
    case D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY:
        return compare(L.Texture2DMSArray, R.Texture2DMSArray);
    case D3D12_SRV_DIMENSION_TEXTURE3D:
        return compare(L.Texture3D, R.Texture3D);
    case D3D12_SRV_DIMENSION_TEXTURECUBE:
        return compare(L.TextureCube, R.TextureCube);
    case D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
        return compare(L.TextureCubeArray, R.TextureCubeArray);
    default:
        misc::unreachable();
    }
}

FIELDWISE_COMPARE(D3D12_BUFFER_UAV, V.FirstElement, V.NumElements, V.StructureByteStride, V.CounterOffsetInBytes, V.Flags)
FIELDWISE_COMPARE(D3D12_TEX1D_UAV, V.MipSlice)
FIELDWISE_COMPARE(D3D12_TEX1D_ARRAY_UAV, V.MipSlice, V.FirstArraySlice)
FIELDWISE_COMPARE(D3D12_TEX2D_UAV, V.MipSlice, V.PlaneSlice)
FIELDWISE_COMPARE(D3D12_TEX2D_ARRAY_UAV, V.MipSlice, V.FirstArraySlice, V.ArraySize, V.PlaneSlice)
FIELDWISE_COMPARE(D3D12_TEX3D_UAV, V.MipSlice, V.FirstWSlice, V.WSize)

bool compare(
    const D3D12_UNORDERED_ACCESS_VIEW_DESC &L,
    const D3D12_UNORDERED_ACCESS_VIEW_DESC &R) noexcept
{
    FIELD_COMPARE(Format)
    FIELD_COMPARE(ViewDimension)

    switch(L.ViewDimension)
    {
    case D3D12_UAV_DIMENSION_UNKNOWN:
        return false;
    case D3D12_UAV_DIMENSION_BUFFER:
        return compare(L.Buffer, R.Buffer);
    case D3D12_UAV_DIMENSION_TEXTURE1D:
        return compare(L.Texture1D, R.Texture1D);
    case D3D12_UAV_DIMENSION_TEXTURE1DARRAY:
        return compare(L.Texture1DArray, R.Texture1DArray);
    case D3D12_UAV_DIMENSION_TEXTURE2D:
        return compare(L.Texture2D, R.Texture2D);
    case D3D12_UAV_DIMENSION_TEXTURE2DARRAY:
        return compare(L.Texture2DArray, R.Texture2DArray);
    case D3D12_UAV_DIMENSION_TEXTURE3D:
        return compare(L.Texture3D, R.Texture3D);
    }

    misc::unreachable();
}

FIELDWISE_COMPARE(D3D12_BUFFER_RTV, V.FirstElement, V.NumElements)
FIELDWISE_COMPARE(D3D12_TEX1D_RTV, V.MipSlice)
FIELDWISE_COMPARE(D3D12_TEX1D_ARRAY_RTV, V.MipSlice, V.FirstArraySlice, V.ArraySize)
FIELDWISE_COMPARE(D3D12_TEX2D_RTV, V.MipSlice, V.PlaneSlice)
FIELDWISE_COMPARE(D3D12_TEX2DMS_RTV, V.UnusedField_NothingToDefine)
FIELDWISE_COMPARE(D3D12_TEX2D_ARRAY_RTV, V.MipSlice, V.FirstArraySlice, V.ArraySize, V.PlaneSlice)
FIELDWISE_COMPARE(D3D12_TEX2DMS_ARRAY_RTV, V.FirstArraySlice, V.ArraySize)
FIELDWISE_COMPARE(D3D12_TEX3D_RTV, V.MipSlice, V.FirstWSlice, V.WSize)

bool compare(
    const D3D12_RENDER_TARGET_VIEW_DESC &L,
    const D3D12_RENDER_TARGET_VIEW_DESC &R) noexcept
{
    FIELD_COMPARE(Format)
    FIELD_COMPARE(ViewDimension)

    switch(L.ViewDimension)
    {
    case D3D12_RTV_DIMENSION_UNKNOWN:
        return false;
    case D3D12_RTV_DIMENSION_BUFFER:
        return compare(L.Buffer, R.Buffer);
    case D3D12_RTV_DIMENSION_TEXTURE1D:
        return compare(L.Texture1D, R.Texture1D);
    case D3D12_RTV_DIMENSION_TEXTURE1DARRAY:
        return compare(L.Texture1DArray, R.Texture1DArray);
    case D3D12_RTV_DIMENSION_TEXTURE2D:
        return compare(L.Texture2D, R.Texture2D);
    case D3D12_RTV_DIMENSION_TEXTURE2DARRAY:
        return compare(L.Texture2DArray, R.Texture2DArray);
    case D3D12_RTV_DIMENSION_TEXTURE2DMS:
        return compare(L.Texture2DMS, R.Texture2DMS);
    case D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY:
        return compare(L.Texture2DMSArray, R.Texture2DMSArray);
    case D3D12_RTV_DIMENSION_TEXTURE3D:
        return compare(L.Texture3D, R.Texture3D);
    }

    misc::unreachable();
}

FIELDWISE_COMPARE(D3D12_TEX1D_DSV, V.MipSlice)
FIELDWISE_COMPARE(D3D12_TEX1D_ARRAY_DSV, V.MipSlice, V.FirstArraySlice, V.ArraySize)
FIELDWISE_COMPARE(D3D12_TEX2D_DSV, V.MipSlice)
FIELDWISE_COMPARE(D3D12_TEX2D_ARRAY_DSV, V.MipSlice, V.FirstArraySlice, V.ArraySize)
FIELDWISE_COMPARE(D3D12_TEX2DMS_DSV, V.UnusedField_NothingToDefine)
FIELDWISE_COMPARE(D3D12_TEX2DMS_ARRAY_DSV, V.FirstArraySlice, V.ArraySize)

bool compare(
    const D3D12_DEPTH_STENCIL_VIEW_DESC &L,
    const D3D12_DEPTH_STENCIL_VIEW_DESC &R) noexcept
{
    FIELD_COMPARE(Format)
    FIELD_COMPARE(ViewDimension)
    FIELD_COMPARE(Flags)

    switch(L.ViewDimension)
    {
    case D3D12_DSV_DIMENSION_UNKNOWN:
        return false;
    case D3D12_DSV_DIMENSION_TEXTURE1D:
        return compare(L.Texture1D, R.Texture1D);
    case D3D12_DSV_DIMENSION_TEXTURE1DARRAY:
        return compare(L.Texture1DArray, R.Texture1DArray);
    case D3D12_DSV_DIMENSION_TEXTURE2D:
        return compare(L.Texture2D, R.Texture2D);
    case D3D12_DSV_DIMENSION_TEXTURE2DARRAY:
        return compare(L.Texture2DArray, R.Texture2DArray);
    case D3D12_DSV_DIMENSION_TEXTURE2DMS:
        return compare(L.Texture2DMS, R.Texture2DMS);
    case D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY:
        return compare(L.Texture2DMSArray, R.Texture2DMSArray);
    }

    misc::unreachable();
}

bool compareResourceView(const ResourceView &L, const ResourceView &R) noexcept
{
    const size_t LIdx = L.index();
    const size_t RIdx = R.index();
    if(LIdx < RIdx)
        return true;
    if(LIdx > RIdx)
        return false;

    if(L.is<std::monostate>())
        return false;

    if(L.is<D3D12_SHADER_RESOURCE_VIEW_DESC>())
    {
        return compare(
            L.as<D3D12_SHADER_RESOURCE_VIEW_DESC>(),
            R.as<D3D12_SHADER_RESOURCE_VIEW_DESC>());
    }

    if(L.is<D3D12_UNORDERED_ACCESS_VIEW_DESC>())
    {
        return compare(
            L.as<D3D12_UNORDERED_ACCESS_VIEW_DESC>(),
            R.as<D3D12_UNORDERED_ACCESS_VIEW_DESC>());
    }

    if(L.is<D3D12_RENDER_TARGET_VIEW_DESC>())
    {
        return compare(
            L.as<D3D12_RENDER_TARGET_VIEW_DESC>(),
            R.as<D3D12_RENDER_TARGET_VIEW_DESC>());
    }

    return compare(
        L.as<D3D12_DEPTH_STENCIL_VIEW_DESC>(),
        R.as<D3D12_DEPTH_STENCIL_VIEW_DESC>());
}

} // namespace anonymous

bool ResourceViewComparer::operator()(
    const ResourceView &L, const ResourceView &R) const noexcept
{
    return compareResourceView(L, R);
}

bool operator<(const ResourceView &lhs, const ResourceView &rhs) noexcept
{
    return ResourceViewComparer()(lhs, rhs);
}

AGZ_D3D12_GRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
