#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>

#include <agz/utility/d3d11/D3D/Shader/ConstantBufferSlot.h>
#include <agz/utility/d3d11/D3D/Shader/Reflection.h>
#include <agz/utility/d3d11/D3D/Shader/SamplerSlot.h>
#include <agz/utility/d3d11/D3D/Shader/ShaderResourceSlot.h>

AGZ_D3D11_BEGIN

namespace Impl
{

    template<ShaderStage STAGE>
    class StageSpecificImpl;

    template<>
    class StageSpecificImpl<ShaderStage::VS>
    {
    public:

        using D3DShaderType = ID3D11VertexShader;

        static const char *DefaultCompileTarget() noexcept
        {
            return "vs_5_0";
        }

#ifdef _DEBUG
        static constexpr UINT COMPILER_FLAGS = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        static constexpr UINT COMPILER_FLAGS = 0;
#endif

        static ComPtr<ID3D10Blob> CompileShader(
            std::string_view source, const char *sourceName, const char *target, const char *entry, D3D_SHADER_MACRO *macros, std::string &errMsg)
        {
            ComPtr<ID3D10Blob> ret, err;
            HRESULT hr = D3DCompile(
                source.data(), source.size(), sourceName, macros, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                entry, target, COMPILER_FLAGS, 0, ret.GetAddressOf(), err.GetAddressOf());
            if(FAILED(hr))
            {
                errMsg = (char *)(err->GetBufferPointer());
                return nullptr;
            }
            return ret;
        }

        static ComPtr<ID3D10Blob> CompileShader(std::string_view source, const char *target, const char *entry, D3D_SHADER_MACRO *macros, std::string &errMsg)
        {
            ComPtr<ID3D10Blob> ret, err;
            HRESULT hr = D3DCompile(
                source.data(), source.size(), nullptr, macros, nullptr,
                entry, target, COMPILER_FLAGS, 0, ret.GetAddressOf(), err.GetAddressOf());
            if(FAILED(hr))
            {
                errMsg = (char *)(err->GetBufferPointer());
                return nullptr;
            }
            return ret;
        }

        static ComPtr<D3DShaderType> CreateShader(void *compiledShader, SIZE_T length)
        {
            assert(compiledShader && length);
            ComPtr<D3DShaderType> shader;
            HRESULT hr = gDevice->CreateVertexShader(compiledShader, length, nullptr, shader.GetAddressOf());
            return FAILED(hr) ? nullptr : shader;
        }

        static void BindShader(D3DShaderType *shader)
        {
            gDeviceContext->VSSetShader(shader, nullptr, 0);
        }
    };

    template<>
    class StageSpecificImpl<ShaderStage::HS>
    {
    public:

        using D3DShaderType = ID3D11HullShader;

        static const char *DefaultCompileTarget() noexcept
        {
            return "hs_5_0";
        }

        static ComPtr<ID3D10Blob> CompileShader(
            std::string_view source, const char *sourceName, const char *target, const char *entry, D3D_SHADER_MACRO *macros, std::string &errMsg)
        {
            return StageSpecificImpl<ShaderStage::VS>::CompileShader(source, sourceName, target, entry, macros, errMsg);
        }

        static ComPtr<ID3D10Blob> CompileShader(std::string_view source, const char *target, const char *entry, D3D_SHADER_MACRO *macros, std::string &errMsg)
        {
            return StageSpecificImpl<ShaderStage::VS>::CompileShader(source, target, entry, macros, errMsg);
        }

        static ComPtr<D3DShaderType> CreateShader(void *compiledShader, SIZE_T length)
        {
            assert(compiledShader && length);
            ComPtr<D3DShaderType> shader;
            HRESULT hr = gDevice->CreateHullShader(compiledShader, length, nullptr, shader.GetAddressOf());
            return FAILED(hr) ? nullptr : shader;
        }

        static void BindShader(D3DShaderType *shader)
        {
            gDeviceContext->HSSetShader(shader, nullptr, 0);
        }
    };

    template<>
    class StageSpecificImpl<ShaderStage::DS>
    {
    public:

        using D3DShaderType = ID3D11DomainShader;

        static const char *DefaultCompileTarget() noexcept
        {
            return "ds_5_0";
        }

