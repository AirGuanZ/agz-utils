#pragma once

#include "common.h"
#include "vec3.h"

namespace agz::math {

template<typename T>
class tcoord3
{
public:

    using axis_t = tvec3<T>;
    using self_t = tcoord3<T>;
    using vec_t  = tvec3<T>;

    axis_t x, y, z;

    /** @brief Ĭ�ϳ�ʼ��Ϊ(1, 0, 0), (0, 1, 0), (0, 0, 1) */
    tcoord3()                                                  noexcept;

    /** @brief ���Զ���x��y��z���й�һ����������������ʩ��������֮��ı������໥��ֱ�Ĵ�ʩ */
    tcoord3(const axis_t &x, const axis_t &y, const axis_t &z) noexcept;

    /** @brief �����ڲ���ԱΪδ��ʼ��״̬ */
    explicit tcoord3(uninitialized_t)                          noexcept;

    /** @brief ����һ���Ը��������ķ���Ϊx���������ֱ������ϵ */
    static self_t from_x(axis_t new_x) noexcept;

    /** @brief ����һ���Ը��������ķ���Ϊy���������ֱ������ϵ */
    static self_t from_y(axis_t new_y) noexcept;

    /** @brief ����һ���Ը��������ķ���Ϊz���������ֱ������ϵ */
    static self_t from_z(axis_t new_z) noexcept;

    /** @brief ��world space�е���������ת����������ϵ���� */
    vec_t global_to_local(const vec_t &global_vector) const noexcept;

    /** @brief ��local space�е���������ת����world space�� */
    vec_t local_to_global(const vec_t &local_vector)  const noexcept;

    /** @brief ��ת������ϵ��ʹ��x�����������Ϊͬһ���򣬷��صõ���������ϵ */
    self_t rotate_to_new_x(const axis_t &new_x) const noexcept;

    /** @brief ��ת������ϵ��ʹ��y�����������Ϊͬһ���򣬷��صõ���������ϵ */
    self_t rotate_to_new_y(const axis_t &new_y) const noexcept;

    /** @brief ��ת������ϵ��ʹ��z�����������Ϊͬһ���򣬷��صõ���������ϵ */
    self_t rotate_to_new_z(const axis_t &new_z) const noexcept;

    /** @brief ���������Ƿ�λ��x�����ڵ������� */
    bool in_positive_x_hemisphere(const vec_t &v) const noexcept;

    /** @brief ���������Ƿ�λ��y�����ڵ������� */
    bool in_positive_y_hemisphere(const vec_t &v) const noexcept;

    /** @brief ���������Ƿ�λ��z�����ڵ������� */
    bool in_positive_z_hemisphere(const vec_t &v) const noexcept;
};

template<typename T>
tcoord3<T> operator-(const tcoord3<T> &coord) noexcept;

} // namespace agz::math
