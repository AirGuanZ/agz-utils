#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/rootSignature.h>

AGZ_D3D12_BEGIN

RootSignatureBuilder::RootSignatureBuilder()
    : RootSignatureBuilder(nullptr)
{
    
}

RootSignatureBuilder::RootSignatureBuilder(ID3D12Device *device)
    : device_(device), flags_(D3D12_ROOT_SIGNATURE_FLAG_NONE)
{
    
}

void RootSignatureBuilder::addParameter(const D3D12_ROOT_PARAMETER &parameter)
{
    assert(device_);
    parameters_.push_back(parameter);
}

void RootSignatureBuilder::addParameter(
    const D3D12_ROOT_DESCRIPTOR_TABLE &table,
    D3D12_SHADER_VISIBILITY            vis)
{
    addParameter(D3D12_ROOT_PARAMETER{
        .ParameterType    = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
        .DescriptorTable  = table,
        .ShaderVisibility = vis
    });
}

void RootSignatureBuilder::addParameter(
    const D3D12_ROOT_CONSTANTS &consts,
    D3D12_SHADER_VISIBILITY     vis)
{
    addParameter(D3D12_ROOT_PARAMETER{
        .ParameterType    = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
        .Constants        = consts,
        .ShaderVisibility = vis
    });
}

void RootSignatureBuilder::addParameterCBV(
    const ShaderRegister   &reg,
    D3D12_SHADER_VISIBILITY vis)
{
    const D3D12_ROOT_PARAMETER param = {
        .ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV,
        .Descriptor    = D3D12_ROOT_DESCRIPTOR{
            .ShaderRegister = reg.registerIndex,
            .RegisterSpace  = reg.registerSpace
        },
        .ShaderVisibility = vis
    };
    addParameter(param);
}

void RootSignatureBuilder::addParameterSRV(
    const ShaderRegister   &reg,
    D3D12_SHADER_VISIBILITY vis)
{
    const D3D12_ROOT_PARAMETER param = {
        .ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV,
        .Descriptor    = D3D12_ROOT_DESCRIPTOR{
            .ShaderRegister = reg.registerIndex,
            .RegisterSpace  = reg.registerSpace
        },
        .ShaderVisibility = vis
    };
    addParameter(param);
}

void RootSignatureBuilder::addParameterUAV(
    const ShaderRegister   &reg,
    D3D12_SHADER_VISIBILITY vis)
{
    const D3D12_ROOT_PARAMETER param = {
        .ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV,
        .Descriptor    = D3D12_ROOT_DESCRIPTOR{
            .ShaderRegister = reg.registerIndex,
            .RegisterSpace  = reg.registerSpace
        },
        .ShaderVisibility = vis
    };
    addParameter(param);
}

void RootSignatureBuilder::addStaticSampler(
    const D3D12_STATIC_SAMPLER_DESC &staticSampler)
{
    assert(device_);
    staticSamplers_.push_back(staticSampler);
}

void RootSignatureBuilder::addStaticSampler(
    ShaderRegister             reg,
    D3D12_SHADER_VISIBILITY    vis,
    D3D12_FILTER               filter,
    D3D12_TEXTURE_ADDRESS_MODE addressU,
    D3D12_TEXTURE_ADDRESS_MODE addressV,
    D3D12_TEXTURE_ADDRESS_MODE addressW)
{
    const D3D12_STATIC_SAMPLER_DESC desc = {
        .Filter           = filter,
        .AddressU         = addressU,
        .AddressV         = addressV,
        .AddressW         = addressW,
        .MipLODBias       = 0,
        .MaxAnisotropy    = 1,
        .ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER,
        .BorderColor      = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
        .MinLOD           = 0,
        .MaxLOD           = D3D12_FLOAT32_MAX,
        .ShaderRegister   = reg.registerIndex,
        .RegisterSpace    = reg.registerSpace,
        .ShaderVisibility = vis
    };

    addStaticSampler(desc);
}

void RootSignatureBuilder::addFlags(D3D12_ROOT_SIGNATURE_FLAGS flags)
{
    assert(device_);
    flags_ |= flags;
}

ComPtr<ID3D12RootSignature> RootSignatureBuilder::build() const
{
    assert(device_);

    D3D12_ROOT_SIGNATURE_DESC desc;
    desc.NumParameters     = static_cast<UINT>(parameters_.size());
    desc.pParameters       = parameters_.data();
    desc.NumStaticSamplers = static_cast<UINT>(staticSamplers_.size());
    desc.pStaticSamplers   = staticSamplers_.data();
    desc.Flags             = flags_;

    ComPtr<ID3D10Blob> serialized, err;
    if(FAILED(D3D12SerializeRootSignature(
        &desc, D3D_ROOT_SIGNATURE_VERSION_1_0,
        serialized.GetAddressOf(), err.GetAddressOf())))
    {
        throw D3D12Exception(
            "failed to serilize root signature. err: " +
            std::string(reinterpret_cast<const char *>(err->GetBufferPointer())));
    }

    ComPtr<ID3D12RootSignature> ret;
    AGZ_D3D12_CHECK_HR_MSG(
        "failed to create root signature",
        device_->CreateRootSignature(
            0,
            serialized->GetBufferPointer(),
            serialized->GetBufferSize(),
            IID_PPV_ARGS(ret.GetAddressOf())));

    return ret;
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