        static ComPtr<ID3D10Blob> CompileShader(
            std::string_view source, const char *sourceName, const char *target, const char *entry, D3D_SHADER_MACRO *macros, std::string &errMsg)
        {
            return StageSpecificImpl<ShaderStage::VS>::CompileShader(source, sourceName, target, entry, macros, errMsg);
        }

        static ComPtr<ID3D10Blob> CompileShader(std::string_view source, const char *target, const char *entry, D3D_SHADER_MACRO *macros, std::string &errMsg)
        {
            return StageSpecificImpl<ShaderStage::VS>::CompileShader(source, target, entry, macros, errMsg);
        }

        static ComPtr<D3DShaderType> CreateShader(void *compiledShader, SIZE_T length)
        {
            assert(compiledShader && length);
            ComPtr<D3DShaderType> shader;
            HRESULT hr = gDevice->CreateDomainShader(compiledShader, length, nullptr, shader.GetAddressOf());
            return FAILED(hr) ? nullptr : shader;
        }

        static void BindShader(D3DShaderType *shader)
        {
            gDeviceContext->DSSetShader(shader, nullptr, 0);
        }
    };
    template<>
    class StageSpecificImpl<ShaderStage::GS>
    {
    public:

        using D3DShaderType = ID3D11GeometryShader;

        static const char *DefaultCompileTarget() noexcept
        {
            return "gs_5_0";
        }

        static ComPtr<ID3D10Blob> CompileShader(
            std::string_view source, const char *sourceName, const char *target, const char *entry, D3D_SHADER_MACRO *macros, std::string &errMsg)
        {
            return StageSpecificImpl<ShaderStage::VS>::CompileShader(source, sourceName, target, entry, macros, errMsg);
        }

        static ComPtr<ID3D10Blob> CompileShader(std::string_view source, const char *target, const char *entry, D3D_SHADER_MACRO *macros, std::string &errMsg)
        {
            return StageSpecificImpl<ShaderStage::VS>::CompileShader(source, target, entry, macros, errMsg);
        }

        static ComPtr<D3DShaderType> CreateShader(void *compiledShader, SIZE_T length)
        {
            assert(compiledShader && length);
            ComPtr<D3DShaderType> shader = nullptr;
            HRESULT hr = gDevice->CreateGeometryShader(compiledShader, length, nullptr, shader.GetAddressOf());
            return FAILED(hr) ? nullptr : shader;
        }

        static void BindShader(D3DShaderType *shader)
        {
            gDeviceContext->GSSetShader(shader, nullptr, 0);
        }
    };

    template<>
    class StageSpecificImpl<ShaderStage::PS>
    {
    public:

        using D3DShaderType = ID3D11PixelShader;

        static const char *DefaultCompileTarget() noexcept
        {
            return "ps_5_0";
        }

        static ComPtr<ID3D10Blob> CompileShader(
            std::string_view source, const char *sourceName, const char *target, const char *entry, D3D_SHADER_MACRO *macros, std::string &errMsg)
        {
            return StageSpecificImpl<ShaderStage::VS>::CompileShader(source, sourceName, target, entry, macros, errMsg);
        }

        static ComPtr<ID3D10Blob> CompileShader(std::string_view source, const char *target, const char *entry, D3D_SHADER_MACRO *macros, std::string &errMsg)
        {
            return StageSpecificImpl<ShaderStage::VS>::CompileShader(source, target, entry, macros, errMsg);
        }

        static ComPtr<D3DShaderType> CreateShader(void *compiledShader, SIZE_T length)
        {
            assert(compiledShader && length);
            ComPtr<D3DShaderType> shader;
            HRESULT hr = gDevice->CreatePixelShader(compiledShader, length, nullptr, shader.GetAddressOf());
            return FAILED(hr) ? nullptr : shader;
        }

        static void BindShader(D3DShaderType *shader)
        {
            gDeviceContext->PSSetShader(shader, nullptr, 0);
        }
    };

} // namespace Impl

template<ShaderStage STAGE>
class Stage : public agz::misc::uncopyable_t
{
    ComPtr<typename Impl::StageSpecificImpl<STAGE>::D3DShaderType> shader_;
    ComPtr<ID3D10Blob> byteCode_;

    typename ConstantBufferSlotManager<STAGE>::RecordTable constantBufferTable_;
    typename ShaderResourceSlotManager<STAGE>::RecordTable shaderResourceTable_;
    typename SamplerSlotManager<STAGE>::RecordTable  samplerTable_;

