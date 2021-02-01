#pragma once

#include <agz-utils/graphics_api/d3d12/graph/common.h>

AGZ_D3D12_GRAPH_BEGIN

class Resource : public misc::uncopyable_t
{
public:

    Resource(std::string name, int index);

    virtual ~Resource() = default;

    const std::string &getName() const;

    int getIndex() const;

    bool isInternal() const;

    bool isExternal() const;

    virtual InternalResource *asInternal();

    virtual ExternalResource *asExternal();

    virtual const InternalResource *asInternal() const;

    virtual const ExternalResource *asExternal() const;

    void setDescription(const D3D12_RESOURCE_DESC &desc);

    const D3D12_RESOURCE_DESC &getDescription() const;

    D3D12_VIEWPORT getDefaultViewport(
        float minDepth = 0, float maxDepth = 1) const;

    D3D12_RECT getDefaultScissor() const;

private:

    std::string name_;
    int         index_;

    D3D12_RESOURCE_DESC desc_;
};

class InternalResource : public Resource
{
public:

    InternalResource(std::string name, int index);

    void setClearValue(const D3D12_CLEAR_VALUE &clearValue);

    void setInitialState(D3D12_RESOURCE_STATES state);

    void setHeapType(D3D12_HEAP_TYPE heapType);

    InternalResource *asInternal() override;

    const InternalResource *asInternal() const override;

private:

    friend class Compiler;

    D3D12_HEAP_TYPE heapType_;

    bool              clear_;
    D3D12_CLEAR_VALUE clearValue_;

    D3D12_RESOURCE_STATES initialState_;
};

class ExternalResource : public Resource
{
public:

    ExternalResource(std::string name, int index);

    void setInitialState(D3D12_RESOURCE_STATES state);

    void setFinalState(D3D12_RESOURCE_STATES state);

    ExternalResource *asExternal() override;

    const ExternalResource *asExternal() const override;

private:

    friend class Compiler;

    D3D12_RESOURCE_STATES initialState_;
    D3D12_RESOURCE_STATES finalState_;
};

AGZ_D3D12_GRAPH_END
