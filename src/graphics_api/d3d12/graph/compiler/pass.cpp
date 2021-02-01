#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/graph/compiler/pass.h>

AGZ_D3D12_GRAPH_BEGIN

bool Vertex::isAggregate() const
{
    return asAggregate() != nullptr;
}

Pass *Vertex::asPass()
{
    return nullptr;
}

const Pass *Vertex::asPass() const
{
    return nullptr;
}

PassAggregate *Vertex::asAggregate()
{
    return nullptr;
}

const PassAggregate *Vertex::asAggregate() const
{
    return nullptr;
}

Pass::Pass(std::string name, int index)
    : name_(std::move(name)), index_(index),
      thread_(0), queue_(0)
{
    
}

Pass *Pass::asPass()
{
    return this;
}

const Pass *Pass::asPass() const
{
    return this;
}

const std::string &Pass::getName() const
{
    return name_;
}

int Pass::getIndex() const
{
    return index_;
}

void Pass::setQueue(int queueIndex)
{
    queue_ = queueIndex;
}

void Pass::setThread(int threadIndex)
{
    thread_ = threadIndex;
}

void Pass::setCallback(PassCallback callback)
{
    callback_ = std::make_shared<PassCallback>(std::move(callback));
}

void Pass::addResourceState(
    const Resource       *resource,
    D3D12_RESOURCE_STATES state,
    UINT                  subrsc)
{
    if(states_.find(resource) != states_.end())
    {
        throw D3D12Exception(
            "repeated resource state declaretion of " +
            resource->getName() + "in pass " + name_);
    }
    states_.insert({ resource, { state, subrsc } });
}

DescriptorItem *Pass::addSRV(
    bool                                   cpu,
    bool                                   gpu,
    const Resource                        *resource,
    ShaderResourceType                     shaderResourceType,
    const D3D12_SHADER_RESOURCE_VIEW_DESC &desc)
{
    auto item = addDescriptor(cpu, gpu);
    item->setSRV(resource, shaderResourceType, desc);
    return item;
}

DescriptorItem *Pass::addUAV(
    bool                                    cpu,
    bool                                    gpu,
    const Resource                         *resource,
    const Resource                         *uavCounterResource,
    const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc)
{
    auto item = addDescriptor(cpu, gpu);
    item->setUAV(resource, uavCounterResource, desc);
    return item;
}

DescriptorItem *Pass::addRTV(
    const Resource                      *resource,
    const D3D12_RENDER_TARGET_VIEW_DESC &desc)
{
    auto item = addDescriptor(true, false);
    item->setRTV(resource, desc);
    return item;
}

DescriptorItem *Pass::addDSV(
    const Resource                      *resource,
    DepthStencilType                     depthStencilType,
    const D3D12_DEPTH_STENCIL_VIEW_DESC &desc)
{
    auto item = addDescriptor(true, false);
    item->setDSV(resource, depthStencilType, desc);
    return item;
}

DescriptorItem *Pass::addDescriptor(bool cpuVisible, bool gpuVisible)
{
    descriptors_.push_back(
        std::make_unique<DescriptorItem>(cpuVisible, gpuVisible));
    return descriptors_.back().get();
}

DescriptorTable *Pass::addDescriptorTable(bool cpuVisible, bool gpuVisible)
{
    descriptorTables_.push_back(
        std::make_unique<DescriptorTable>(cpuVisible, gpuVisible));
    return descriptorTables_.back().get();
}

PassAggregate::PassAggregate(std::string name)
    : name_(std::move(name)), entry_(nullptr), exit_(nullptr)
{
    
}

const std::string &PassAggregate::getName() const
{
    return name_;
}

void PassAggregate::setEntry(Vertex *entry)
{
    entry_ = entry;
}

void PassAggregate::setExit(Vertex *exit)
{
    exit_ = exit;
}

PassAggregate *PassAggregate::asAggregate()
{
    return this;
}

const PassAggregate *PassAggregate::asAggregate() const
{
    return this;
}

AGZ_D3D12_GRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
