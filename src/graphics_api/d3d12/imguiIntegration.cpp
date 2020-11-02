#ifdef AGZ_ENABLE_D3D12

#include <agz/utility/graphics_api/d3d12/device.h>
#include <agz/utility/graphics_api/d3d12/imguiIntegration.h>
#include <agz/utility/graphics_api/d3d12/swapChain.h>
#include <agz/utility/graphics_api/d3d12/window.h>

#include <agz/utility/graphics_api/imgui/imgui_impl_dx12.h>
#include <agz/utility/graphics_api/imgui/imgui_impl_win32.h>
#include <agz/utility/graphics_api/imgui/inputDispatcher.h>

AGZ_D3D12_BEGIN

class ImGuiIntegration::ImGuiInputDispatcher :
    public event::receiver_t<RawKeyDownEvent>,
    public event::receiver_t<RawKeyUpEvent>,
    public event::receiver_t<CharInputEvent>,
    public event::receiver_t<MouseButtonDownEvent>,
    public event::receiver_t<MouseButtonUpEvent>,
    public event::receiver_t<WheelScrollEvent>
{
    static int ToImGuiMouseButton(MouseButton mb) noexcept
    {
        if(mb == MouseButton::Left)  return 0;
        if(mb == MouseButton::Right) return 1;
        return 2;
    }

public:

    void handle(const RawKeyDownEvent &e) override
    {
        ImGui::Input::keyDown(e.vk);
    }

    void handle(const RawKeyUpEvent &e) override
    {
        ImGui::Input::keyUp(e.vk);
    }

    void handle(const CharInputEvent &e) override
    {
        ImGui::Input::charInput(e.ch);
    }

    void handle(const MouseButtonDownEvent &e) override
    {
        ImGui::Input::mouseButtonDown(ToImGuiMouseButton(e.button));
    }

    void handle(const MouseButtonUpEvent &e) override
    {
        ImGui::Input::mouseButtonUp(ToImGuiMouseButton(e.button));
    }

    void handle(const WheelScrollEvent &e) override
    {
        ImGui::Input::mouseWheel(e.offset);
    }

    void attachTo(Input &input)
    {
        input.attach(static_cast<receiver_t<RawKeyDownEvent>      *>(this));
        input.attach(static_cast<receiver_t<RawKeyUpEvent>        *>(this));
        input.attach(static_cast<receiver_t<CharInputEvent>       *>(this));
        input.attach(static_cast<receiver_t<MouseButtonDownEvent> *>(this));
        input.attach(static_cast<receiver_t<MouseButtonUpEvent>   *>(this));
        input.attach(static_cast<receiver_t<WheelScrollEvent>     *>(this));
    }
};

ImGuiIntegration::ImGuiIntegration(
    Window &window, SwapChain &swapChain, Device &device, Descriptor SRVDesc)
{
    SRVDesc_ = SRVDesc;

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(window.getWindowHandle());
    ImGui_ImplDX12_Init(
        device,
        swapChain.getImageCount(),
        swapChain.getImageFormat(),
        nullptr,
        SRVDesc_,
        SRVDesc_);
    ImGui::StyleColorsLight();

    inputDispatcher_ = std::make_unique<ImGuiInputDispatcher>();
    inputDispatcher_->attachTo(*window.getInput());
}

ImGuiIntegration::~ImGuiIntegration()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(ImGui::GetCurrentContext());
}

void ImGuiIntegration::newFrame()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiIntegration::render(ID3D12GraphicsCommandList *cmdList)
{
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
