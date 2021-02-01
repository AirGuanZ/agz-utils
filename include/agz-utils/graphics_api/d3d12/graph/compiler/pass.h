#pragma once

#include <map>
#include <set>

#include <agz-utils/graphics_api/d3d12/graph/compiler/descriptorDecleration.h>

AGZ_D3D12_GRAPH_BEGIN

class Pass;
class PassAggregate;

class Vertex : public misc::uncopyable_t
{
public:

    virtual ~Vertex() = default;

    bool isAggregate() const;

    virtual       Pass *asPass();
    virtual const Pass *asPass() const;

    virtual       PassAggregate *asAggregate();
    virtual const PassAggregate *asAggregate() const;
};

class Pass : public Vertex
{
public:

    Pass(std::string name, int index);

    Pass *asPass() override;

    const Pass *asPass() const override;

    const std::string &getName() const;

    int getIndex() const;

    void setQueue(int queueIndex);

    void setThread(int threadIndex);

    void setCallback(PassCallback callback);

    template<typename A, typename B>
    void setCallback(A pThis, B memFunc)
    {
        this->setCallback(
            [=](PassContext &ctx) { (pThis->*memFunc)(ctx); });
    }

    void addResourceState(
        const Resource       *resource,
        D3D12_RESOURCE_STATES state,
        UINT                  subrsc = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

    DescriptorItem *addSRV(
        bool                                   cpu,
        bool                                   gpu,
        const Resource                        *resource,
        ShaderResourceType                     shaderResourceType,
        const D3D12_SHADER_RESOURCE_VIEW_DESC &desc);

    DescriptorItem *addUAV(
        bool                                   cpu,
        bool                                   gpu,
        const Resource                         *resource,
        const Resource                         *uavCounterResource,
        const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc);

    DescriptorItem *addRTV(
        const Resource                      *resource,
        const D3D12_RENDER_TARGET_VIEW_DESC &desc);

    DescriptorItem *addDSV(
        const Resource                      *resource,
        DepthStencilType                     depthStencilType,
        const D3D12_DEPTH_STENCIL_VIEW_DESC &desc);

    DescriptorItem *addDescriptor(bool cpuVisible, bool gpuVisible);

    DescriptorTable *addDescriptorTable(bool cpuVisible, bool gpuVisible);

private:

    friend class Compiler;

    struct ResourceStateRecord
    {
        D3D12_RESOURCE_STATES state       = D3D12_RESOURCE_STATE_COMMON;
        UINT                  subresource = 0;
    };

    std::string name_;
    int         index_;

    int thread_;
    int queue_;

    std::shared_ptr<PassCallback> callback_;

    std::set<Pass *> in_;
    std::set<Pass *> out_;

    std::set<Pass *> inFromLastFrame_;
    std::set<Pass *> outToNextFrame_;

    std::map<const Resource*, ResourceStateRecord> states_;
    std::vector<std::unique_ptr<DescriptorItem>>   descriptors_;
    std::vector<std::unique_ptr<DescriptorTable>>  descriptorTables_;
};

class PassAggregate : public Vertex
{
public:

    explicit PassAggregate(std::string name);

    const std::string &getName() const;

    void setEntry(Vertex *entry);

    void setExit(Vertex *exit);

    PassAggregate *asAggregate() override;

    const PassAggregate *asAggregate() const override;

private:

    friend class Compiler;

    std::string name_;

    Vertex *entry_;
    Vertex *exit_;
};

AGZ_D3D12_GRAPH_END
