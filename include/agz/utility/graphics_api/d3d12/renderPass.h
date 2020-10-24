#pragma once

#include <d3dx12.h>

#include <agz/utility/graphics_api/d3d12/common.h>

AGZ_D3D12_BEGIN

namespace rg
{

class Pass : public misc::uncopyable_t
{
public:

    using Callback = std::function<void(ID3D12GraphicsCommandList*)>;

    Pass() = default;

    Pass(Pass &&other) noexcept = default;

    Pass &operator=(Pass &&other) noexcept = default;

    void setCallback(std::shared_ptr<Callback> callback) noexcept
    {
        callback_ = std::move(callback);
    }

    void addResourceStateTransition(
        int                   idx,
        D3D12_RESOURCE_STATES beg,
        D3D12_RESOURCE_STATES mid,
        D3D12_RESOURCE_STATES end)
    {
        rscStateTransitions_.push_back({ idx, beg, mid, end });
    }

    void execute(
        ID3D12GraphicsCommandList *cmdList,
        ID3D12Resource    * const *rscs) const
    {
        std::vector<D3D12_RESOURCE_BARRIER> beforeBarriers, afterBarriers;

        for(auto &t : rscStateTransitions_)
        {
            if(t.beg != t.mid)
            {
                beforeBarriers.push_back(
                    CD3DX12_RESOURCE_BARRIER::Transition(
                        rscs[t.idx], t.beg, t.mid));
            }

            if(t.mid != t.end)
            {
                afterBarriers.push_back(
                    CD3DX12_RESOURCE_BARRIER::Transition(
                        rscs[t.idx], t.mid, t.end));
            }
        }

        cmdList->ResourceBarrier(
            static_cast<UINT>(beforeBarriers.size()), beforeBarriers.data());

        (*callback_)(cmdList);

        cmdList->ResourceBarrier(
            static_cast<UINT>(afterBarriers.size()), afterBarriers.data());
    }

private:

    friend class RenderGraphBuilder;

    struct ResourceStateTransition
    {
        int idx;

        D3D12_RESOURCE_STATES beg;
        D3D12_RESOURCE_STATES mid;
        D3D12_RESOURCE_STATES end;
    };

    std::shared_ptr<Callback> callback_;

    std::vector<ResourceStateTransition> rscStateTransitions_;
};

class Section : public misc::uncopyable_t
{
public:

    Section() = default;

    Section(Section &&other) noexcept = default;

    Section &operator=(Section &&other) noexcept = default;

    void addPass(Pass pass)
    {
        passes_.push_back(std::move(pass));
    }

    void execute(ID3D12GraphicsCommandList *cmdList, ID3D12Resource *const *rscs) const
    {
        for(auto &p : passes_)
            p.execute(cmdList, rscs);
    }

private:

    std::vector<Pass> passes_;
};

} // namespace rg

AGZ_D3D12_END
