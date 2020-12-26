﻿#pragma once

#include "decl/aabb2.h"
#include "decl/aabb3.h"
#include "decl/collision.h"
#include "decl/color2.h"
#include "decl/color3.h"
#include "decl/color4.h"
#include "decl/coord.h"
#include "decl/distribution.h"
#include "decl/func.h"
#include "decl/low_discrepancy.h"
#include "decl/mat3_c.h"
#include "decl/mat4_c.h"
#include "decl/quaternion.h"
#include "decl/rng.h"
#include "decl/transform2.h"
#include "decl/transform3.h"
#include "decl/variance.h"
#include "decl/vec.h"
#include "decl/vec2.h"
#include "decl/vec3.h"
#include "decl/vec4.h"

#ifndef __CUDACC__

#include "decl/spherical_harmonics.h"
#include "decl/tensor.h"
#include "decl/tensor_view.h"

#include "decl/simd/float3.h"
#include "decl/simd/float4.h"
#include "decl/simd/float4x4.h"
#include "decl/simd/coord_float3.h"
#include "decl/simd/transform_float3.h"

#endif

#include "impl/aabb2.inl"
#include "impl/aabb3.inl"
#include "impl/collision.inl"
#include "impl/color2.inl"
#include "impl/color3.inl"
#include "impl/color4.inl"
#include "impl/coord.inl"
#include "impl/distribution.inl"
#include "impl/mat3_c.inl"
#include "impl/mat4_c.inl"
#include "impl/quaternion.inl"
#include "impl/transform2.inl"
#include "impl/transform3.inl"
#include "impl/vec.inl"
#include "impl/vec2.inl"
#include "impl/vec3.inl"
#include "impl/vec4.inl"
#include "impl/low_discrepancy.inl"

#ifndef __CUDACC__

#include "impl/spherical_harmonics.inl"
#include "impl/tensor.inl"
#include "impl/tensor_view.inl"

#include "impl/simd/float3.inl"
#include "impl/simd/float4.inl"
#include "impl/simd/float4x4.inl"
#include "impl/simd/coord_float3.inl"
#include "impl/simd/transform_float3.inl"

#endif
