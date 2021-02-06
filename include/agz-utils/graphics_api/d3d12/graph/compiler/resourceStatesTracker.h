#pragma once

#include <map>

#include <agz-utils/graphics_api/d3d12/graph/compiler/descriptorDecleration.h>

AGZ_D3D12_GRAPH_BEGIN

class ResourceStatesTracker
{
public:

    struct Usage
    {
        int                                   pass;
        std::map<UINT, D3D12_RESOURCE_STATES> state;
    };

    ResourceStatesTracker() = default;

    explicit ResourceStatesTracker(const Resource *resource);

    void addResourceState(
        int pass, UINT subresource, D3D12_RESOURCE_STATES state);

    void addDescriptor(
        int pass, const DescriptorInfo &descInfo);

    std::vector<Usage> getUsages() const;

private:

    struct Key
    {
        int  pass   = 0;
        UINT subrsc = 0;

        bool operator<(const Key &rhs) const;
    };

    void addResourceStateWithDetailedSubresource(
        int pass, UINT subresource, D3D12_RESOURCE_STATES state);

    const Resource *resource_    = nullptr;
    UINT            subrscCount_ = 0;

    std::map<Key, D3D12_RESOURCE_STATES> states_;
};

AGZ_D3D12_GRAPH_END