    void InitializeRecordTables()
    {
        ComPtr<ID3D11ShaderReflection> reflection = Reflection::GetShaderReflection(byteCode_.Get());
        if(!reflection)
        {
            throw VRPGBaseException("failed to get shader reflection");
        }

        std::map<std::string, Reflection::ConstantBufferInfo> constantBuffers;
        std::map<std::string, Reflection::ShaderResourceInfo> shaderResources;
        std::map<std::string, Reflection::ShaderSamplerInfo> samplers;
        GetShaderInfo(reflection.Get(), constantBuffers, shaderResources, samplers);

        for(auto &it : constantBuffers)
        {
            constantBufferTable_[it.first] = { ConstantBufferSlot<STAGE>(it.second.slot) };
        }

        auto deleteArraySyntax = [](const std::string &name)
        {
            return name.substr(0, name.find('['));
        };
        for(auto &it : shaderResources)
        {
            auto left = deleteArraySyntax(it.first);
            shaderResourceTable_[left] = { ShaderResourceSlot<STAGE>(it.second.slot, it.second.count) };
        }

        for(auto &it : samplers)
        {
            samplerTable_[it.first] = { SamplerSlot<STAGE>(it.second.slot) };
        }
    }

public:

    using SpecImpl = Impl::StageSpecificImpl<STAGE>;

    static constexpr ShaderStage ShaderStageValue = STAGE;

    void Initialize(std::string_view source, const char *sourceName, const char *entry, const char *target, D3D_SHADER_MACRO *macros)
    {
        agz::misc::scope_guard_t initGuard([&] { this->Destroy(); });

        std::string errMsg;
        byteCode_ = SpecImpl::CompileShader(source, sourceName, target, entry, macros, errMsg);
        if(!byteCode_)
        {
            throw VRPGBaseException("failed to compile shader source: " + errMsg);
        }

        shader_ = SpecImpl::CreateShader(byteCode_->GetBufferPointer(), byteCode_->GetBufferSize());

        InitializeRecordTables();

        initGuard.dismiss();
    }

    void Initialize(std::string_view source, const char *entry, const char *target, D3D_SHADER_MACRO *macros)
    {
        agz::misc::scope_guard_t initGuard([&] { this->Destroy(); });

        std::string errMsg;
        byteCode_ = SpecImpl::CompileShader(source, target, entry, macros, errMsg);
        if(!byteCode_)
        {
            throw VRPGBaseException("failed to compile shader source: " + errMsg);
        }

        shader_ = SpecImpl::CreateShader(byteCode_->GetBufferPointer(), byteCode_->GetBufferSize());

        InitializeRecordTables();

        initGuard.dismiss();
    }

    void Initialize(ID3D10Blob *shaderByteCode)
    {
        assert(!byteCode_);
        agz::misc::scope_guard_t initGuard([&] { this->Destroy(); });

        std::string errMsg;
        byteCode_ = shaderByteCode;
        if(!byteCode_)
        {
            throw VRPGBaseException("failed to compile shader source: " + errMsg);
        }

        shader_ = SpecImpl::CreateShader(byteCode_->GetBufferPointer(), byteCode_->GetBufferSize());

        InitializeRecordTables();

        initGuard.dismiss();
    }

    bool IsAvailable() const noexcept
    {
        assert((shader_ != nullptr) == (byteCode_ != nullptr));
        return shader_ != nullptr;
    }

    void Destroy()
    {
        shader_.Reset();
        byteCode_.Reset();

        constantBufferTable_.clear();
        shaderResourceTable_.clear();
        samplerTable_.clear();
    }

    void Bind() const
    {
        SpecImpl::BindShader(shader_.Get());
    }

    void Unbind() const
    {
        SpecImpl::BindShader(nullptr);
    }

    const typename ConstantBufferSlotManager<STAGE>::RecordTable &GetConstantBufferTable() const noexcept
    {
        return constantBufferTable_;
    }

    const typename ShaderResourceSlotManager<STAGE>::RecordTable &GetShaderResourceTable() const noexcept
    {
        return shaderResourceTable_;
    }

    const typename SamplerSlotManager<STAGE>::RecordTable &GetSamplerTable() const noexcept
    {
        return samplerTable_;
    }

    ID3D10Blob *GetShaderByteCode() const noexcept
    {
        return byteCode_.Get();
    }
};

AGZ_D3D11_END
