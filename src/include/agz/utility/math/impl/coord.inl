#pragma once

namespace agz::math {

template<typename T>
tcoord3<T>::tcoord3() noexcept
    : x(axis_t(1, 0, 0)), y(axis_t(0, 1, 0)), z(axis_t(0, 0, 1))
{
    
}

template<typename T>
tcoord3<T>::tcoord3(const axis_t &x, const axis_t &y, const axis_t &z) noexcept
    : x(x.normalize()), y(y.normalize()), z(z.normalize())
{
    
}

template<typename T>
tcoord3<T>::tcoord3(uninitialized_t) noexcept
    : x(UNINIT), y(UNINIT), z(UNINIT)
{
    
}

template<typename T>
typename tcoord3<T>::self_t tcoord3<T>::from_x(axis_t new_x) noexcept
{
    new_x = new_x.normalize();
    axis_t new_z(UNINIT);
    if(std::abs(std::abs(dot(new_x, axis_t(0, 1, 0))) - 1) < T(0.1))
        new_z = cross(new_x, axis_t(0, 0, 1));
    else
        new_z = cross(new_x, axis_t(0, 1, 0));
    return self_t(new_x, cross(new_z, new_x), new_z);
}

template<typename T>
typename tcoord3<T>::self_t tcoord3<T>::from_y(axis_t new_y) noexcept
{
    new_y = new_y.normalize();
    axis_t new_x(UNINIT);
    if(std::abs(std::abs(dot(new_y, axis_t(0, 0, 1))) - 1) < T(0.1))
        new_x = cross(new_y, axis_t(1, 0, 0));
    else
        new_x = cross(new_y, axis_t(0, 0, 1));
    return self_t(new_x, new_y, cross(new_x, new_y));
}

template<typename T>
typename tcoord3<T>::self_t tcoord3<T>::from_z(axis_t new_z) noexcept
{
    new_z = new_z.normalize();
    axis_t new_y(UNINIT);
    if(std::abs(std::abs(dot(new_z, axis_t(1, 0, 0))) - 1) < T(0.1))
        new_y = cross(new_z, axis_t(0, 1, 0));
    else
        new_y = cross(new_z, axis_t(1, 0, 0));
    return self_t(cross(new_y, new_z), new_y, new_z);
}

template<typename T>
typename tcoord3<T>::vec_t tcoord3<T>::global_to_local(const vec_t &global_vector) const noexcept
{
    return vec_t(dot(x, global_vector),
                 dot(y, global_vector),
                 dot(z, global_vector));
}

template<typename T>
typename tcoord3<T>::vec_t tcoord3<T>::local_to_global(const vec_t &local_vector) const noexcept
{
    return x * local_vector.x
         + y * local_vector.y
         + z * local_vector.z;
}

template<typename T>
typename tcoord3<T>::self_t tcoord3<T>::rotate_to_new_x(const axis_t &new_x) const noexcept
{
    axis_t new_y = cross(z, new_x);
    axis_t new_z = cross(new_x, new_y);
    return self_t(new_x, new_y, new_z);
}

template<typename T>
typename tcoord3<T>::self_t tcoord3<T>::rotate_to_new_y(const axis_t &new_y) const noexcept
{
    axis_t new_x = cross(new_y, z);
    axis_t new_z = cross(new_x, new_y);
    return self_t(new_x, new_y, new_z);
}

template<typename T>
typename tcoord3<T>::self_t tcoord3<T>::rotate_to_new_z(const axis_t &new_z) const noexcept
{
    axis_t new_x = cross(y, new_z);
    axis_t new_y = cross(new_z, new_x);
    return self_t(new_x, new_y, new_z);
}

template<typename T>
tcoord3<T> operator-(const tcoord3<T> &coord) noexcept
{
    return tcoord3<T>(-coord.x, -coord.y, -coord.z);
}

template<typename T>
bool tcoord3<T>::in_positive_x_hemisphere(const vec_t &v) const noexcept
{
    return dot(v, x) > 0;
}

template<typename T>
bool tcoord3<T>::in_positive_y_hemisphere(const vec_t &v) const noexcept
{
    return dot(v, y) > 0;
}

template<typename T>
bool tcoord3<T>::in_positive_z_hemisphere(const vec_t &v) const noexcept
{
    return dot(v, z) > 0;
}

} // namespace agz::math
