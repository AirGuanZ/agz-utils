#pragma once

#include <vector>

#include <d3d11.h>

#include <agz/utility/d3d11/D3D/Shader.h>

AGZ_D3D11_BEGIN

class InputLayout
{
    ComPtr<ID3D11InputLayout> inputLayout_;

public:

    InputLayout() = default;

    explicit InputLayout(ID3D11InputLayout *initializedInputLayout)
        : inputLayout_(initializedInputLayout)
    {
        
    }

    InputLayout &operator=(ID3D11InputLayout *initializedInputLayout)
    {
        inputLayout_ = initializedInputLayout;
        return *this;
    }

    InputLayout &operator=(ComPtr<ID3D11InputLayout> initializedInputLayout)
    {
        inputLayout_ = std::move(initializedInputLayout);
        return *this;
    }

    void Initialize(
        const D3D11_INPUT_ELEMENT_DESC desc[], size_t descCount,
        ID3D10Blob *shaderByteCode)
    {
        inputLayout_.Reset();
        HRESULT hr = gDevice->CreateInputLayout(desc, UINT(descCount), shaderByteCode->GetBufferPointer(), shaderByteCode->GetBufferSize(), inputLayout_.GetAddressOf());
        if(FAILED(hr))
        {
            throw VRPGBaseException("failed to create input layout");
        }
    }

    bool IsAvailable() const noexcept
    {
        return inputLayout_ != nullptr;
    }

    void Destroy()
    {
        inputLayout_.Reset();
    }

    ID3D11InputLayout *Get() const noexcept
    {
        return inputLayout_.Get();
    }

    operator ID3D11InputLayout*() const noexcept
    {
        return inputLayout_.Get();
    }

    void Bind() const
    {
        gDeviceContext->IASetInputLayout(inputLayout_.Get());
    }

    void Unbind() const
    {
        gDeviceContext->IASetInputLayout(nullptr);
    }
};

class InputLayoutBuilder
{
    std::vector<D3D11_INPUT_ELEMENT_DESC> descs_;
    UINT inputSlot_ = 0;

public:

    InputLayoutBuilder() = default;

    InputLayoutBuilder(
        const char *semantic,
        UINT        semanticIndex,
        DXGI_FORMAT format,
        UINT        byteOffset)
        : InputLayoutBuilder()
    {
        Append(semantic, semanticIndex, format, byteOffset);
    }

    InputLayoutBuilder(
        const char *semantic,
        UINT        semanticIndex,
        DXGI_FORMAT format,
        UINT        byteOffset,
        UINT        instanceDataStepRate)
        : InputLayoutBuilder()
    {
        Append(semantic, semanticIndex, format, byteOffset, instanceDataStepRate);
    }

    InputLayoutBuilder &Append(
        const char *semantic,
        UINT        semanticIndex,
        DXGI_FORMAT format,
        UINT        byteOffset)
    {
        descs_.push_back({
            semantic,
            semanticIndex,
            format,
            inputSlot_,
            byteOffset,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        });
        return *this;
    }

    InputLayoutBuilder &Append(
        const char *semantic,
        UINT        semanticIndex,
        DXGI_FORMAT format,
        UINT        byteOffset,
        UINT        instanceDataStepRate)
    {
        descs_.push_back({
            semantic,
            semanticIndex,
            format,
            inputSlot_,
            byteOffset,
            D3D11_INPUT_PER_INSTANCE_DATA,
            instanceDataStepRate
        });
        return *this;
    }

    InputLayoutBuilder &operator()(
        const char *semantic,
        UINT        semanticIndex,
        DXGI_FORMAT format,
        UINT        byteOffset)
    {
        return Append(semantic, semanticIndex, format, byteOffset);
    }

    InputLayoutBuilder &operator()(
        const char *semantic,
        UINT        semanticIndex,
        DXGI_FORMAT format,
        UINT        byteOffset,
        UINT        instanceDataStepRate)
    {
        return Append(semantic, semanticIndex, format, byteOffset, instanceDataStepRate);
    }

    InputLayoutBuilder &NextInputSlot() noexcept
    {
        ++inputSlot_;
        return *this;
    }

    InputLayoutBuilder &SetInputSlot(UINT inputSlot) noexcept
    {
        inputSlot_ = inputSlot;
        return *this;
    }

    template<ShaderStage...STAGES>
    ComPtr<ID3D11InputLayout> Build(const Shader<STAGES...> &shader) const
    {
        return Build(shader.GetVertexShaderByteCode());
    }

    ComPtr<ID3D11InputLayout> Build(ID3D10Blob *shaderByteCode) const
    {
        ComPtr<ID3D11InputLayout> inputLayout;
        HRESULT hr = gDevice->CreateInputLayout(
            descs_.data(), UINT(descs_.size()),
            shaderByteCode->GetBufferPointer(),
            shaderByteCode->GetBufferSize(),
            inputLayout.GetAddressOf());
        if(FAILED(hr))
        {
            throw VRPGBaseException("failed to create input layout");
        }
        return inputLayout;
    }
};

AGZ_D3D11_END
