#pragma once

#include <agz/utility/graphics_api/d3d12/graph/graphCompiler.h>
#include <agz/utility/graphics_api/d3d12/graph/graphRuntime.h>

AGZ_D3D12_GRAPH_BEGIN

class Graph : public misc::uncopyable_t
{
public:

    Graph();

    Graph(Graph &&other) noexcept;

    Graph &operator=(Graph &&other) noexcept;

    void swap(Graph &other) noexcept;

    // compile

    void setThreadCount(int count);

    void setQueueCount(int count);

    void setFrameCount(int count);

    InternalResource *addInternalResource(std::string name);

    ExternalResource *addExternalResource(std::string name);

    Pass *addPass(std::string name, int thread = 0, int queue = 0);

    void addDependency(Pass *head, Pass *tail);

    void compile(
        ID3D12Device                           *device,
        ResourceManager                        &rscMgr,
        DescriptorAllocator                    &GPUDescAlloc,
        std::vector<ComPtr<ID3D12CommandQueue>> queues);

    // run

    void runAsync(int frameIndex);

    void sync();

    void run(int frameIndex);

    ID3D12Resource *getRawResource(Resource *resource) noexcept;

    ID3D12Resource *getRawResource(int index) noexcept;

    void setExternalResource(
        ExternalResource *node, ComPtr<ID3D12Resource> rsc);

    void clearExternalResources();

private:

    std::unique_ptr<GraphCompiler> compiler_;
    std::unique_ptr<GraphRuntime>  runtime_;
};

inline Graph::Graph()
{
    compiler_ = std::make_unique<GraphCompiler>();
    runtime_  = std::make_unique<GraphRuntime>();
}

inline Graph::Graph(Graph &&other) noexcept
    : Graph()
{
    swap(other);
}

inline Graph &Graph::operator=(Graph &&other) noexcept
{
    swap(other);
    return *this;
}

inline void Graph::swap(Graph &other) noexcept
{
    compiler_.swap(other.compiler_);
    runtime_.swap(other.runtime_);
}

inline void Graph::setThreadCount(int count)
{
    compiler_->setThreadCount(count);
}

inline void Graph::setQueueCount(int count)
{
    compiler_->setQueueCount(count);
}

inline void Graph::setFrameCount(int count)
{
    compiler_->setFrameCount(count);
}

inline InternalResource *Graph::addInternalResource(std::string name)
{
    return compiler_->addInternalResource(std::move(name));
}

inline ExternalResource *Graph::addExternalResource(std::string name)
{
    return compiler_->addExternalResource(std::move(name));
}

inline Pass *Graph::addPass(std::string name, int thread, int queue)
{
    return compiler_->addPass(std::move(name), thread, queue);
}

inline void Graph::addDependency(Pass *head, Pass *tail)
{
    return compiler_->addDependency(head, tail);
}

inline void Graph::compile(
    ID3D12Device                           *device,
    ResourceManager                        &rscMgr,
    DescriptorAllocator                    &GPUDescAlloc,
    std::vector<ComPtr<ID3D12CommandQueue>> queues)
{
    compiler_->compile(
        device, rscMgr, GPUDescAlloc, std::move(queues), *runtime_);
}

inline void Graph::runAsync(int frameIndex)
{
    runtime_->runAsync(frameIndex);
}

inline void Graph::sync()
{
    runtime_->sync();
}

inline void Graph::run(int frameIndex)
{
    runtime_->run(frameIndex);
}

inline ID3D12Resource *Graph::getRawResource(Resource *resource) noexcept
{
    return runtime_->getRawResource(resource);
}

inline ID3D12Resource *Graph::getRawResource(int index) noexcept
{
    return runtime_->getRawResource(index);
}

inline void Graph::setExternalResource(
    ExternalResource *node, ComPtr<ID3D12Resource> rsc)
{
    runtime_->setExternalResource(node, std::move(rsc));
}

inline void Graph::clearExternalResources()
{
    runtime_->clearExternalResources();
}

AGZ_D3D12_GRAPH_END
