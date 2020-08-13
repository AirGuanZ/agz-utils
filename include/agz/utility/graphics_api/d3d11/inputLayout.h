#pragma once

#include "shader.h"

AGZ_D3D11_BEGIN

class InputLayoutBuilder
{
    std::vector<D3D11_INPUT_ELEMENT_DESC> elems_;

public:

    InputLayoutBuilder() = default;

    explicit InputLayoutBuilder(const D3D11_INPUT_ELEMENT_DESC &elem)
        : elems_{ elem }
    {

    }

    template<int N>
    explicit InputLayoutBuilder(const D3D11_INPUT_ELEMENT_DESC (&elems)[N])
        : elems_(std::begin(elems), std::end(elems))
    {
        
    }

    explicit InputLayoutBuilder(
        std::initializer_list<D3D11_INPUT_ELEMENT_DESC> elems)
        : elems_(elems.begin(), elems.end())
    {

    }

    InputLayoutBuilder &add(const D3D11_INPUT_ELEMENT_DESC &elem)
    {
        elems_.push_back(elem);
        return *this;
    }

    template<int N>
    InputLayoutBuilder &add(const D3D11_INPUT_ELEMENT_DESC (&elems)[N])
    {
        elems_.reserve(elems_.size() + N);
        std::copy(
            std::begin(elems), std::end(elems), std::back_inserter(elems_));
        return *this;
    }

    InputLayoutBuilder &add(
        std::initializer_list<D3D11_INPUT_ELEMENT_DESC> elems)
    {
        elems_.reserve(elems_.size() + elems.size());
        std::copy(
            std::begin(elems), std::end(elems), std::back_inserter(elems_));
        return *this;
    }

    ComPtr<ID3D11InputLayout> build(
        const void *vertexShaderByteCode, size_t len) const
    {
        ComPtr<ID3D11InputLayout> ret;
        if(FAILED(gDevice->CreateInputLayout(
            elems_.data(), static_cast<UINT>(elems_.size()),
            vertexShaderByteCode, len, ret.GetAddressOf())))
            throw D3D11Exception("failed to create input layout");

        return ret;
    }

    ComPtr<ID3D11InputLayout> build(
        ComPtr<ID3D10Blob> vertexShaderByteCode) const
    {
        return build(
            vertexShaderByteCode->GetBufferPointer(),
            vertexShaderByteCode->GetBufferSize());
    }

    ComPtr<ID3D11InputLayout> build(
        ID3D10Blob *vertexShaderByteCode) const
    {
        return build(
            vertexShaderByteCode->GetBufferPointer(),
            vertexShaderByteCode->GetBufferSize());
    }

    template<ShaderStage...STAGES>
    ComPtr<ID3D11InputLayout> build(const Shader<STAGES...> &shader) const
    {
        return this->build(shader.getVertexShaderByteCode());
    }
};

AGZ_D3D11_END
