#pragma once

#include <agz/utility/math.h>

#define AGZ_GL_BEGIN namespace agz::gl {
#define AGZ_GL_END   }

AGZ_GL_BEGIN

using vec2 = math::vec2f;
using vec3 = math::vec3f;
using vec4 = math::vec4f;

using int2 = math::vec2i;
using int3 = math::vec3i;

using mat4   = math::mat4f_c;
using trans4 = mat4::left_transform;

using color4 = math::color4f;

class gl_exception_t : public std::runtime_error
{
public:

    using runtime_error::runtime_error;
};

#define AGZ_GL_DECL_EVENT_MGR_HANDLER(EventMgr, EventName)                     \
    void attach(event::receiver_t<EventName> *handler)                         \
        { EventMgr.attach(handler); }                                          \
    void attach(std::shared_ptr<event::receiver_t<EventName>> handler)         \
        { EventMgr.attach(handler); }                                          \
    void detach(event::receiver_t<EventName> *handler)                         \
        { EventMgr.detach(handler); }                                          \
    void detach(std::shared_ptr<event::receiver_t<EventName>> handler)         \
        { EventMgr.detach(handler); }

AGZ_GL_END