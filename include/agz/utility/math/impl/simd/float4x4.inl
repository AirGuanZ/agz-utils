#pragma once

#ifdef AGZ_UTILS_SSE

#include <immintrin.h>

namespace agz::math
{

inline _simd_float4x4_c_t::_simd_float4x4_c_t(
    const _simd_float4_t &c0,
    const _simd_float4_t &c1,
    const _simd_float4_t &c2,
    const _simd_float4_t &c3) noexcept
    : data{ c0, c1, c2, c3 }
{
    
}

inline _simd_float4x4_c_t::_simd_float4x4_c_t() noexcept
    : _simd_float4x4_c_t(identity())
{
    
}

inline _simd_float4x4_c_t::_simd_float4x4_c_t(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33) noexcept
    : data{ col_t(m00, m10, m20, m30),
            col_t(m01, m11, m21, m31),
            col_t(m02, m12, m22, m32),
            col_t(m03, m13, m23, m33) }
{
    
}

inline _simd_float4x4_c_t::_simd_float4x4_c_t(uninitialized_t) noexcept
	: data{ col_t(UNINIT), col_t(UNINIT), col_t(UNINIT), col_t(UNINIT) }
{
    
}

inline _simd_float4x4_c_t _simd_float4x4_c_t::from_rows(
    const row_t &r0, const row_t &r1, const row_t &r2, const row_t &r3) noexcept
{
    return self_t(
        r0.x, r0.y, r0.z, r0.w,
        r1.x, r1.y, r1.z, r1.w,
        r2.x, r2.y, r2.z, r2.w,
        r3.x, r3.y, r3.z, r3.w);
}

inline _simd_float4x4_c_t _simd_float4x4_c_t::from_cols(
    const col_t &c0, const col_t &c1, const col_t &c2, const col_t &c3) noexcept
{
    return _simd_float4x4_c_t(c0, c1, c2, c3);
}

inline _simd_float4x4_c_t _simd_float4x4_c_t::diag(float val) noexcept
{
    return self_t(
        val, 0,   0,   0,
        0,   val, 0,   0,
        0,   0,   val, 0,
        0,   0,   0,   val);
}

inline _simd_float4x4_c_t _simd_float4x4_c_t::all(float val) noexcept
{
    return self_t(col_t(val), col_t(val), col_t(val), col_t(val));
}

inline const _simd_float4x4_c_t::self_t &_simd_float4x4_c_t::zero() noexcept
{
    static const self_t ret = all(0);
    return ret;
}

inline const _simd_float4x4_c_t::self_t &_simd_float4x4_c_t::one() noexcept
{
    static const self_t ret = all(1);
    return ret;
}

inline const _simd_float4x4_c_t::self_t &_simd_float4x4_c_t::identity() noexcept
{
    static const self_t ret = diag(1);
    return ret;
}

inline _simd_float4x4_c_t::col_t &_simd_float4x4_c_t::operator[](size_t idx) noexcept
{
    return data[idx];
}

inline const _simd_float4x4_c_t::col_t &_simd_float4x4_c_t::operator[](size_t idx) const noexcept
{
    return data[idx];
}

inline float &_simd_float4x4_c_t::operator()(size_t row, size_t col) noexcept
{
    return data[col][row];
}

inline float _simd_float4x4_c_t::operator()(size_t row, size_t col) const noexcept
{
    return data[col][row];
}

inline const _simd_float4x4_c_t::row_t _simd_float4x4_c_t::get_row(size_t idx) const noexcept
{
    return row_t(data[0][idx], data[1][idx], data[2][idx], data[3][idx]);
}

/* det & inv reference: glm library
================================================================================
OpenGL Mathematics(GLM)
--------------------------------------------------------------------------------
GLM is licensed under The Happy Bunny License or MIT License

================================================================================
The Happy Bunny License(Modified MIT License)
--------------------------------------------------------------------------------
Copyright(c) 2005 - G - Truc Creation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software andassociated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, andto permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice andthis permission notice shall be included in
all copies or substantial portions of the Software.

Restrictions :
    By making use of the Software for military purposes, you choose to make a
    Bunny unhappy.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.

================================================================================
The MIT License
--------------------------------------------------------------------------------
Copyright(c) 2005 - G - Truc Creation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software andassociated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, andto permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice andthis permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.*/

inline float _simd_float4x4_c_t::det() const noexcept
{
    __m128 Swp2A = _mm_shuffle_ps(data[2], data[2], _MM_SHUFFLE(0, 1, 1, 2));
    __m128 Swp3A = _mm_shuffle_ps(data[3], data[3], _MM_SHUFFLE(3, 2, 3, 3));
    __m128 MulA = _mm_mul_ps(Swp2A, Swp3A);

    __m128 Swp2B = _mm_shuffle_ps(data[2], data[2], _MM_SHUFFLE(3, 2, 3, 3));
    __m128 Swp3B = _mm_shuffle_ps(data[3], data[3], _MM_SHUFFLE(0, 1, 1, 2));
    __m128 MulB = _mm_mul_ps(Swp2B, Swp3B);

    __m128 SubE = _mm_sub_ps(MulA, MulB);

    __m128 Swp2C = _mm_shuffle_ps(data[2], data[2], _MM_SHUFFLE(0, 0, 1, 2));
    __m128 Swp3C = _mm_shuffle_ps(data[3], data[3], _MM_SHUFFLE(1, 2, 0, 0));
    __m128 MulC = _mm_mul_ps(Swp2C, Swp3C);
    __m128 SubF = _mm_sub_ps(_mm_movehl_ps(MulC, MulC), MulC);

    __m128 SubFacA = _mm_shuffle_ps(SubE, SubE, _MM_SHUFFLE(2, 1, 0, 0));
    __m128 SwpFacA = _mm_shuffle_ps(data[1], data[1], _MM_SHUFFLE(0, 0, 0, 1));
    __m128 MulFacA = _mm_mul_ps(SwpFacA, SubFacA);

    __m128 SubTmpB = _mm_shuffle_ps(SubE, SubF, _MM_SHUFFLE(0, 0, 3, 1));
    __m128 SubFacB = _mm_shuffle_ps(SubTmpB, SubTmpB, _MM_SHUFFLE(3, 1, 1, 0));
    __m128 SwpFacB = _mm_shuffle_ps(data[1], data[1], _MM_SHUFFLE(1, 1, 2, 2));
    __m128 MulFacB = _mm_mul_ps(SwpFacB, SubFacB);

    __m128 SubRes = _mm_sub_ps(MulFacA, MulFacB);

    __m128 SubTmpC = _mm_shuffle_ps(SubE, SubF, _MM_SHUFFLE(1, 0, 2, 2));
    __m128 SubFacC = _mm_shuffle_ps(SubTmpC, SubTmpC, _MM_SHUFFLE(3, 3, 2, 0));
    __m128 SwpFacC = _mm_shuffle_ps(data[1], data[1], _MM_SHUFFLE(2, 3, 3, 3));
    __m128 MulFacC = _mm_mul_ps(SwpFacC, SubFacC);

    __m128 AddRes = _mm_add_ps(SubRes, MulFacC);
    __m128 DetCof = _mm_mul_ps(AddRes, _mm_setr_ps(1.0f, -1.0f, 1.0f, -1.0f));

    return dot(data[0], _simd_float4_t(DetCof));
}

inline float _simd_float4x4_c_t::determinant() const noexcept
{
    return det();
}

inline _simd_float4x4_c_t _simd_float4x4_c_t::inv() const noexcept
{
	auto &in = data;

	__m128 Fac0;
	{
		__m128 Swp0a = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(3, 3, 3, 3));
		__m128 Swp0b = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(2, 2, 2, 2));

