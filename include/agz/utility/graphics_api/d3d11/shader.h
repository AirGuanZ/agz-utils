#pragma once

#include <map>

#include <d3dcompiler.h>

#include "../../file.h"
#include "../../misc.h"
#include "device.h"
#include "deviceContext.h"

AGZ_D3D11_BEGIN

enum class ShaderStage
{
    VS = 0,
    HS = 1,
    DS = 2,
    GS = 3,
    PS = 4,
    CS = 5
};

constexpr ShaderStage VS = ShaderStage::VS;
constexpr ShaderStage HS = ShaderStage::HS;
constexpr ShaderStage DS = ShaderStage::DS;
constexpr ShaderStage GS = ShaderStage::GS;
constexpr ShaderStage PS = ShaderStage::PS;
constexpr ShaderStage CS = ShaderStage::CS;

namespace shaderImpl
{

    template<ShaderStage STAGE>
    void bindConstantBuffer(
        UINT slot, ID3D11Buffer *buffer);

    template<ShaderStage STAGE>
    void bindShaderResourceView(
        UINT slot, ID3D11ShaderResourceView *srv);

    template<ShaderStage STAGE>
    void bindShaderResourceViewArray(
        UINT slot, UINT count, ID3D11ShaderResourceView **srvs);

    template<ShaderStage STAGE>
    void bindSampler(
        UINT slot, ID3D11SamplerState *sampler);

    template<ShaderStage STAGE>
    void bindUnorderedAccessView(
        UINT slot, ID3D11UnorderedAccessView *uav, UINT initialCounter);

#define AGZ_D3D11_BIND_CONSTANT_BUFFER(STAGE)                                   \
    template<>                                                                  \
    inline void bindConstantBuffer<ShaderStage::STAGE>(                         \
        UINT slot, ID3D11Buffer *buffer)                                        \
    {                                                                           \
        deviceContext.d3dDeviceContext->STAGE##SetConstantBuffers(              \
            slot, 1, &buffer);                                                  \
    }

#define AGZ_D3D11_BIND_SHADER_RESOURCE_VIEW(STAGE)                              \
    template<>                                                                  \
    inline void bindShaderResourceView<ShaderStage::STAGE>(                     \
        UINT slot, ID3D11ShaderResourceView *srv)                               \
    {                                                                           \
        deviceContext.d3dDeviceContext->STAGE##SetShaderResources(              \
            slot, 1, &srv);                                                     \
    }

#define AGZ_D3D11_BIND_SHADER_RESOURCE_VIEW_ARRAY(STAGE)                        \
    template<>                                                                  \
    inline void bindShaderResourceViewArray<ShaderStage::STAGE>(                \
        UINT slot, UINT count, ID3D11ShaderResourceView **srv)                  \
    {                                                                           \
        deviceContext.d3dDeviceContext->STAGE##SetShaderResources(              \
            slot, count, srv);                                                  \
    }

#define AGZ_D3D11_BIND_SAMPLER(STAGE)                                           \
    template<>                                                                  \
    inline void bindSampler<ShaderStage::STAGE>(                                \
        UINT slot, ID3D11SamplerState *sampler)                                 \
    {                                                                           \
        deviceContext.d3dDeviceContext->STAGE##SetSamplers(                     \
            slot, 1, &sampler);                                                 \
    }

    AGZ_D3D11_BIND_CONSTANT_BUFFER(VS)
    AGZ_D3D11_BIND_CONSTANT_BUFFER(HS)
    AGZ_D3D11_BIND_CONSTANT_BUFFER(DS)
    AGZ_D3D11_BIND_CONSTANT_BUFFER(GS)
    AGZ_D3D11_BIND_CONSTANT_BUFFER(PS)
    AGZ_D3D11_BIND_CONSTANT_BUFFER(CS)

    AGZ_D3D11_BIND_SHADER_RESOURCE_VIEW(VS)
    AGZ_D3D11_BIND_SHADER_RESOURCE_VIEW(HS)
    AGZ_D3D11_BIND_SHADER_RESOURCE_VIEW(DS)
    AGZ_D3D11_BIND_SHADER_RESOURCE_VIEW(GS)
    AGZ_D3D11_BIND_SHADER_RESOURCE_VIEW(PS)
    AGZ_D3D11_BIND_SHADER_RESOURCE_VIEW(CS)

    AGZ_D3D11_BIND_SHADER_RESOURCE_VIEW_ARRAY(VS)
    AGZ_D3D11_BIND_SHADER_RESOURCE_VIEW_ARRAY(HS)
    AGZ_D3D11_BIND_SHADER_RESOURCE_VIEW_ARRAY(DS)
    AGZ_D3D11_BIND_SHADER_RESOURCE_VIEW_ARRAY(GS)
    AGZ_D3D11_BIND_SHADER_RESOURCE_VIEW_ARRAY(PS)
    AGZ_D3D11_BIND_SHADER_RESOURCE_VIEW_ARRAY(CS)

    AGZ_D3D11_BIND_SAMPLER(VS)
    AGZ_D3D11_BIND_SAMPLER(HS)
    AGZ_D3D11_BIND_SAMPLER(DS)
    AGZ_D3D11_BIND_SAMPLER(GS)
    AGZ_D3D11_BIND_SAMPLER(PS)
    AGZ_D3D11_BIND_SAMPLER(CS)

    template<ShaderStage STAGE>
    void bindUnorderedAccessView(
        UINT slot, ID3D11UnorderedAccessView *uav, UINT initialCounter)
    {
        // do nothing
    }

    template<>
    inline void bindUnorderedAccessView<ShaderStage::CS>(
        UINT slot, ID3D11UnorderedAccessView *uav, UINT initialCounter)
    {
        deviceContext.d3dDeviceContext->CSSetUnorderedAccessViews(
            slot, 1, &uav, &initialCounter);
    }

