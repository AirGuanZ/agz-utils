#pragma once

#include <agz/utility/graphics_api/d3d12/descriptorHeapRaw.h>

AGZ_D3D12_BEGIN

class Device;
class SwapChain;
class Window;

class ImGuiIntegration : public misc::uncopyable_t
{
public:

    explicit ImGuiIntegration(
        Window &window, SwapChain &swapChain, Device &device);

    ~ImGuiIntegration();

    void newFrame();

    void render(ID3D12GraphicsCommandList *cmdList);

    ID3D12DescriptorHeap *getGPUHeap() noexcept;

private:

    class ImGuiInputDispatcher;

    RawDescriptorHeap srvHeap_;

    std::unique_ptr<ImGuiInputDispatcher> inputDispatcher_;
};

AGZ_D3D12_END
