#pragma once

#include <agz-utils/graphics_api/d3d12/graph/graph.h>
#include <agz-utils/graphics_api/d3d12/descriptorHeap.h>

AGZ_D3D12_BEGIN

class Device;
class SwapChain;
class Window;

class ImGuiIntegration : public misc::uncopyable_t
{
public:

    ImGuiIntegration();

    explicit ImGuiIntegration(
        Window    &window,
        SwapChain &swapChain,
        Device    &device,
        Descriptor SRVDesc);

    ~ImGuiIntegration();

    void initialize(
        Window    &window,
        SwapChain &swapChain,
        Device    &device,
        Descriptor SRVDesc);

    bool isAvailable() const noexcept;

    void destroy();

    rg::Pass *addToRenderGraph(
        rg::Graph    &graph,
        rg::Resource *renderTarget,
        int           thread = 0,
        int           queue  = 0);

    void newFrame();

    void render(ID3D12GraphicsCommandList *cmdList);

private:

    class ImGuiInputDispatcher;

    Descriptor SRVDesc_;

    std::unique_ptr<ImGuiInputDispatcher> inputDispatcher_;
};

AGZ_D3D12_END
