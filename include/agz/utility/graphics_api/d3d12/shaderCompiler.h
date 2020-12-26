#pragma once

#include <dxcapi.h>

#include <agz/utility/graphics_api/d3d12/common.h>

AGZ_D3D12_BEGIN

class FXC
{
public:

    enum OptimizationLevel
    {
        Debug, O0, O1, O2, O3
    };

    struct Options
    {
        const D3D_SHADER_MACRO *macros     = nullptr;
        ID3DInclude            *includes   = nullptr;
        const char             *sourceName = nullptr;
        const char             *entry      = "main";
    };

    explicit FXC();

    void setOptimization(OptimizationLevel level) noexcept;

    void setWarnings(bool treatWarningAsError) noexcept;

    ComPtr<ID3D10Blob> compile(
        std::string_view source,
        const char      *target,
        const Options   &options = {});

    const std::string &getWarnings() const noexcept;

private:

    void updateFlag();

    OptimizationLevel optLevel_;
    bool              warningAsError_;
    UINT              flag_;

    std::string warnings_;
};

class DXC
{
public:

    enum OptimizationLevel
    {
        Debug, O0, O1, O2, O3, Auto
    };

    struct Options
    {
        const wchar_t                                     *entry      = nullptr;
        const wchar_t                                     *target     = nullptr;
        const wchar_t                                     *sourceName = nullptr;
        IDxcIncludeHandler                                *includes   = nullptr;
        std::vector<std::pair<std::wstring, std::wstring>> macros;
    };

    explicit DXC();

    void setOptimization(OptimizationLevel level) noexcept;

    void setWarnings(bool treatWarningAsError) noexcept;

    ComPtr<ID3DBlob> compile(
        std::string_view  source,
        const Options    &options);

private:

    OptimizationLevel optLevel_;
    bool              warningAsError_;
};

AGZ_D3D12_END
