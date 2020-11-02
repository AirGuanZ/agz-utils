#pragma once

#include <agz/utility/graphics_api/d3d12/descriptorHeap.h>

AGZ_D3D12_BEGIN

class Device;
class SwapChain;
class Window;

class ImGuiIntegration : public misc::uncopyable_t
{
public:

    explicit ImGuiIntegration(
        Window    &window,
        SwapChain &swapChain,
        Device    &device,
        Descriptor SRVDesc);

    ~ImGuiIntegration();

    void newFrame();

    void render(ID3D12GraphicsCommandList *cmdList);

private:

    class ImGuiInputDispatcher;

    Descriptor SRVDesc_;

    std::unique_ptr<ImGuiInputDispatcher> inputDispatcher_;
};

AGZ_D3D12_END
