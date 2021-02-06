#pragma once

#include <agz-utils/graphics_api/d3d12/graph/compiler/resource.h>

AGZ_D3D12_GRAPH_BEGIN

enum class ShaderResourceType
{
    PixelOnly,
    NonPixelOnly,
    PixelAndNonPixel
};

enum class DepthStencilType
{
    ReadOnly,
    ReadAndWrite
};

struct DescriptorInfo
{
    const Resource    *resource           = nullptr;
    ResourceView       view               = {};
    ShaderResourceType shaderResourceType = ShaderResourceType::PixelAndNonPixel;
    DepthStencilType   depthStencilType   = DepthStencilType::ReadAndWrite;
    const Resource    *uavCounterResource = nullptr;

    bool operator<(const DescriptorInfo &info) const;
};

class DescriptorItem
{
public:

    DescriptorItem(bool cpu, bool gpu);

    void setSRV(
        const Resource                        *resource,
        ShaderResourceType                     type,
        const D3D12_SHADER_RESOURCE_VIEW_DESC &desc);
    
    void setUAV(
        const Resource                         *resource,
        const Resource                         *uavCounterResource,
        const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc);

    void setRTV(
        const Resource                      *resource,
        const D3D12_RENDER_TARGET_VIEW_DESC &desc);

    void setDSV(
        const Resource                      *resource,
        DepthStencilType                     type,
        const D3D12_DEPTH_STENCIL_VIEW_DESC &desc);

    const Resource *getResource() const;

    bool operator<(const DescriptorItem &rhs) const;

    const DescriptorInfo &getInfo() const;

    bool isOnCPU() const;

    bool isOnGPU() const;

private:

    friend class Compiler;

    bool cpu_;
    bool gpu_;

    DescriptorInfo info_;
};

class DescriptorTable
{
public:

    DescriptorTable(bool cpu, bool gpu);

    void addSRV(
        const Resource                        *resource,
        ShaderResourceType                     type,
        const D3D12_SHADER_RESOURCE_VIEW_DESC &desc);

    void addUAV(
        const Resource                         *resource,
        const Resource                         *uavCounterResource,
        const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc);

    void addRTV(
        const Resource                      *resource,
        const D3D12_RENDER_TARGET_VIEW_DESC &desc);

    void addDSV(
        const Resource                      *resource,
        DepthStencilType                     type,
        const D3D12_DEPTH_STENCIL_VIEW_DESC &desc);

    bool operator<(const DescriptorTable &rhs) const;

    misc::span<const DescriptorInfo> getRecords() const;

    bool isOnCPU() const;

    bool isOnGPU() const;

private:

    friend class Compiler;

    bool cpu_;
    bool gpu_;

    std::vector<DescriptorInfo> records_;
};

AGZ_D3D12_GRAPH_END
