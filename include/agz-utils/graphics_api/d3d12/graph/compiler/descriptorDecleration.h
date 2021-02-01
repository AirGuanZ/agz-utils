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
        const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc);

    void setRTV(
        const Resource                      *resource,
        const D3D12_RENDER_TARGET_VIEW_DESC &desc);

    void setDSV(
        const Resource                      *resource,
        DepthStencilType                     type,
        const D3D12_DEPTH_STENCIL_VIEW_DESC &desc);
    
    void setSRV(
        const Resource                        *resource,
        const D3D12_SHADER_RESOURCE_VIEW_DESC &desc)
    {
        setSRV(resource, ShaderResourceType::PixelAndNonPixel, desc);
    }
    
    void setDSV(
        const Resource                      *resource,
        const D3D12_DEPTH_STENCIL_VIEW_DESC &desc)
    {
        setDSV(resource, DepthStencilType::ReadAndWrite, desc);
    }

    const Resource *getResource() const;

    bool operator<(const DescriptorItem &rhs) const;

private:

    friend class Compiler;

    bool cpu_;
    bool gpu_;

    const Resource    *resource_           = nullptr;
    ResourceView       view_               = {};
    ShaderResourceType shaderResourceType_ = ShaderResourceType::PixelAndNonPixel;
    DepthStencilType   depthStencilType_   = DepthStencilType::ReadAndWrite;
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
        const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc);

    void addRTV(
        const Resource                      *resource,
        const D3D12_RENDER_TARGET_VIEW_DESC &desc);

    void addDSV(
        const Resource                      *resource,
        DepthStencilType                     type,
        const D3D12_DEPTH_STENCIL_VIEW_DESC &desc);
    
    void addSRV(
        const Resource                        *resource,
        const D3D12_SHADER_RESOURCE_VIEW_DESC &desc)
    {
        addSRV(resource, ShaderResourceType::PixelAndNonPixel, desc);
    }
    
    void addDSV(
        const Resource                      *resource,
        const D3D12_DEPTH_STENCIL_VIEW_DESC &desc)
    {
        addDSV(resource, DepthStencilType::ReadAndWrite, desc);
    }

    bool operator<(const DescriptorTable &rhs) const;

private:

    friend class Compiler;

    struct Record
    {
        const Resource    *resource           = nullptr;
        ResourceView       view               = {};
        ShaderResourceType shaderResourceType = ShaderResourceType::PixelAndNonPixel;
        DepthStencilType   depthStencilType   = DepthStencilType::ReadAndWrite;

        bool operator<(const Record &rhs) const;
    };

    bool cpu_;
    bool gpu_;

    std::vector<Record> records_;
};

AGZ_D3D12_GRAPH_END