		__m128 Swp00 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(2, 2, 2, 2));
		__m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0));
		__m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0));
		__m128 Swp03 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(3, 3, 3, 3));

		__m128 Mul00 = _mm_mul_ps(Swp00, Swp01);
		__m128 Mul01 = _mm_mul_ps(Swp02, Swp03);
		Fac0 = _mm_sub_ps(Mul00, Mul01);
	}

	__m128 Fac1;
	{
		__m128 Swp0a = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(3, 3, 3, 3));
		__m128 Swp0b = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(1, 1, 1, 1));

		__m128 Swp00 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(1, 1, 1, 1));
		__m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0));
		__m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0));
		__m128 Swp03 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(3, 3, 3, 3));

		__m128 Mul00 = _mm_mul_ps(Swp00, Swp01);
		__m128 Mul01 = _mm_mul_ps(Swp02, Swp03);
		Fac1 = _mm_sub_ps(Mul00, Mul01);
	}


	__m128 Fac2;
	{
		__m128 Swp0a = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(2, 2, 2, 2));
		__m128 Swp0b = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(1, 1, 1, 1));

		__m128 Swp00 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(1, 1, 1, 1));
		__m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0));
		__m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0));
		__m128 Swp03 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(2, 2, 2, 2));

		__m128 Mul00 = _mm_mul_ps(Swp00, Swp01);
		__m128 Mul01 = _mm_mul_ps(Swp02, Swp03);
		Fac2 = _mm_sub_ps(Mul00, Mul01);
	}

	__m128 Fac3;
	{
		__m128 Swp0a = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(3, 3, 3, 3));
		__m128 Swp0b = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(0, 0, 0, 0));

		__m128 Swp00 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(0, 0, 0, 0));
		__m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0));
		__m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0));
		__m128 Swp03 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(3, 3, 3, 3));

		__m128 Mul00 = _mm_mul_ps(Swp00, Swp01);
		__m128 Mul01 = _mm_mul_ps(Swp02, Swp03);
		Fac3 = _mm_sub_ps(Mul00, Mul01);
	}

	__m128 Fac4;
	{
		__m128 Swp0a = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(2, 2, 2, 2));
		__m128 Swp0b = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(0, 0, 0, 0));

		__m128 Swp00 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(0, 0, 0, 0));
		__m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0));
		__m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0));
		__m128 Swp03 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(2, 2, 2, 2));

		__m128 Mul00 = _mm_mul_ps(Swp00, Swp01);
		__m128 Mul01 = _mm_mul_ps(Swp02, Swp03);
		Fac4 = _mm_sub_ps(Mul00, Mul01);
	}

	__m128 Fac5;
	{
		__m128 Swp0a = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(1, 1, 1, 1));
		__m128 Swp0b = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(0, 0, 0, 0));

		__m128 Swp00 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(0, 0, 0, 0));
		__m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0));
		__m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0));
		__m128 Swp03 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(1, 1, 1, 1));

		__m128 Mul00 = _mm_mul_ps(Swp00, Swp01);
		__m128 Mul01 = _mm_mul_ps(Swp02, Swp03);
		Fac5 = _mm_sub_ps(Mul00, Mul01);
	}

	__m128 SignA = _mm_set_ps(1.0f, -1.0f, 1.0f, -1.0f);
	__m128 SignB = _mm_set_ps(-1.0f, 1.0f, -1.0f, 1.0f);

	__m128 Temp0 = _mm_shuffle_ps(in[1], in[0], _MM_SHUFFLE(0, 0, 0, 0));
	__m128 Vec0 = _mm_shuffle_ps(Temp0, Temp0, _MM_SHUFFLE(2, 2, 2, 0));

	__m128 Temp1 = _mm_shuffle_ps(in[1], in[0], _MM_SHUFFLE(1, 1, 1, 1));
	__m128 Vec1 = _mm_shuffle_ps(Temp1, Temp1, _MM_SHUFFLE(2, 2, 2, 0));

	__m128 Temp2 = _mm_shuffle_ps(in[1], in[0], _MM_SHUFFLE(2, 2, 2, 2));
	__m128 Vec2 = _mm_shuffle_ps(Temp2, Temp2, _MM_SHUFFLE(2, 2, 2, 0));

	__m128 Temp3 = _mm_shuffle_ps(in[1], in[0], _MM_SHUFFLE(3, 3, 3, 3));
	__m128 Vec3 = _mm_shuffle_ps(Temp3, Temp3, _MM_SHUFFLE(2, 2, 2, 0));

	__m128 Mul00 = _mm_mul_ps(Vec1, Fac0);
	__m128 Mul01 = _mm_mul_ps(Vec2, Fac1);
	__m128 Mul02 = _mm_mul_ps(Vec3, Fac2);
	__m128 Sub00 = _mm_sub_ps(Mul00, Mul01);
	__m128 Add00 = _mm_add_ps(Sub00, Mul02);
	__m128 Inv0 = _mm_mul_ps(SignB, Add00);

	__m128 Mul03 = _mm_mul_ps(Vec0, Fac0);
	__m128 Mul04 = _mm_mul_ps(Vec2, Fac3);
	__m128 Mul05 = _mm_mul_ps(Vec3, Fac4);
	__m128 Sub01 = _mm_sub_ps(Mul03, Mul04);
	__m128 Add01 = _mm_add_ps(Sub01, Mul05);
	__m128 Inv1 = _mm_mul_ps(SignA, Add01);

	__m128 Mul06 = _mm_mul_ps(Vec0, Fac1);
	__m128 Mul07 = _mm_mul_ps(Vec1, Fac3);
	__m128 Mul08 = _mm_mul_ps(Vec3, Fac5);
	__m128 Sub02 = _mm_sub_ps(Mul06, Mul07);
	__m128 Add02 = _mm_add_ps(Sub02, Mul08);
	__m128 Inv2 = _mm_mul_ps(SignB, Add02);

	__m128 Mul09 = _mm_mul_ps(Vec0, Fac2);
	__m128 Mul10 = _mm_mul_ps(Vec1, Fac4);
	__m128 Mul11 = _mm_mul_ps(Vec2, Fac5);
	__m128 Sub03 = _mm_sub_ps(Mul09, Mul10);
	__m128 Add03 = _mm_add_ps(Sub03, Mul11);
	__m128 Inv3 = _mm_mul_ps(SignA, Add03);

	__m128 Row0 = _mm_shuffle_ps(Inv0, Inv1, _MM_SHUFFLE(0, 0, 0, 0));
	__m128 Row1 = _mm_shuffle_ps(Inv2, Inv3, _MM_SHUFFLE(0, 0, 0, 0));
	__m128 Row2 = _mm_shuffle_ps(Row0, Row1, _MM_SHUFFLE(2, 0, 2, 0));

	__m128 Det0 = _mm_set_ps1(dot(in[0], _simd_float4_t(Row2)));
	__m128 Rcp0 = _mm_div_ps(_mm_set1_ps(1.0f), Det0);

	self_t ret(UNINIT);
	ret[0] = _mm_mul_ps(Inv0, Rcp0);
	ret[1] = _mm_mul_ps(Inv1, Rcp0);
	ret[2] = _mm_mul_ps(Inv2, Rcp0);
	ret[3] = _mm_mul_ps(Inv3, Rcp0);

	return ret;
}

