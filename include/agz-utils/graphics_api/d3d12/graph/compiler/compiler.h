#pragma once

#include <set>

#include <agz-utils/graphics_api/d3d12/graph/compiler/descriptorSlotAssigner.h>
#include <agz-utils/graphics_api/d3d12/graph/compiler/pass.h>
#include <agz-utils/graphics_api/d3d12/graph/runtime/runtime.h>
#include <agz-utils/graphics_api/d3d12/resourceManager.h>

AGZ_D3D12_GRAPH_BEGIN

class Compiler : public misc::uncopyable_t
{
public:

    Compiler();

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
        Vertex *entry = nullptr,
        Vertex *exit  = nullptr);

    void addDependency(Vertex *head, Vertex *tail);

    void addCrossFrameDependency(Vertex *head, Vertex *tail);

    void compile(
        ID3D12Device                           *device,
        ResourceManager                        &resourceManager,
        DescriptorAllocator                    &descriptorAllocator,
        std::vector<ComPtr<ID3D12CommandQueue>> allQueues,
        Runtime                           &runtime);

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
    };

    struct Temps
    {
        std::vector<int>         linearPasses;
        std::vector<PassTemp>    passes;
        std::vector<SectionTemp> sections;
        std::vector<ThreadTemp>  threads;

        DescriptorSlotAssigner descSlotAllocator;
    };

    void addDependencyImpl(Vertex *head, Vertex *tail, bool crossFrame);

    std::vector<int> sortPasses() const;

    Temps assignSectionsToThreads() const;

    void generateSectionDependencies(ID3D12Device *device, Temps &temps) const;

    void generateResourceTransitions(Temps &temps);

    void generateDescriptorRecords(Temps &temps);
    
    void fillRuntimeResources(
        Runtime    &runtime,
        ResourceManager &rscMgr,
        Temps           &temps) const;
    
    void fillRuntimeDescriptors(
        ID3D12Device        *device,
        Runtime        &runtime,
        DescriptorAllocator &GPUDescAlloc,
        Temps               &temps) const;
    
    void fillRuntimeSections(
        ID3D12Device *device,
        Runtime &runtime,
        Temps        &temps) const;
    
    void fillRuntimeCommandAllocators(
        ID3D12Device *device,
        Runtime &runtime) const;

    int threadCount_;
    int queueCount_;
    int frameCount_;

    std::vector<std::unique_ptr<Resource>> resources_;
    std::vector<std::unique_ptr<Pass>>     passes_;

    std::vector<std::unique_ptr<PassAggregate>> aggregates_;
};

AGZ_D3D12_GRAPH_END
