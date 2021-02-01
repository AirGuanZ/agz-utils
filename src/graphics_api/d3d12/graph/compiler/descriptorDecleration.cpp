#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/graph/compiler/descriptorDecleration.h>

AGZ_D3D12_GRAPH_BEGIN

bool DescriptorInfo::operator<(const DescriptorInfo &rhs) const
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

DescriptorItem::DescriptorItem(bool cpu, bool gpu)
    : cpu_(cpu), gpu_(gpu)
{
    
}

void DescriptorItem::setSRV(
    const Resource                        *resource,
    ShaderResourceType                     type,
    const D3D12_SHADER_RESOURCE_VIEW_DESC &desc)
{
    info_.resource           = resource;
    info_.view               = desc;
    info_.shaderResourceType = type;
}

void DescriptorItem::setUAV(
    const Resource                         *resource,
    const Resource                         *uavCounterResource,
    const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc)
{
    info_.resource           = resource;
    info_.view               = desc;
    info_.uavCounterResource = uavCounterResource;
}

void DescriptorItem::setRTV(
    const Resource                      *resource,
    const D3D12_RENDER_TARGET_VIEW_DESC &desc)
{
    info_.resource = resource;
    info_.view     = desc;
}

void DescriptorItem::setDSV(
    const Resource                      *resource,
    DepthStencilType                     type,
    const D3D12_DEPTH_STENCIL_VIEW_DESC &desc)
{
    info_.resource         = resource;
    info_.depthStencilType = type;
    info_.view             = desc;
}

const Resource *DescriptorItem::getResource() const
{
    return info_.resource;
}

bool DescriptorItem::operator<(const DescriptorItem &rhs) const
{
    return std::tie(cpu_, gpu_, info_) < std::tie(rhs.cpu_, rhs.gpu_, rhs.info_);
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
        const Resource                         *uavCounterResource,
    const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc)
{
    records_.push_back({ resource, desc, {}, {}, uavCounterResource });
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

AGZ_D3D12_GRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
