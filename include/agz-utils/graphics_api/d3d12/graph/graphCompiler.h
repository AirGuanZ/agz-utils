#pragma once

#include <set>

#include <agz-utils/graphics_api/d3d12/graph/graphRuntime.h>
#include <agz-utils/graphics_api/d3d12/resourceManager.h>

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

    friend class GraphCompiler;

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

    friend class GraphCompiler;

    D3D12_RESOURCE_STATES initialState_;
    D3D12_RESOURCE_STATES finalState_;
};

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

    friend class GraphCompiler;

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

    friend class GraphCompiler;

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
        const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc);

    DescriptorItem *addRTV(
        const Resource                      *resource,
        const D3D12_RENDER_TARGET_VIEW_DESC &desc);

    DescriptorItem *addDSV(
        const Resource                      *resource,
        DepthStencilType                     depthStencilType,
        const D3D12_DEPTH_STENCIL_VIEW_DESC &desc);
    
    DescriptorItem *addSRV(
        bool                                   cpu,
        bool                                   gpu,
        const Resource                        *resource,
        const D3D12_SHADER_RESOURCE_VIEW_DESC &desc)
    {
        return addSRV(
            cpu, gpu, resource, ShaderResourceType::PixelAndNonPixel, desc);
    }
    
    DescriptorItem *addDSV(
        const Resource                      *resource,
        const D3D12_DEPTH_STENCIL_VIEW_DESC &desc)
    {
        return addDSV(resource, DepthStencilType::ReadAndWrite, desc);
    }

    DescriptorItem *addDescriptor(bool cpuVisible, bool gpuVisible);

    DescriptorTable *addDescriptorTable(bool cpuVisible, bool gpuVisible);

private:

    friend class GraphCompiler;

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

    friend class GraphCompiler;

    std::string name_;

    Vertex *entry_;
    Vertex *exit_;
};

class GraphCompiler : public misc::uncopyable_t
{
public:

    GraphCompiler();

    void setThreadCount(int count);

    void setQueueCount(int count);

    void setFrameCount(int count);

    InternalResource *addInternalResource(std::string name);

    ExternalResource *addExternalResource(std::string name);

    Pass *addPass(
        std::string name,
        int thread = 0,
        int queue  = 0);

    PassAggregate *addAggregate(
        std::string name,
        Vertex   *entry = nullptr,
        Vertex   *exit  = nullptr);

    void addDependency(Vertex *head, Vertex *tail);

    void addCrossFrameDependency(Vertex *head, Vertex *tail);

    void compile(
        ID3D12Device                           *device,
        ResourceManager                        &resourceManager,
        DescriptorAllocator                    &descriptorAllocator,
        std::vector<ComPtr<ID3D12CommandQueue>> allQueues,
        GraphRuntime                           &runtime);

private:

    struct PassTemp
    {
        int parentSection = 0;
        int idxInThread   = 0;
        bool shouldSubmit = false;

        std::vector<PassRuntime::StateTransition> stateTransitions;

        struct DescriptorDeclaretion
        {
            const DescriptorItem *item           = nullptr;
            int                   descriptorSlot = 0;
        };

        struct DescriptorTableDeclaretion
        {
            const DescriptorTable *table               = nullptr;
            int                    descriptorRangeSlot = 0;
        };

        std::vector<DescriptorDeclaretion>      descriptors_;
        std::vector<DescriptorTableDeclaretion> descriptorRanges_;
    };

    struct SectionTemp
    {
        int parentThread        = 0;
        int indexInParentThread = 0;

        std::vector<int> passes;

        int externalDependenciesCount = 0;
        std::vector<int> outputs;

        std::vector<ComPtr<ID3D12Fence>> waitFencesOfLastFrame;
        std::vector<ComPtr<ID3D12Fence>> waitFences;
        ComPtr<ID3D12Fence>              signalFence;
    };

    struct ThreadTemp
    {
        std::vector<int> passes;
        std::vector<int> sections;

        struct DescriptorRecord
        {
            int                   slot;
            const DescriptorItem *item;
        };

        struct DescriptorRangeRecord
        {
            int                    slot;
            const DescriptorTable *table;
        };

        std::vector<DescriptorRecord>      descDecls;
        std::vector<DescriptorRangeRecord> descRanges;
    };

    struct Temps
    {
        std::vector<int>         linearPasses;
        std::vector<PassTemp>    passes;
        std::vector<SectionTemp> sections;
        std::vector<ThreadTemp>  threads;
    };

    void addDependencyImpl(Vertex *head, Vertex *tail, bool crossFrame);

    std::vector<int> sortPasses() const;

    Temps assignSectionsToThreads() const;

    void generateSectionDependencies(ID3D12Device *device, Temps &temps) const;

    void generateResourceTransitions(Temps &temps);

    void generateDescriptorRecords(Temps &temps);
    
    void fillRuntimeResources(
        GraphRuntime    &runtime,
        ResourceManager &rscMgr,
        Temps           &temps) const;
    
    void fillRuntimeDescriptors(
        ID3D12Device        *device,
        GraphRuntime        &runtime,
        DescriptorAllocator &GPUDescAlloc,
        Temps               &temps) const;
    
    void fillRuntimeSections(
        ID3D12Device *device,
        GraphRuntime &runtime,
        Temps        &temps) const;
    
    void fillRuntimeCommandAllocators(
        ID3D12Device *device,
        GraphRuntime &runtime) const;

    int threadCount_;
    int queueCount_;
    int frameCount_;

    std::vector<std::unique_ptr<Resource>> resources_;
    std::vector<std::unique_ptr<Pass>>     passes_;

    std::vector<std::unique_ptr<PassAggregate>> aggregates_;
};

AGZ_D3D12_GRAPH_END