#undef AGZ_D3D11_BIND_CONSTANT_BUFFER
#undef AGZ_D3D11_BIND_SHADER_RESOURCE_VIEW
#undef AGZ_D3D11_BIND_SHADER_RESOURCE_VIEW_ARRAY
#undef AGZ_D3D11_BIND_SAMPLER

} // namespace shaderImpl

template<ShaderStage STAGE>
class ConstantBufferSlot
{
    UINT slot_;
    ComPtr<ID3D11Buffer> buffer_;

public:

    explicit ConstantBufferSlot(UINT slot = 0) noexcept
        : slot_(slot)
    {
        
    }

    void setBuffer(ComPtr<ID3D11Buffer> buffer) noexcept
    {
        buffer_.Swap(buffer);
    }

    UINT getSlot() const noexcept
    {
        return slot_;
    }

    void bind() const
    {
        shaderImpl::bindConstantBuffer<STAGE>(slot_, buffer_.Get());
    }

    void unbind() const
    {
        shaderImpl::bindConstantBuffer<STAGE>(slot_, nullptr);
    }
};

template<ShaderStage STAGE>
class SamplerSlot
{
    UINT slot_;
    ComPtr<ID3D11SamplerState> sampler_;

public:

    explicit SamplerSlot(UINT slot = 0) noexcept
        : slot_(slot)
    {

    }

    void setSampler(ComPtr<ID3D11SamplerState> sampler)
    {
        sampler_.Swap(sampler);
    }

    UINT getSlot() const noexcept
    {
        return slot_;
    }

    void bind() const
    {
        shaderImpl::bindSampler<STAGE>(slot_, sampler_.Get());
    }

    void unbind() const
    {
        shaderImpl::bindSampler<STAGE>(slot_, nullptr);
    }
};

template<ShaderStage STAGE>
class ShaderResourceViewSlot
{
    UINT slot_;
    ComPtr<ID3D11ShaderResourceView> srv_;

public:

    explicit ShaderResourceViewSlot(UINT slot = 0) noexcept
        : slot_(slot)
    {

    }

    void setShaderResourceView(ComPtr<ID3D11ShaderResourceView> srv)
    {
        srv_.Swap(srv);
    }

    UINT getSlot() const noexcept
    {
        return slot_;
    }

    void bind() const
    {
        shaderImpl::bindShaderResourceView<STAGE>(slot_, srv_.Get());
    }

    void unbind() const
    {
        shaderImpl::bindShaderResourceView<STAGE>(slot_, nullptr);
    }
};

template<ShaderStage STAGE>
class ShaderResourceViewArraySlot
{
    UINT slot_;
    mutable std::vector<ID3D11ShaderResourceView *> srvs_;

    void addRef() noexcept
    {
        for(auto srv : srvs_)
        {
            if(srv)
                srv->AddRef();
        }
    }

    void decRef() noexcept
    {
        for(auto srv : srvs_)
        {
            if(srv)
                srv->Release();
        }
    }

public:

    explicit ShaderResourceViewArraySlot(UINT slot = 0, UINT count = 1)
        : slot_(slot), srvs_(count)
    {
        
    }

    ShaderResourceViewArraySlot(const ShaderResourceViewArraySlot<STAGE> &copy)
    {
        slot_ = copy.slot_;
        srvs_ = copy.srvs_;
        addRef();
    }

    ShaderResourceViewArraySlot<STAGE> &operator=(
        const ShaderResourceViewArraySlot<STAGE> &copy)
    {
        decRef();
        slot_ = copy.slot_;
        srvs_ = copy.srvs_;
        addRef();
        return *this;
    }

    ShaderResourceViewArraySlot(
        ShaderResourceViewArraySlot<STAGE> &&moveFrom) noexcept
        : slot_(moveFrom.slot_), srvs_(std::move(moveFrom.srvs_))
    {
        moveFrom.slot_ = 0;
    }

