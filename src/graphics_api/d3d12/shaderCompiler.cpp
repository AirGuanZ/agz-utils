#ifdef AGZ_ENABLE_D3D12

#include <d3dcompiler.h>

#include <agz/utility/graphics_api/d3d12/shaderCompiler.h>

AGZ_D3D12_BEGIN

ShaderCompiler::ShaderCompiler()
{
    optLevel_       = Debug;
    warningAsError_ = true;
    flag_           = 0;
    updateFlag();
}

void ShaderCompiler::setOptimization(OptimizationLevel level) noexcept
{
    optLevel_ = level;
    updateFlag();
}

void ShaderCompiler::setWarnings(bool treatWarningAsError) noexcept
{
    warningAsError_ = treatWarningAsError;
    updateFlag();
}

ComPtr<ID3D10Blob> ShaderCompiler::compile(
    std::string_view source,
    const char      *target,
    const Options   &options)
{
    ComPtr<ID3D10Blob> byteCode;
    ComPtr<ID3D10Blob> errMsg;

    const HRESULT hr = D3DCompile(
        source.data(),
        source.size(),
        options.sourceName,
        options.macros,
        options.includes,
        options.entry,
        target,
        flag_,
        0,
        byteCode.GetAddressOf(),
        errMsg.GetAddressOf());

    warnings_.clear();

    if(FAILED(hr))
    {
        if(!errMsg)
            throw D3D12Exception("failed to compile d3d shader");

        std::string errStr(
            reinterpret_cast<const char*>(errMsg->GetBufferPointer()));
        throw D3D12Exception(errStr);
    }

    if(errMsg)
        warnings_ = reinterpret_cast<const char *>(errMsg->GetBufferPointer());

    assert(byteCode);
    return byteCode;
}

const std::string &ShaderCompiler::getWarnings() const noexcept
{
    return warnings_;
}

void ShaderCompiler::updateFlag()
{
    switch(optLevel_)
    {
    case Debug:
        flag_ = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
        break;
    case O0:
        flag_ = D3DCOMPILE_OPTIMIZATION_LEVEL0;
        break;
    case O1:
        flag_ = D3DCOMPILE_OPTIMIZATION_LEVEL1;
        break;
    case O2:
        flag_ = D3DCOMPILE_OPTIMIZATION_LEVEL2;
        break;
    case O3:
        flag_ = D3DCOMPILE_OPTIMIZATION_LEVEL3;
        break;
    }

    if(warningAsError_)
        flag_ |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