inline _simd_float4x4_c_t _simd_float4x4_c_t::inverse() const noexcept
{
	return inv();
}

inline _simd_float4x4_c_t _simd_float4x4_c_t::t() const noexcept
{
	__m128 tmp0 = _mm_shuffle_ps(data[0], data[1], 0x44);
	__m128 tmp2 = _mm_shuffle_ps(data[0], data[1], 0xEE);
	__m128 tmp1 = _mm_shuffle_ps(data[2], data[3], 0x44);
	__m128 tmp3 = _mm_shuffle_ps(data[2], data[3], 0xEE);

	self_t ret(UNINIT);
	ret[0] = _mm_shuffle_ps(tmp0, tmp1, 0x88);
	ret[1] = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
	ret[2] = _mm_shuffle_ps(tmp2, tmp3, 0x88);
	ret[3] = _mm_shuffle_ps(tmp2, tmp3, 0xDD);

	return ret;
}

inline _simd_float4x4_c_t _simd_float4x4_c_t::transpose() const noexcept
{
	return t();
}

inline _simd_float4x4_c_t::self_t _simd_float4x4_c_t::left_transform::translate(
	const float3 &offset) noexcept
{
	return translate(offset.x, offset.y, offset.z);
}

inline _simd_float4x4_c_t::self_t _simd_float4x4_c_t::left_transform::translate(
	float x, float y, float z) noexcept
{
	return self_t(
		1, 0, 0, x,
		0, 1, 0, y,
		0, 0, 1, z,
		0, 0, 0, 1);
}