    ShaderResourceViewArraySlot<STAGE> &operator=(
        ShaderResourceViewArraySlot<STAGE> &&moveFrom) noexcept
    {
        std::swap(slot_, moveFrom.slot_);
        std::swap(srvs_, moveFrom.srvs_);
        return *this;
    }

    ~ShaderResourceViewArraySlot()
    {
        decRef();
    }

    int getSRVCount() const noexcept
    {
        return static_cast<int>(srvs_.size());
    }

    void setShaderResourceView(int index, ID3D11ShaderResourceView *srv)
    {
        auto &dst = srvs_[index];
        if(dst)
            dst->Release();
        dst = srv;
        if(dst)
            dst->AddRef();
    }

    void setShaderResourceView(
        int index, const ComPtr<ID3D11ShaderResourceView> &srv)
    {
        setShaderResourceView(index, srv.Get());
    }

    void setShaderResourceView(
        int start, int count, ID3D11ShaderResourceView **srvs)
    {
        for(int i = 0, j = start; i < count; ++i, ++j)
            setShaderResourceView(j, srvs[i]);
    }

    void bind() const
    {
        shaderImpl::bindShaderResourceViewArray<STAGE>(
            slot_, static_cast<UINT>(srvs_.size()), srvs_.data());
    }

    void unbind() const
    {
        if(srvs_.size() < 20)
        {
            static ID3D11ShaderResourceView *EMPTY_SRVS[20] = { nullptr };
            shaderImpl::bindShaderResourceViewArray<STAGE>(
                slot_, static_cast<UINT>(srvs_.size()), EMPTY_SRVS);
        }
        else
        {
            std::vector<ID3D11ShaderResourceView*> Es(srvs_.size(), nullptr);
            shaderImpl::bindShaderResourceViewArray<STAGE>(
                slot_, static_cast<UINT>(srvs_.size()), Es.data());
        }
    }
};

template<ShaderStage STAGE>
class UnorderedAccessViewSlot
{
    UINT slot_;
    ComPtr<ID3D11UnorderedAccessView> uav_;
    UINT initialCounter_;

public:

    explicit UnorderedAccessViewSlot(UINT slot = 0) noexcept
        : slot_(slot), initialCounter_(-1)
    {
        
    }

    void setUnorderedAccessView(
        ComPtr<ID3D11UnorderedAccessView> uav, UINT initialCounter = -1)
    {
        uav_.Swap(uav);
        initialCounter_ = initialCounter;
    }

    UINT getSlot() const noexcept
    {
        return slot_;
    }

    void bind() const
    {
        shaderImpl::bindUnorderedAccessView<STAGE>(slot_, uav_.Get(), initialCounter_);
    }

    void unbind() const
    {
        shaderImpl::bindUnorderedAccessView<STAGE>(slot_, nullptr, -1);
    }
};

namespace shaderImpl
{

    template<typename RESOURCE>
    class ResourceManager
    {
    public:

        using Record = RESOURCE;
        using RecordTable = std::map<std::string, Record, std::less<>>;

        ResourceManager() = default;

        explicit ResourceManager(RecordTable recordTable) noexcept
            : recordTable_(std::move(recordTable))
        {

        }

        template<typename...Args>
        void add(std::string name, Args &&...args)
        {
            if(recordTable_.find(name) != recordTable_.end())
                throw D3D11Exception("repeated resource name: " + name);

            recordTable_.insert(
                std::make_pair(
                    std::move(name),
                    RESOURCE(std::forward<Args>(args)...)));
        }

        RESOURCE *get(std::string_view name)
        {
            auto it = recordTable_.find(name);
            return it != recordTable_.end() ? &it->second : nullptr;
        }

        const RESOURCE *get(std::string_view name) const
        {
            auto it = recordTable_.find(name);
            return it != recordTable_.end() ? &it->second : nullptr;
        }

        void bind() const
        {
            for(auto &p : recordTable_)
                p.second.bind();
        }

        void unbind() const
        {
            for(auto &p : recordTable_)
                p.second.unbind();
        }

    private:

        RecordTable recordTable_;
    };
} // namespace shaderImpl

template<ShaderStage STAGE>
using ConstantBufferSlotManager =
    shaderImpl::ResourceManager<ConstantBufferSlot<STAGE>>;

template<ShaderStage STAGE>
using SamplerSlotManager =
    shaderImpl::ResourceManager<SamplerSlot<STAGE>>;

template<ShaderStage STAGE>
using ShaderResourceViewSlotManager =
    shaderImpl::ResourceManager<ShaderResourceViewSlot<STAGE>>;

template<ShaderStage STAGE>
using ShaderResourceViewArraySlotManager =
    shaderImpl::ResourceManager<ShaderResourceViewArraySlot<STAGE>>;

