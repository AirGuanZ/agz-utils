#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/graph/compiler/resource.h>

AGZ_D3D12_GRAPH_BEGIN

Resource::Resource(std::string name, int index)
    : name_(std::move(name)), index_(index), desc_{}
{
    
}

const std::string &Resource::getName() const
{
    return name_;
}

int Resource::getIndex() const
{
    return index_;
}

bool Resource::isInternal() const
{
    return asInternal() != nullptr;
}

bool Resource::isExternal() const
{
    return asExternal() != nullptr;
}

InternalResource *Resource::asInternal()
{
    return nullptr;
}

const InternalResource *Resource::asInternal() const
{
    return nullptr;
}

ExternalResource *Resource::asExternal()
{
    return nullptr;
}

const ExternalResource *Resource::asExternal() const
{
    return nullptr;
}

void Resource::setPerFrame(bool isPerFrame)
{
    isPerFrame_ = isPerFrame;
}

bool Resource::isPerFrame() const
{
    return isPerFrame_;
}

void Resource::setDescription(const D3D12_RESOURCE_DESC &desc)
{
    desc_ = desc;
}

const D3D12_RESOURCE_DESC &Resource::getDescription() const
{
    return desc_;
}

D3D12_VIEWPORT Resource::getDefaultViewport(
    float minDepth, float maxDepth) const
{
    D3D12_VIEWPORT ret;
    ret.TopLeftX = 0;
    ret.TopLeftY = 0;
    ret.Width    = static_cast<float>(desc_.Width);
    ret.Height   = static_cast<float>(desc_.Height);
    ret.MinDepth = minDepth;
    ret.MaxDepth = maxDepth;
    return ret;
}

D3D12_RECT Resource::getDefaultScissor() const
{
    D3D12_RECT ret;
    ret.left   = 0;
    ret.top    = 0;
    ret.right  = static_cast<LONG>(desc_.Width);
    ret.bottom = static_cast<LONG>(desc_.Height);
    return ret;
}

InternalResource::InternalResource(std::string name, int index)
    : Resource(std::move(name), index),
      heapType_(D3D12_HEAP_TYPE_DEFAULT),
      clear_(false), clearValue_{},
      initialState_(D3D12_RESOURCE_STATE_COMMON)
{
    
}

void InternalResource::setClearValue(const D3D12_CLEAR_VALUE &clearValue)
{
    clear_      = true;
    clearValue_ = clearValue;
}

void InternalResource::setInitialState(D3D12_RESOURCE_STATES state)
{
    initialState_ = state;
}

void InternalResource::setHeapType(D3D12_HEAP_TYPE heapType)
{
    heapType_ = heapType;
}

InternalResource *InternalResource::asInternal()
{
    return this;
}

const InternalResource *InternalResource::asInternal() const
{
    return this;
}

ExternalResource::ExternalResource(std::string name, int index)
    : Resource(std::move(name), index),
      initialState_(D3D12_RESOURCE_STATE_COMMON),
      finalState_(D3D12_RESOURCE_STATE_COMMON)
{
    
}

void ExternalResource::setInitialState(D3D12_RESOURCE_STATES state)
{
    initialState_ = state;
}

void ExternalResource::setFinalState(D3D12_RESOURCE_STATES state)
{
    finalState_ = state;
}

ExternalResource *ExternalResource::asExternal()
{
    return this;
}

const ExternalResource *ExternalResource::asExternal() const
{
    return this;
}

AGZ_D3D12_GRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