inline _simd_float4x4_c_t::self_t _simd_float4x4_c_t::left_transform::rotate(
	const float3 &_axis, float rad) noexcept
{
	const auto axis = _axis.normalize();
	const float sinv = std::sin(rad), cosv = std::cos(rad);

	self_t ret(UNINIT);

	ret[0][0] = axis.x * axis.x + (1 - axis.x * axis.x) * cosv;
	ret[0][1] = axis.x * axis.y * (1 - cosv) - axis.z * sinv;
	ret[0][2] = axis.x * axis.z * (1 - cosv) + axis.y * sinv;
	ret[0][3] = 0;

	ret[1][0] = axis.x * axis.y * (1 - cosv) + axis.z * sinv;
	ret[1][1] = axis.y * axis.y + (1 - axis.y * axis.y) * cosv;
	ret[1][2] = axis.y * axis.z * (1 - cosv) - axis.x * sinv;
	ret[1][3] = 0;

	ret[2][0] = axis.x * axis.z * (1 - cosv) - axis.y * sinv;
	ret[2][1] = axis.y * axis.z * (1 - cosv) + axis.x * sinv;
	ret[2][2] = axis.z * axis.z + (1 - axis.z * axis.z) * cosv;
	ret[2][3] = 0;

	ret[3][0] = 0;
	ret[3][1] = 0;
	ret[3][2] = 0;
	ret[3][3] = 1;

	return ret;
}