template<ShaderStage STAGE>
using UnorderedAccessViewSlotManager =
    shaderImpl::ResourceManager<UnorderedAccessViewSlot<STAGE>>;

namespace shaderImpl
{

    inline ComPtr<ID3D11ShaderReflection> getShaderReflection(
        ID3D10Blob *byteCode)
    {
        assert(byteCode);
        ComPtr<ID3D11ShaderReflection> reflection;
        HRESULT hr = D3DReflect(
            byteCode->GetBufferPointer(), byteCode->GetBufferSize(),
            IID_ID3D11ShaderReflection, (void **)reflection.GetAddressOf());
        return SUCCEEDED(hr) ? reflection : nullptr;
    }

    struct ConstantBufferInfo
    {
        UINT slot;
    };

    struct ShaderResourceInfo
    {
        UINT slot;
        UINT count;
    };

    struct UnorderedAccessInfo
    {
        UINT slot;
    };

    struct ShaderSamplerInfo
    {
        UINT slot;
    };

    inline void getShaderReflectionInfo(
        ID3D11ShaderReflection *reflection,
        std::map<std::string, ConstantBufferInfo>  &constantBufferInfo,
        std::map<std::string, ShaderResourceInfo>  &shaderResourceInfo,
        std::map<std::string, UnorderedAccessInfo> &unorderedAccessInfo,
        std::map<std::string, ShaderSamplerInfo>   &shaderSamplerInfo)
    {
        assert(reflection);

        D3D11_SHADER_DESC shaderDesc;
        reflection->GetDesc(&shaderDesc);

        for(UINT rsc = 0; rsc < shaderDesc.BoundResources; ++rsc)
        {
            D3D11_SHADER_INPUT_BIND_DESC bdDesc;
            reflection->GetResourceBindingDesc(rsc, &bdDesc);
            if(bdDesc.Type == D3D_SIT_CBUFFER)
            {
                constantBufferInfo.insert(std::make_pair(
                    std::string(bdDesc.Name),
                    ConstantBufferInfo{ bdDesc.BindPoint }));
            }
            else if(bdDesc.Type == D3D_SIT_TEXTURE ||
                bdDesc.Type == D3D_SIT_STRUCTURED ||
                bdDesc.Type == D3D_SIT_BYTEADDRESS)
            {
                shaderResourceInfo.insert(std::make_pair(
                    std::string(bdDesc.Name),
                    ShaderResourceInfo{ bdDesc.BindPoint, bdDesc.BindCount }));
            }
            else if(bdDesc.Type == D3D_SIT_SAMPLER)
            {
                shaderSamplerInfo.insert(std::make_pair(
                    std::string(bdDesc.Name),
                    ShaderSamplerInfo{ bdDesc.BindPoint }));
            }
            else if(bdDesc.Type == D3D_SIT_UAV_RWTYPED ||
                    bdDesc.Type == D3D_SIT_UAV_RWSTRUCTURED ||
                    bdDesc.Type == D3D_SIT_UAV_RWBYTEADDRESS ||
                    bdDesc.Type == D3D_SIT_UAV_APPEND_STRUCTURED ||
                    bdDesc.Type == D3D_SIT_UAV_CONSUME_STRUCTURED ||
                    bdDesc.Type == D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER)
            {
                // TODO: uav array
                if(bdDesc.BindCount > 1)
                    throw D3D11Exception("uav array is not implementated");

                unorderedAccessInfo.insert(std::make_pair(
                    std::string(bdDesc.Name),
                    UnorderedAccessInfo{ bdDesc.BindPoint }));
            }
        }
    }

    template<ShaderStage> struct StageToShaderType;
    template<> struct StageToShaderType<ShaderStage::VS> { using Type = ID3D11VertexShader;   };
    template<> struct StageToShaderType<ShaderStage::HS> { using Type = ID3D11HullShader;     };
    template<> struct StageToShaderType<ShaderStage::DS> { using Type = ID3D11DomainShader;   };
    template<> struct StageToShaderType<ShaderStage::GS> { using Type = ID3D11GeometryShader; };
    template<> struct StageToShaderType<ShaderStage::PS> { using Type = ID3D11PixelShader;    };
    template<> struct StageToShaderType<ShaderStage::CS> { using Type = ID3D11ComputeShader;  };

    template<ShaderStage STAGE>
    class StageTrait
    {
    public:

        using D3DShaderType = typename StageToShaderType<STAGE>::Type;

        static const char *defaultCompileTarget() noexcept
        {
            if constexpr(STAGE == ShaderStage::VS) return "vs_5_0";
            if constexpr(STAGE == ShaderStage::HS) return "hs_5_0";
            if constexpr(STAGE == ShaderStage::DS) return "ds_5_0";
            if constexpr(STAGE == ShaderStage::GS) return "gs_5_0";
            if constexpr(STAGE == ShaderStage::PS) return "ps_5_0";
            return "cs_5_0";
        }

#ifdef AGZ_DEBUG
        static constexpr UINT COMPILE_FLAG = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        static constexpr UINT COMPILE_FLAG = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

