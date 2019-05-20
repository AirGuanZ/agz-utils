#pragma once

#include "../math/math.h"

namespace agz::img
{

template<typename P>
using image_buffer = math::tensor_t<P, 2>;

} // namespace agz::img