inline _simd_float4x4_c_t::self_t _simd_float4x4_c_t::left_transform::rotate_x(
	float rad) noexcept
{
	const auto S = std::sin(rad), C = std::cos(rad);
	return self_t(
		1, 0, 0, 0,
		0, C, -S, 0,
		0, S, C, 0,
		0, 0, 0, 1);
}

inline _simd_float4x4_c_t::self_t _simd_float4x4_c_t::left_transform::rotate_y(
	float rad) noexcept
{
	const auto S = std::sin(rad), C = std::cos(rad);
	return self_t(
		C, 0, S, 0,
		0, 1, 0, 0,
		-S, 0, C, 0,
		0, 0, 0, 1);
}

inline _simd_float4x4_c_t::self_t _simd_float4x4_c_t::left_transform::rotate_z(
	float rad) noexcept
{
	const auto S = std::sin(rad), C = std::cos(rad);
	return self_t(
		C, -S, 0, 0,
		S, C, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
}

inline _simd_float4x4_c_t::self_t _simd_float4x4_c_t::left_transform::scale(
	const float3 &ratio) noexcept
{
	return scale(ratio.x, ratio.y, ratio.z);
}

inline _simd_float4x4_c_t::self_t _simd_float4x4_c_t::left_transform::scale(
	float x, float y, float z) noexcept
{
	return self_t(
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1);
}

inline _simd_float4x4_c_t::self_t _simd_float4x4_c_t::left_transform::perspective(
	float fov_y_rad, float w_over_h,
	float near_plane, float far_plane) noexcept
{
	const float inv_dis = 1 / (far_plane - near_plane);
	auto y_rad = 0.5f * fov_y_rad;
	auto cot = std::cos(y_rad) / std::sin(y_rad);

	return self_t(
		cot / w_over_h, 0, 0, 0,
		0, cot, 0, 0,
		0, 0, far_plane * inv_dis, -far_plane * near_plane * inv_dis,
		0, 0, 1, 0);
}

inline _simd_float4x4_c_t::self_t _simd_float4x4_c_t::left_transform::look_at(
	const float3 &eye, const float3 &dst, const float3 &up) noexcept
{
	auto D = (dst - eye).normalize();
	auto R = cross(up, D).normalize();
	auto U = cross(D, R);
	return self_t(
		R.x, U.x, D.x, eye.x,
		R.y, U.y, D.y, eye.y,
		R.z, U.z, D.z, eye.z,
		0, 0, 0, 1).inverse();
}

inline _simd_float4x4_c_t operator+(const _simd_float4x4_c_t &lhs, const _simd_float4x4_c_t &rhs) noexcept
{
	_simd_float4x4_c_t ret(UNINIT);
	ret[0] = _mm_add_ps(lhs[0], rhs[0]);
	ret[1] = _mm_add_ps(lhs[1], rhs[1]);
	ret[2] = _mm_add_ps(lhs[2], rhs[2]);
	ret[3] = _mm_add_ps(lhs[3], rhs[3]);
	return ret;
}

inline _simd_float4x4_c_t operator-(const _simd_float4x4_c_t &lhs, const _simd_float4x4_c_t &rhs) noexcept
{
	_simd_float4x4_c_t ret(UNINIT);
	ret[0] = _mm_sub_ps(lhs[0], rhs[0]);
	ret[1] = _mm_sub_ps(lhs[1], rhs[1]);
	ret[2] = _mm_sub_ps(lhs[2], rhs[2]);
	ret[3] = _mm_sub_ps(lhs[3], rhs[3]);
	return ret;
}

inline _simd_float4x4_c_t operator*(const _simd_float4x4_c_t &lhs, const _simd_float4x4_c_t &rhs) noexcept
{
	_simd_float4x4_c_t ret(UNINIT);

    {
		__m128 e0 = _mm_shuffle_ps(rhs[0], rhs[0], _MM_SHUFFLE(0, 0, 0, 0));
		__m128 e1 = _mm_shuffle_ps(rhs[0], rhs[0], _MM_SHUFFLE(1, 1, 1, 1));
		__m128 e2 = _mm_shuffle_ps(rhs[0], rhs[0], _MM_SHUFFLE(2, 2, 2, 2));
		__m128 e3 = _mm_shuffle_ps(rhs[0], rhs[0], _MM_SHUFFLE(3, 3, 3, 3));

		__m128 m0 = _mm_mul_ps(lhs[0], e0);
		__m128 m1 = _mm_mul_ps(lhs[1], e1);
		__m128 m2 = _mm_mul_ps(lhs[2], e2);
		__m128 m3 = _mm_mul_ps(lhs[3], e3);

		__m128 a0 = _mm_add_ps(m0, m1);
		__m128 a1 = _mm_add_ps(m2, m3);
		__m128 a2 = _mm_add_ps(a0, a1);

		ret[0] = a2;
	}

	{
		__m128 e0 = _mm_shuffle_ps(rhs[1], rhs[1], _MM_SHUFFLE(0, 0, 0, 0));
		__m128 e1 = _mm_shuffle_ps(rhs[1], rhs[1], _MM_SHUFFLE(1, 1, 1, 1));
		__m128 e2 = _mm_shuffle_ps(rhs[1], rhs[1], _MM_SHUFFLE(2, 2, 2, 2));
		__m128 e3 = _mm_shuffle_ps(rhs[1], rhs[1], _MM_SHUFFLE(3, 3, 3, 3));

		__m128 m0 = _mm_mul_ps(lhs[0], e0);
		__m128 m1 = _mm_mul_ps(lhs[1], e1);
		__m128 m2 = _mm_mul_ps(lhs[2], e2);
		__m128 m3 = _mm_mul_ps(lhs[3], e3);

		__m128 a0 = _mm_add_ps(m0, m1);
		__m128 a1 = _mm_add_ps(m2, m3);
		__m128 a2 = _mm_add_ps(a0, a1);

		ret[1] = a2;
	}

	{
		__m128 e0 = _mm_shuffle_ps(rhs[2], rhs[2], _MM_SHUFFLE(0, 0, 0, 0));
		__m128 e1 = _mm_shuffle_ps(rhs[2], rhs[2], _MM_SHUFFLE(1, 1, 1, 1));
		__m128 e2 = _mm_shuffle_ps(rhs[2], rhs[2], _MM_SHUFFLE(2, 2, 2, 2));
		__m128 e3 = _mm_shuffle_ps(rhs[2], rhs[2], _MM_SHUFFLE(3, 3, 3, 3));

		__m128 m0 = _mm_mul_ps(lhs[0], e0);
		__m128 m1 = _mm_mul_ps(lhs[1], e1);
		__m128 m2 = _mm_mul_ps(lhs[2], e2);
		__m128 m3 = _mm_mul_ps(lhs[3], e3);

		__m128 a0 = _mm_add_ps(m0, m1);
		__m128 a1 = _mm_add_ps(m2, m3);
		__m128 a2 = _mm_add_ps(a0, a1);

		ret[2] = a2;
	}

	{
		__m128 e0 = _mm_shuffle_ps(rhs[3], rhs[3], _MM_SHUFFLE(0, 0, 0, 0));
		__m128 e1 = _mm_shuffle_ps(rhs[3], rhs[3], _MM_SHUFFLE(1, 1, 1, 1));
		__m128 e2 = _mm_shuffle_ps(rhs[3], rhs[3], _MM_SHUFFLE(2, 2, 2, 2));
		__m128 e3 = _mm_shuffle_ps(rhs[3], rhs[3], _MM_SHUFFLE(3, 3, 3, 3));

		__m128 m0 = _mm_mul_ps(lhs[0], e0);
		__m128 m1 = _mm_mul_ps(lhs[1], e1);
		__m128 m2 = _mm_mul_ps(lhs[2], e2);
		__m128 m3 = _mm_mul_ps(lhs[3], e3);

		__m128 a0 = _mm_add_ps(m0, m1);
		__m128 a1 = _mm_add_ps(m2, m3);
		__m128 a2 = _mm_add_ps(a0, a1);

		ret[3] = a2;
	}

	return ret;
}

inline _simd_float4_t operator*(const _simd_float4x4_c_t &lhs, const _simd_float4_t &rhs) noexcept
{
	__m128 v0 = _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(0, 0, 0, 0));
	__m128 v1 = _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(1, 1, 1, 1));
	__m128 v2 = _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(2, 2, 2, 2));
	__m128 v3 = _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(3, 3, 3, 3));

	__m128 m0 = _mm_mul_ps(lhs[0], v0);
	__m128 m1 = _mm_mul_ps(lhs[1], v1);
	__m128 m2 = _mm_mul_ps(lhs[2], v2);
	__m128 m3 = _mm_mul_ps(lhs[3], v3);

	__m128 a0 = _mm_add_ps(m0, m1);
	__m128 a1 = _mm_add_ps(m2, m3);
	__m128 a2 = _mm_add_ps(a0, a1);

	return _simd_float4_t(a2);
}