        static ComPtr<ID3D10Blob> compile(
            std::string_view        source,
            const char             *sourceName,
            const char             *target,
            const char             *entry,
            const D3D_SHADER_MACRO *macros)
        {
            ComPtr<ID3D10Blob> ret, err;
            const auto hr = D3DCompile(
                source.data(), source.size(), sourceName, macros,
                D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, target,
                COMPILE_FLAG, 0,
                ret.GetAddressOf(), err.GetAddressOf());
            if(FAILED(hr))
            {
                throw D3D11Exception(
                    reinterpret_cast<char*>(err->GetBufferPointer()));
            }
            return ret;
        }

        static ComPtr<ID3D10Blob> compile(
            std::string_view        source,
            const char             *target,
            const char             *entry,
            const D3D_SHADER_MACRO *macros)
        {
            ComPtr<ID3D10Blob> ret, err;
            const auto hr = D3DCompile(
                source.data(), source.size(), nullptr, macros, nullptr,
                entry, target, COMPILE_FLAG, 0,
                ret.GetAddressOf(), err.GetAddressOf());
            if(FAILED(hr))
            {
                throw D3D11Exception(
                    reinterpret_cast<char *>(err->GetBufferPointer()));
            }
            return ret;
        }

        static ComPtr<D3DShaderType> createShader(void *compiled, size_t len)
        {
            ComPtr<D3DShaderType> ret;
            HRESULT hr;
            if constexpr(STAGE == ShaderStage::VS)
            {
                hr = device.d3dDevice->CreateVertexShader(
                    compiled, len, nullptr, ret.GetAddressOf());
            }
            else if constexpr(STAGE == ShaderStage::HS)
            {
                hr = device.d3dDevice->CreateHullShader(
                    compiled, len, nullptr, ret.GetAddressOf());
            }
            else if constexpr(STAGE == ShaderStage::DS)
            {
                hr = device.d3dDevice->CreateDomainShader(
                    compiled, len, nullptr, ret.GetAddressOf());
            }
            else if constexpr(STAGE == ShaderStage::GS)
            {
                hr = device.d3dDevice->CreateGeometryShader(
                    compiled, len, nullptr, ret.GetAddressOf());
            }
            else if constexpr(STAGE == ShaderStage::PS)
            {
                hr = device.d3dDevice->CreatePixelShader(
                    compiled, len, nullptr, ret.GetAddressOf());
            }
            else
            {
                hr = device.d3dDevice->CreateComputeShader(
                    compiled, len, nullptr, ret.GetAddressOf());
            }

            if(FAILED(hr))
            {
                throw D3D11Exception(
                    "failed to create shader from compiled byte code");
            }

            return ret;
        }

        static void bind(D3DShaderType *shader)
        {
            if constexpr(STAGE == ShaderStage::VS)
                deviceContext.d3dDeviceContext->VSSetShader(shader, nullptr, 0);
            else if constexpr(STAGE == ShaderStage::HS)
                deviceContext.d3dDeviceContext->HSSetShader(shader, nullptr, 0);
            else if constexpr(STAGE == ShaderStage::DS)
                deviceContext.d3dDeviceContext->DSSetShader(shader, nullptr, 0);
            else if constexpr(STAGE == ShaderStage::GS)
                deviceContext.d3dDeviceContext->GSSetShader(shader, nullptr, 0);
            else if constexpr(STAGE == ShaderStage::PS)
                deviceContext.d3dDeviceContext->PSSetShader(shader, nullptr, 0);
            else
                deviceContext.d3dDeviceContext->CSSetShader(shader, nullptr, 0);
        }
    };

    template<ShaderStage STAGE>
    class Stage : public misc::uncopyable_t
    {
        ComPtr<typename StageTrait<STAGE>::D3DShaderType> shader_;
        ComPtr<ID3D10Blob> byteCode_;

        typename ConstantBufferSlotManager         <STAGE>::RecordTable cbRcds_;
        typename ShaderResourceViewSlotManager     <STAGE>::RecordTable srvRcds_;
        typename ShaderResourceViewArraySlotManager<STAGE>::RecordTable srvsRcds_;
        typename UnorderedAccessViewSlotManager    <STAGE>::RecordTable uavRcds_;
        typename SamplerSlotManager                <STAGE>::RecordTable samRcds_;

