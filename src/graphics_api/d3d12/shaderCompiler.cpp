#ifdef AGZ_ENABLE_D3D12

#include <d3dcompiler.h>

#include <agz-utils/graphics_api/d3d12/shaderCompiler.h>
#include <agz-utils/string.h>

AGZ_D3D12_BEGIN

FXC::FXC()
{
    optLevel_       = Auto;
    warningAsError_ = true;
    flag_           = 0;
    updateFlag();
}

void FXC::setOptimization(OptimizationLevel level) noexcept
{
    optLevel_ = level;
    updateFlag();
}

void FXC::setWarnings(bool treatWarningAsError) noexcept
{
    warningAsError_ = treatWarningAsError;
    updateFlag();
}

ComPtr<ID3D10Blob> FXC::compile(
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

const std::string &FXC::getWarnings() const noexcept
{
    return warnings_;
}

void FXC::updateFlag()
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

DXC::DXC()
    : optLevel_(Auto), warningAsError_(false)
{
    
}

void DXC::setOptimization(OptimizationLevel level) noexcept
{
    optLevel_ = level;
}

void DXC::setWarnings(bool treatWarningAsError) noexcept
{
    warningAsError_ = treatWarningAsError;
}

ComPtr<ID3D10Blob> DXC::compile(
    std::string_view  source,
    const Options    &options)
{
    ComPtr<IDxcLibrary> library;
    AGZ_D3D12_CHECK_HR(
        DxcCreateInstance(
            CLSID_DxcLibrary, IID_PPV_ARGS(library.GetAddressOf())));

    ComPtr<IDxcCompiler> compiler;
    AGZ_D3D12_CHECK_HR(
        DxcCreateInstance(
            CLSID_DxcCompiler, IID_PPV_ARGS(compiler.GetAddressOf())));

    ComPtr<IDxcBlobEncoding> sourceBlob;
    AGZ_D3D12_CHECK_HR(
        library->CreateBlobWithEncodingOnHeapCopy(
            source.data(),
            static_cast<UINT32>(source.size()),
            CP_UTF8, &sourceBlob));

    std::vector<LPCWSTR> args;

    if(warningAsError_)
        args.push_back(L"-WX");

    switch(optLevel_)
    {
#ifdef AGZ_DEBUG
    case Auto:
#endif
    case Debug:
        args.push_back(L"-Zi");
        //args.push_back(L"-Oembed_debug");
        args.push_back(L"-Od");
        break;
    case O0:
        args.push_back(L"-O0");
        break;
    case O1:
        args.push_back(L"-O1");
        break;
    case O2:
        args.push_back(L"-O2");
        break;
#ifndef AGZ_DEBUG
    case Auto:
#endif
    case O3:
        args.push_back(L"-O3");
        break;
    }

    std::vector<DxcDefine> macros;
    for(auto &m : options.macros)
        macros.push_back(DxcDefine{ m.first.c_str(), m.second.c_str() });

    ComPtr<IDxcOperationResult> compileResult;
    AGZ_D3D12_CHECK_HR_MSG(
        "failed to compile d3d shader with dxc",
        compiler->Compile(
            sourceBlob.Get(),
            options.sourceName,
            options.entry,
            options.target,
            args.data(),   static_cast<UINT32>(args.size()),
            macros.data(), static_cast<UINT32>(macros.size()),
            options.includes,
            compileResult.GetAddressOf()));

    HRESULT HRCompile;
    AGZ_D3D12_CHECK_HR_MSG(
        "failed to get dxc result status",
        compileResult->GetStatus(&HRCompile));
    if(FAILED(HRCompile))
    {
        ComPtr<IDxcBlobEncoding> errBlob;
        AGZ_D3D12_CHECK_HR_MSG(
            "failed to get dxc error message",
            compileResult->GetErrorBuffer(errBlob.GetAddressOf()));

        throw D3D12Exception(static_cast<char *>(
                    errBlob->GetBufferPointer()));
    }

    ComPtr<ID3DBlob> result;
    AGZ_D3D12_CHECK_HR_MSG(
        "failed to get dxc output",
        compileResult->GetResult(
            reinterpret_cast<IDxcBlob **>(result.GetAddressOf())));

    return result;
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