inline _simd_float4_t operator*(const _simd_float4_t &lhs, const _simd_float4x4_c_t &rhs) noexcept
{
	__m128 i0 = rhs[0];
	__m128 i1 = rhs[1];
	__m128 i2 = rhs[2];
	__m128 i3 = rhs[3];

	__m128 m0 = _mm_mul_ps(lhs, i0);
	__m128 m1 = _mm_mul_ps(lhs, i1);
	__m128 m2 = _mm_mul_ps(lhs, i2);
	__m128 m3 = _mm_mul_ps(lhs, i3);

	__m128 u0 = _mm_unpacklo_ps(m0, m1);
	__m128 u1 = _mm_unpackhi_ps(m0, m1);
	__m128 a0 = _mm_add_ps(u0, u1);

	__m128 u2 = _mm_unpacklo_ps(m2, m3);
	__m128 u3 = _mm_unpackhi_ps(m2, m3);
	__m128 a1 = _mm_add_ps(u2, u3);

	__m128 f0 = _mm_movelh_ps(a0, a1);
	__m128 f1 = _mm_movehl_ps(a1, a0);
	__m128 f2 = _mm_add_ps(f0, f1);

	return _simd_float4_t(f2);
}

inline _simd_float4x4_c_t operator*(const _simd_float4x4_c_t &lhs, float rhs) noexcept
{
	return _simd_float4x4_c_t::from_cols(
	    lhs[0] * rhs,
		lhs[1] * rhs,
		lhs[2] * rhs,
		lhs[3] * rhs);
}