        void initRcds()
        {
            ComPtr<ID3D11ShaderReflection> refl =
                getShaderReflection(byteCode_.Get());
            if(!refl)
                throw D3D11Exception("failed to get shader reflection");

            std::map<std::string, ConstantBufferInfo>  constantBuffers;
            std::map<std::string, ShaderResourceInfo>  shaderResources;
            std::map<std::string, UnorderedAccessInfo> unorderedAccesses;
            std::map<std::string, ShaderSamplerInfo>   samplers;

            getShaderReflectionInfo(
                refl.Get(),
                constantBuffers, shaderResources, unorderedAccesses, samplers);

            for(auto &p : constantBuffers)
                cbRcds_[p.first] = ConstantBufferSlot<STAGE>(p.second.slot);

            for(auto &p : shaderResources)
            {
                const auto leftBracPos = p.first.find('[');
                if(leftBracPos == std::string::npos)
                {
                    assert(p.second.count == 1);
                    srvRcds_[p.first] = ShaderResourceViewSlot<STAGE>(
                        p.second.slot);
                }
                else
                {
                    srvsRcds_[p.first.substr(0, leftBracPos)] =
                        ShaderResourceViewArraySlot<STAGE>(
                            p.second.slot, p.second.count);
                }
            }

            for(auto &p : unorderedAccesses)
            {
                uavRcds_[p.first] = UnorderedAccessViewSlot<STAGE>(
                    p.second.slot);
            }

            for(auto &p : samplers)
                samRcds_[p.first] = SamplerSlot<STAGE>(p.second.slot);
        }

    public:

        using Trait = StageTrait<STAGE>;

        static constexpr ShaderStage ShaderStageValue = STAGE;

        void initialize(
            std::string_view        source,
            const char             *sourceName,
            const char             *entry,
            const char             *target,
            const D3D_SHADER_MACRO *macros)
        {
            misc::scope_guard_t guard([&] { this->destroy(); });

            byteCode_ = Trait::compile(
                source, sourceName, target, entry, macros);

            shader_ = Trait::createShader(
                byteCode_->GetBufferPointer(), byteCode_->GetBufferSize());

            initRcds();

            guard.dismiss();
        }

        void initialize(
            std::string_view        source,
            const char             *entry,
            const char             *target,
            const D3D_SHADER_MACRO *macros)
        {
            misc::scope_guard_t guard([&] { this->destroy(); });

            byteCode_ = Trait::compile(
                source, target, entry, macros);

            shader_ = Trait::createShader(
                byteCode_->GetBufferPointer(), byteCode_->GetBufferSize());

            initRcds();

            guard.dismiss();
        }

        void initialize(ComPtr<ID3D10Blob> byteCode)
        {
            misc::scope_guard_t guard([&] { this->destroy(); });

            byteCode_.Swap(byteCode);

            shader_ = Trait::createShader(
                byteCode_->GetBufferPointer(), byteCode_->GetBufferSize());

            initRcds();

            guard.dismiss();
        }

        bool isAvailable() const noexcept
        {
            return shader_ != nullptr;
        }

        void destroy()
        {
            shader_.Reset();
            byteCode_.Reset();

            cbRcds_.clear();
            srvRcds_.clear();
            srvsRcds_.clear();
            uavRcds_.clear();
            samRcds_.clear();
        }

        void bind() const
        {
            Trait::bind(shader_.Get());
        }

        void unbind() const
        {
            Trait::bind(nullptr);
        }

        const auto &getConstantBufferTable() const noexcept
        {
            return cbRcds_;
        }

        const auto &getShaderResourceViewTable() const noexcept
        {
            return srvRcds_;
        }

        const auto &getShaderResourceViewArrayTable() const noexcept
        {
            return srvsRcds_;
        }

        const auto &getUnorderedAccessViewTable() const noexcept
        {
            return uavRcds_;
        }

        const auto &getSamplerTable() const noexcept
        {
            return samRcds_;
        }

        ID3D10Blob *getShaderByteCode() const noexcept
        {
            return byteCode_.Get();
        }
    };

    template<ShaderStage STAGE>
    class StageResourceManager
    {
        ConstantBufferSlotManager<STAGE>          cb_;
        ShaderResourceViewSlotManager<STAGE>      srv_;
        ShaderResourceViewArraySlotManager<STAGE> srvs_;
        UnorderedAccessViewSlotManager<STAGE>     uav_;
        SamplerSlotManager<STAGE>                 sam_;

    public:

        StageResourceManager() = default;

        explicit StageResourceManager(const Stage<STAGE> &stage)
            : cb_(stage.getConstantBufferTable()),
              srv_(stage.getShaderResourceViewTable()),
              srvs_(stage.getShaderResourceViewArrayTable()),
              uav_(stage.getUnorderedAccessViewTable()),
              sam_(stage.getSamplerTable())
        {
            
        }

        auto getCBMgr()       noexcept { return &cb_; }
        auto getSRVMgr()      noexcept { return &srv_; }
        auto getSRVArrayMgr() noexcept { return &srvs_; }
        auto getUAVMgr()      noexcept { return &uav_; }
        auto getSamplerMgr()  noexcept { return &sam_; }

