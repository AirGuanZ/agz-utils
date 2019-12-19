#pragma once

#include <stdexcept>

#include <agz/utility/math.h>
#include <wrl/client.h>
#include <d3d11.h>

#define AGZ_D3D11_BEGIN namespace agz::d3d11 {
#define AGZ_D3D11_END   }

AGZ_D3D11_BEGIN

using Vec2 = agz::math::vec2f;
using Vec3 = agz::math::vec3f;
using Vec4 = agz::math::vec4f;

using Vec2i = agz::math::vec2i;
using Vec3i = agz::math::vec3i;
using Vec4i = agz::math::vec4i;

using Vec3b = agz::math::vec3b;
using Vec4b = agz::math::vec4b;

using Mat3 = agz::math::mat3f_c;
using Mat4 = agz::math::mat4f_c;

using Trans4 = Mat4::right_transform;

using Quaternion = agz::math::tquaternion_t<float>;

class VRPGBaseException : public std::runtime_error
{
public:

    using runtime_error::runtime_error;
};

using Microsoft::WRL::ComPtr;

template<typename T, typename = std::enable_if_t<std::is_pointer_v<std::remove_reference_t<T>>>>
void ReleaseCOMObjects(T &&ptr)
{
    if(ptr)
    {
        ptr->Release();
        ptr = nullptr;
    }
}

template<typename T0, typename T1, typename...Ts>
void ReleaseCOMObjects(T0 &&ptr0, T1 &&ptr1, Ts&&...ptrs)
{
    ReleaseCOMObjects(std::forward<T0>(ptr0));
    ReleaseCOMObjects(std::forward<T1>(ptr1), std::forward<Ts>(ptrs)...);
}

inline ID3D11Device        *gDevice        = nullptr;
inline ID3D11DeviceContext *gDeviceContext = nullptr;

class Window;

AGZ_D3D11_END
