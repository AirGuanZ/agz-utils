#pragma once

#include <string>
#include <system_error>

#include <d3d12.h>
#include <dxgi.h>

#include <agz/utility/math.h>
#include <wrl/client.h>

#define AGZ_D3D12_BEGIN namespace agz::d3d12 {
#define AGZ_D3D12_END   }

AGZ_D3D12_BEGIN

using Float2 = math::vec2f;
using Float3 = math::vec3f;
using Float4 = math::vec4f;

using Int2 = math::vec2i;
using Int3 = math::vec3i;

using Mat4   = math::mat4f_c;
using Trans4 = Mat4::right_transform;

using Microsoft::WRL::ComPtr;

class D3D12Exception : public std::runtime_error
{
public:

    using runtime_error::runtime_error;
};

struct ShaderRegister { UINT registerSpace; UINT registerIndex; };

constexpr ShaderRegister b0 = { 0, 0 };
constexpr ShaderRegister b1 = { 0, 1 };
constexpr ShaderRegister b2 = { 0, 2 };
constexpr ShaderRegister b3 = { 0, 3 };
constexpr ShaderRegister b4 = { 0, 4 };
constexpr ShaderRegister b5 = { 0, 5 };

constexpr ShaderRegister t0 = { 0, 0 };
constexpr ShaderRegister t1 = { 0, 1 };
constexpr ShaderRegister t2 = { 0, 2 };
constexpr ShaderRegister t3 = { 0, 3 };
constexpr ShaderRegister t4 = { 0, 4 };
constexpr ShaderRegister t5 = { 0, 5 };

constexpr ShaderRegister s0 = { 0, 0 };
constexpr ShaderRegister s1 = { 0, 1 };
constexpr ShaderRegister s2 = { 0, 2 };
constexpr ShaderRegister s3 = { 0, 3 };
constexpr ShaderRegister s4 = { 0, 4 };
constexpr ShaderRegister s5 = { 0, 5 };

constexpr ShaderRegister u0 = { 0, 0 };
constexpr ShaderRegister u1 = { 0, 1 };
constexpr ShaderRegister u2 = { 0, 2 };
constexpr ShaderRegister u3 = { 0, 3 };
constexpr ShaderRegister u4 = { 0, 4 };
constexpr ShaderRegister u5 = { 0, 5 };

#define AGZ_D3D12_CHECK_HR(X)                                                   \
    do {                                                                        \
        const HRESULT autoname_hr = X;                                          \
        if(!SUCCEEDED(autoname_hr))                                             \
        {                                                                       \
            throw ::agz::d3d12::D3D12Exception(                                 \
                    std::string("errcode = ")                                   \
                    + std::to_string(autoname_hr) + "."                         \
                    + std::system_category().message(autoname_hr));             \
        }                                                                       \
    } while(false)

#define AGZ_D3D12_CHECK_HR_MSG(E, X)                                            \
    do {                                                                        \
        const HRESULT autoname_hr = X;                                          \
        if(!SUCCEEDED(autoname_hr))                                             \
        {                                                                       \
            throw ::agz::d3d12::D3D12Exception((E) + std::string(".errcode = ") \
                    + std::to_string(autoname_hr) + "."                         \
                    + std::system_category().message(autoname_hr));             \
        }                                                                       \
    } while(false)

AGZ_D3D12_END
