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

private:

    friend class GraphCompiler;

    struct Record
    {
        const Resource    *resource           = nullptr;
        ResourceView       view               = {};
        ShaderResourceType shaderResourceType = ShaderResourceType::PixelAndNonPixel;
        DepthStencilType   depthStencilType   = DepthStencilType::ReadAndWrite;
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

    enum DescriptorType
    {
        CPUOnly,
        GPUOnly,
        BothCPUAndGPU
    };

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
        D3D12_RESOURCE_STATES state);

    void addSRV(
        DescriptorType                         type,
        const Resource                        *resource,
        ShaderResourceType                     shaderResourceType,
        const D3D12_SHADER_RESOURCE_VIEW_DESC &desc);

    void addUAV(
        DescriptorType                          type,
        const Resource                         *resource,
        const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc);

    void addRTV(
        const Resource                      *resource,
        const D3D12_RENDER_TARGET_VIEW_DESC &desc);

    void addDSV(
        const Resource                      *resource,
        DepthStencilType                     depthStencilType,
        const D3D12_DEPTH_STENCIL_VIEW_DESC &desc);
    
    void addSRV(
        DescriptorType                         type,
        const Resource                        *resource,
        const D3D12_SHADER_RESOURCE_VIEW_DESC &desc)
    {
        addSRV(
            type, resource, ShaderResourceType::PixelAndNonPixel, desc);
    }
    
    void addDSV(
        const Resource                      *resource,
        const D3D12_DEPTH_STENCIL_VIEW_DESC &desc)
    {
        addDSV(resource, DepthStencilType::ReadAndWrite, desc);
    }

    DescriptorTable *addDescriptorTable(DescriptorType type);

private:

    friend class GraphCompiler;

    struct DescriptorDeclaretion
    {
        const Resource    *resource           = nullptr;
        DescriptorType     type               = CPUOnly;
        ResourceView       view               = {};
        ShaderResourceType shaderResourceType = ShaderResourceType::PixelAndNonPixel;
        DepthStencilType   depthStencilType   = DepthStencilType::ReadAndWrite;

        bool operator<(const DescriptorDeclaretion &rhs) const;
    };

    std::string name_;
    int         index_;

    int thread_;
    int queue_;

    std::shared_ptr<PassCallback> callback_;

    std::set<Pass *> in_;
    std::set<Pass *> out_;

    std::map<const Resource*, D3D12_RESOURCE_STATES> states_;
    std::vector<DescriptorDeclaretion>               descriptors_;
    std::vector<std::unique_ptr<DescriptorTable>>    descriptorTables_;
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
        bool shouldSubmit = false;

        std::vector<PassRuntime::StateTransition> stateTransitions;

        struct DescriptorDeclaretion
        {
            const Resource *resource       = nullptr;
            int             descriptorSlot = 0;
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

        std::vector<ComPtr<ID3D12Fence>> waitFences;
        ComPtr<ID3D12Fence>              signalFence;
    };

    struct ThreadTemp
    {
        std::vector<int> passes;
        std::vector<int> sections;

        struct DescriptorRecord
        {
            int                         slot;
            Pass::DescriptorDeclaretion decl;
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
        std::vector<PassTemp>    passes;
        std::vector<SectionTemp> sections;
        std::vector<ThreadTemp>  threads;
    };

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