        auto getCBMgr()       const noexcept { return &cb_; }
        auto getSRVMgr()      const noexcept { return &srv_; }
        auto getSRVArrayMgr() const noexcept { return &srvs_; }
        auto getUAVMgr()      const noexcept { return &uav_; }
        auto getSamplerMgr()  const noexcept { return &sam_; }

        void bind() const
        {
            cb_.bind();
            srv_.bind();
            srvs_.bind();
            uav_.bind();
            sam_.bind();
        }

        void unbind() const
        {
            cb_.unbind();
            srv_.unbind();
            srvs_.unbind();
            uav_.unbind();
            sam_.unbind();
        }
    };

} // namespace shaderImpl

template<ShaderStage...STAGES>
class ResourceManager
{
public:

    ResourceManager() = default;

    explicit ResourceManager(const shaderImpl::Stage<STAGES> &...stages)
        : mgrs_(stages...)
    {
        
    }

    template<ShaderStage STAGE>
    ConstantBufferSlotManager<STAGE> *
        getConstantBufferSlotManager()
    {
        return getStageMgr<STAGE>()->getCBMgr();
    }

    template<ShaderStage STAGE>
    const ConstantBufferSlotManager<STAGE> *
        getConstantBufferSlotManager() const
    {
        return getStageMgr<STAGE>()->getCBMgr();
    }

    template<ShaderStage STAGE>
    ShaderResourceViewSlotManager<STAGE> *
        getShaderResourceViewSlotManager()
    {
        return getStageMgr<STAGE>()->getSRVMgr();
    }

    template<ShaderStage STAGE>
    const ShaderResourceViewSlotManager<STAGE> *
        getShaderResourceViewSlotManager() const
    {
        return getStageMgr<STAGE>()->getSRVMgr();
    }

    template<ShaderStage STAGE>
    ShaderResourceViewArraySlotManager<STAGE> *
        getShaderResourceViewArraySlotManager()
    {
        return getStageMgr<STAGE>()->getSRVArrayMgr();
    }

    template<ShaderStage STAGE>
    const ShaderResourceViewArraySlotManager<STAGE> *
        getShaderResourceViewArraySlotManager() const
    {
        return getStageMgr<STAGE>()->getSRVArrayMgr();
    }

    template<ShaderStage STAGE>
    UnorderedAccessViewSlotManager<STAGE> *
        getUnorderedAccessViewSlotManager()
    {
        return getStageMgr<STAGE>()->getUAVMgr();
    }

    template<ShaderStage STAGE>
    const UnorderedAccessViewSlotManager<STAGE> *
        getUnorderedAccessViewSlotManager() const
    {
        return getStageMgr<STAGE>()->getUAVMgr();
    }

    template<ShaderStage STAGE>
    SamplerSlotManager<STAGE> *
        getSamplerSlotManager()
    {
        return getStageMgr<STAGE>()->getSamplerMgr();
    }

    template<ShaderStage STAGE>
    const SamplerSlotManager<STAGE> *
        getSamplerSlotManager() const
    {
        return getStageMgr<STAGE>()->getSamplerMgr();
    }

    template<ShaderStage STAGE>
    ConstantBufferSlot<STAGE> *
        getConstantBufferSlot(std::string_view name)
    {
        return getConstantBufferSlotManager<STAGE>()->get(name);
    }

    template<ShaderStage STAGE>
    const ConstantBufferSlot<STAGE> *
        getConstantBufferSlot(std::string_view name) const
    {
        return getConstantBufferSlotManager<STAGE>()->get(name);
    }

    template<ShaderStage STAGE>
    ShaderResourceViewSlot<STAGE> *
        getShaderResourceViewSlot(std::string_view name)
    {
        return getShaderResourceViewSlotManager<STAGE>()->get(name);
    }

    template<ShaderStage STAGE>
    const ShaderResourceViewSlot<STAGE> *
        getShaderResourceViewSlot(std::string_view name) const
    {
        return getShaderResourceViewSlotManager<STAGE>()->get(name);
    }

    template<ShaderStage STAGE>
    ShaderResourceViewArraySlot<STAGE> *
        getShaderResourceViewArraySlot(std::string_view name)
    {
        return getShaderResourceViewArraySlotManager<STAGE>()->get(name);
    }

    template<ShaderStage STAGE>
    const ShaderResourceViewArraySlot<STAGE> *
        getShaderResourceViewArraySlot(std::string_view name) const
    {
        return getShaderResourceViewArraySlotManager<STAGE>()->get(name);
    }

    template<ShaderStage STAGE>
    UnorderedAccessViewSlot<STAGE> *
        getUnorderedAccessViewSlot(std::string_view name)
    {
        return getUnorderedAccessViewSlotManager<STAGE>()->get(name);
    }


    template<ShaderStage STAGE>
    const UnorderedAccessViewSlot<STAGE> *
        getUnorderedAccessViewSlot(std::string_view name) const
    {
        return getUnorderedAccessViewSlotManager<STAGE>()->get(name);
    }

