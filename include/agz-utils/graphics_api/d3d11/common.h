#pragma once

#include <d3d11.h>
#include <system_error>

#include <agz-utils/math.h>
#include <wrl/client.h>

#define AGZ_D3D11_BEGIN namespace agz::d3d11 {
#define AGZ_D3D11_END   }

AGZ_D3D11_BEGIN

using Float2 = math::vec2f;
using Float3 = math::vec3f;
using Float4 = math::vec4f;

using Color4 = math::color4f;

using Int2 = math::vec2i;
using Int3 = math::vec3i;

using Mat4   = math::mat4f_c;
using Trans4 = Mat4::right_transform;

using Microsoft::WRL::ComPtr;

class D3D11Exception : public std::runtime_error
{
public:

    using runtime_error::runtime_error;
};

#define AGZ_D3D11_DECL_EVENT_MGR_HANDLER(EventMgr, EventName) \
    AGZ_DECL_EVENT_SENDER_HANDLER(EventMgr, EventName)

AGZ_D3D11_END