inline _simd_float4x4_c_t operator*(float lhs, const _simd_float4x4_c_t &rhs) noexcept
{
	return rhs * lhs;
}

inline _simd_float4x4_c_t operator/(const _simd_float4x4_c_t &lhs, float rhs) noexcept
{
	const float invRHS = 1 / rhs;
	return lhs * invRHS;
}

inline _simd_float4x4_c_t &operator+=(
	_simd_float4x4_c_t &lhs, const _simd_float4x4_c_t &rhs) noexcept
{
	lhs = lhs + rhs;
	return lhs;
}

inline _simd_float4x4_c_t &operator-=(
	_simd_float4x4_c_t &lhs, const _simd_float4x4_c_t &rhs) noexcept
{
	lhs = lhs - rhs;
	return lhs;
}

inline _simd_float4x4_c_t &operator*=(
	_simd_float4x4_c_t &lhs, const _simd_float4x4_c_t &rhs) noexcept
{
	lhs = lhs * rhs;
	return lhs;
}

inline _simd_float4x4_c_t &operator*=(
	_simd_float4x4_c_t &lhs, float rhs) noexcept
{
	lhs = lhs * rhs;
	return lhs;
}

inline _simd_float4x4_c_t &operator/=(
	_simd_float4x4_c_t &lhs, float rhs) noexcept
{
	lhs = lhs / rhs;
	return lhs;
}

} // namespace agz::math

#endif // #ifdef AGZ_UTILS_SSE