    template<ShaderStage STAGE>
    SamplerSlot<STAGE> *
        getSamplerSlot(std::string_view name)
    {
        return getSamplerSlotManager<STAGE>()->get(name);
    }

    template<ShaderStage STAGE>
    const SamplerSlot<STAGE> *
        getSamplerSlot(std::string_view name) const
    {
        return getSamplerSlotManager<STAGE>()->get(name);
    }

    void bind() const
    {
        std::apply(
            [](const auto &...mgr) { ((mgr.bind()), ...); }, mgrs_);
    }

    void unbind() const
    {
        std::apply(
            [](const auto &...mgr) { ((mgr.unbind()), ...); }, mgrs_);
    }

private:

    template<ShaderStage STAGE>
    auto getStageMgr() noexcept
    {
        return &std::get<shaderImpl::StageResourceManager<STAGE>>(mgrs_);
    }

    template<ShaderStage STAGE>
    auto getStageMgr() const noexcept
    {
        return &std::get<shaderImpl::StageResourceManager<STAGE>>(mgrs_);
    }

    std::tuple<shaderImpl::StageResourceManager<STAGES>...> mgrs_;
};

template<ShaderStage...STAGES>
class Shader : public misc::uncopyable_t
{
    std::tuple<shaderImpl::Stage<STAGES>...> stages_;

public:

    using Self = Shader<STAGES...>;

    template<ShaderStage STAGE>
    static const char *defaultTarget() noexcept
    {
        return shaderImpl::Stage<STAGE>::Trait::defaultCompileTarget();
    }

    template<ShaderStage STAGE>
    Self &initializeStage(
        std::string_view        source,
        const char              *sourceName,
        const  D3D_SHADER_MACRO *macros = nullptr,
        const char              *entry  = "main",
        const char              *target = defaultTarget<STAGE>())
    {
        auto &stage = std::get<shaderImpl::Stage<STAGE>>(stages_);
        stage.initialize(source, sourceName, entry, target, macros);
        return *this;
    }

    template<ShaderStage STAGE>
    Self &initializeStage(
        std::string_view        source,
        const D3D_SHADER_MACRO *macros = nullptr,
        const char             *entry  = "main",
        const char             *target = defaultTarget<STAGE>())
    {
        auto &stage = std::get<shaderImpl::Stage<STAGE>>(stages_);
        stage.initialize(source, entry, target, macros);
        return *this;
    }
    
    template<ShaderStage STAGE>
    Self &initializeStage(
        const char             *source,
        const D3D_SHADER_MACRO *macros = nullptr,
        const char             *entry  = "main",
        const char             *target = defaultTarget<STAGE>())
    {
        this->initializeStage<STAGE>(
            std::string_view(source), macros, entry, target);
        return *this;
    }

    template<ShaderStage STAGE>
    Self &initializeStage(ComPtr<ID3D10Blob> byteCode)
    {
        auto &stage = std::get<shaderImpl::Stage<STAGE>>(stages_);
        stage.initialize(std::move(byteCode));
        return *this;
    }

    template<ShaderStage STAGE>
    Self &initializeStageFromFile(
        const std::string       &filename,
        const D3D_SHADER_MACRO  *macros = nullptr,
        const char              *entry  = "main",
        const char              *target = defaultTarget<STAGE>())
    {
        const auto source = file::read_txt_file(filename);
        this->initializeStage<STAGE>(
            std::string_view(source), filename.c_str(), macros, entry, target);
        return *this;
    }

    void destroy()
    {
        std::apply(
            [](auto &...stage) { ((stage.destroy()), ...); },
            stages_);
    }

    bool isAllStageAvailable() const noexcept
    {
        bool ret = true;
        std::apply(
            [&](auto &...stage) { ((ret &= stage.isAvailable()), ...); },
            stages_);
        return ret;
    }

    template<ShaderStage STAGE>
    shaderImpl::Stage<STAGE> *getStage() noexcept
    {
        return &std::get<shaderImpl::Stage<STAGE>>(stages_);
    }

    template<ShaderStage STAGE>
    const shaderImpl::Stage<STAGE> *getStage() const noexcept
    {
        return &std::get<shaderImpl::Stage<STAGE>>(stages_);
    }

    ResourceManager<STAGES...> createResourceManager() const
    {
        return misc::construct_from_tuple<ResourceManager<STAGES...>>(stages_);
    }

    ID3D10Blob *getVertexShaderByteCode() const noexcept
    {
        return getStage<ShaderStage::VS>()->getShaderByteCode();
    }

    void bind() const
    {
        std::apply(
            [](const auto &...stage) { ((stage.bind()), ...); },
            stages_);
    }

    void unbind() const
    {
        std::apply(
            [](const auto &...stage) { ((stage.unbind()), ...); },
            stages_);
    }
};

AGZ_D3D11_END
