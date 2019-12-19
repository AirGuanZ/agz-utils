#pragma once

#include <agz/utility/file.h>
#include <agz/utility/misc.h>

#include <agz/utility/d3d11/D3D/Shader/Stage.h>
#include <agz/utility/d3d11/D3D/Shader/UniformManager.h>

AGZ_D3D11_BEGIN

template<ShaderStage...STAGES>
class Shader : public agz::misc::uncopyable_t
{
    std::tuple<Stage<STAGES>...> stages_;

public:

    Shader() = default;
    ~Shader() = default;

    Shader(Shader<STAGES...> &&)                       noexcept = default;
    Shader<STAGES...> &operator=(Shader<STAGES...> &&) noexcept = default;

    template<ShaderStage STAGE>
    void InitializeStage(
        std::string_view source,
        const std::string &sourceName,
        D3D_SHADER_MACRO *macros = nullptr,
        const char *entry = "main",
        const char *target = Stage<STAGE>::SpecImpl::DefaultCompileTarget())
    {
        auto &stage = std::get<Stage<STAGE>>(stages_);
        stage.Initialize(source, sourceName.c_str(), entry, target, macros);
    }

    template<ShaderStage STAGE>
    void InitializeStage(
        std::string_view source,
        const char *sourceName,
        D3D_SHADER_MACRO *macros = nullptr,
        const char *entry = "main",
        const char *target = Stage<STAGE>::SpecImpl::DefaultCompileTarget())
    {
        auto &stage = std::get<Stage<STAGE>>(stages_);
        stage.Initialize(source, sourceName, entry, target, macros);
    }

    template<ShaderStage STAGE>
    void InitializeStageFromFile(
        const std::string &filename,
        D3D_SHADER_MACRO *macros = nullptr,
        const char *entry = "main",
        const char *target = Stage<STAGE>::SpecImpl::DefaultCompileTarget())
    {
        auto source = agz::file::read_txt_file(filename);
        auto &stage = std::get<Stage<STAGE>>(stages_);
        stage.Initialize(source, filename.c_str(), entry, target, macros);
    }

    template<ShaderStage STAGE>
    void InitializeStage(
        std::string_view source,
        D3D_SHADER_MACRO *macros = nullptr,
        const char *entry = "main",
        const char *target = Stage<STAGE>::SpecImpl::DefaultCompileTarget())
    {
        auto &stage = std::get<Stage<STAGE>>(stages_);
        stage.Initialize(source, entry, target, macros);
    }

    template<ShaderStage STAGE>
    void InitializeStage(ID3D10Blob *shaderByteCode)
    {
        auto &stage = std::get<Stage<STAGE>>(stages_);
        stage.Initialize(shaderByteCode);
    }

    template<ShaderStage STAGE>
    void DestroyStage()
    {
        auto &stage = std::get<Stage<STAGE>>(stages_);
        stage.Destroy();
    }

    void Destroy()
    {
        std::apply(
            [](auto &...stage) { ((stage.Destroy()), ...); },
            stages_);
    }

    bool IsAllStagesAvailable() const noexcept
    {
        bool ret = true;
        std::apply(
            [&](auto &...stage) { ((ret &= stage.IsAvailable()), ...); },
            stages_);
        return ret;
    }

    template<ShaderStage STAGE>
    Stage<STAGE> *GetStage() noexcept
    {
        return &std::get<Stage<STAGE>>(stages_);
    }

    template<ShaderStage STAGE>
    const Stage<STAGE> *GetStage() const noexcept
    {
        return &std::get<Stage<STAGE>>(stages_);
    }

    UniformManager<STAGES...> CreateUniformManager() const
    {
        return agz::misc::construct_from_tuple<UniformManager<STAGES...>>(stages_);
    }

    ID3D10Blob *GetVertexShaderByteCode() const noexcept
    {
        return GetStage<ShaderStage::VS>()->GetShaderByteCode();
    }

    void Bind() const
    {
        std::apply(
            [](const auto &...stage) { ((stage.Bind()), ...); },
            stages_);
    }

    void Unbind() const
    {
        std::apply(
            [](const auto &...stage) { ((stage.Unbind()), ...); },
            stages_);
    }
};

AGZ_D3D11_END
