#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/graph/compiler/descriptorDecleration.h>

AGZ_D3D12_GRAPH_BEGIN

DescriptorItem::DescriptorItem(bool cpu, bool gpu)
    : cpu_(cpu), gpu_(gpu)
{
    
}

void DescriptorItem::setSRV(
    const Resource                        *resource,
    ShaderResourceType                     type,
    const D3D12_SHADER_RESOURCE_VIEW_DESC &desc)
{
    resource_           = resource;
    view_               = desc;
    shaderResourceType_ = type;
}

void DescriptorItem::setUAV(
    const Resource                         *resource,
    const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc)
{
    resource_ = resource;
    view_     = desc;
}

void DescriptorItem::setRTV(
    const Resource                      *resource,
    const D3D12_RENDER_TARGET_VIEW_DESC &desc)
{
    resource_ = resource;
    view_     = desc;
}

void DescriptorItem::setDSV(
    const Resource                      *resource,
    DepthStencilType                     type,
    const D3D12_DEPTH_STENCIL_VIEW_DESC &desc)
{
    resource_         = resource;
    depthStencilType_ = type;
    view_             = desc;
}

const Resource *DescriptorItem::getResource() const
{
    return resource_;
}

bool DescriptorItem::operator<(const DescriptorItem &rhs) const
{
    auto L = std::tie(
        resource_, cpu_, gpu_,
        shaderResourceType_, depthStencilType_);
    auto R = std::tie(
        rhs.resource_, cpu_, gpu_,
        rhs.shaderResourceType_, rhs.depthStencilType_);
    return L < R || (L == R && view_ < rhs.view_);
}

DescriptorTable::DescriptorTable(bool cpu, bool gpu)
    : cpu_(cpu), gpu_(gpu)
{
    
}

void DescriptorTable::addSRV(
    const Resource                        *resource,
    ShaderResourceType                     type,
    const D3D12_SHADER_RESOURCE_VIEW_DESC &desc)
{
    records_.push_back({ resource, desc, type, {} });
}

void DescriptorTable::addUAV(
    const Resource                         *resource,
    const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc)
{
    records_.push_back({ resource, desc, {}, {} });
}

void DescriptorTable::addRTV(
    const Resource                      *resource,
    const D3D12_RENDER_TARGET_VIEW_DESC &desc)
{
    records_.push_back({ resource, desc, {}, {} });
}

void DescriptorTable::addDSV(
    const Resource                      *resource,
    DepthStencilType                     type,
    const D3D12_DEPTH_STENCIL_VIEW_DESC &desc)
{
    records_.push_back({ resource, desc, {}, type });
}

bool DescriptorTable::operator<(const DescriptorTable &rhs) const
{
    return std::tie(cpu_, gpu_, records_) <
           std::tie(rhs.cpu_, rhs.gpu_, rhs.records_);
}

bool DescriptorTable::Record::operator<(const Record &rhs) const
{
    const int LResourceIndex = resource->getIndex();
    const int RResourceIndex = rhs.resource->getIndex();

    auto L = std::tie(
        LResourceIndex,
        resource,
        shaderResourceType,
        depthStencilType);
    auto R = std::tie(
        RResourceIndex,
        rhs.resource,
        rhs.shaderResourceType,
        rhs.depthStencilType);

    return L < R || (L == R && view < rhs.view);
}

AGZ_D3D12_GRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
