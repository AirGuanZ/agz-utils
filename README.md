# agzm

Utilities for computer graphics

## usage

In `CMakeLists.txt`:

```cmake
ADD_SUBDIRECTORY(path_to_agzm/)
INCLUDE_DIRECTORIES(${AGZUtils_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(your_target AGZUtils)
```

In C++:

```cpp
#include <agz/utility/utility.h>
```

## math

```cpp
using namespace agz::math;
```

### vec

```cpp
vec<int, 6> integer_vector_6d(1, 2, 3, 4, 5, 6);
vec2f       float_vector_2d;
vec3f       float_vector_3d;
vec4f       float_vector_4d;
vec2d       double_vector_2d;
vec3d       double_vector_3d;
vec4d       double_vector_4d;
```

### color

```cpp
color3f float_rgb;
color4f float_rgba;
color3d double_rgb;
color4d double_rgba;
color3b byte_rgb;
color4b byte_rgba;
```

### swizzle

```cpp
auto v = vec4f(0, 1, 2, 3).zzx();
auto c = color3b(0, 1, 2).bgr();
```

### mat_c (column-major)

```cpp
// translate/rotate/scale/perspective/look_at
auto m4 = mat4f::translate(1, 2, 3);
auto v4 = m4.inv() * vec4f(0, 0, 0, 1);

auto m3 = mat3f::scale(2, 3);
auto v3 = m3 * vec3f(1, 1, 1);
```

### transform

```cpp
auto transform          = transform3f::scale(1, 2, 3) * transform3f::rotate_x(PI_f / 2);
auto normal             = vec3f(1, 2, 3).normalize();
auto transformed_normal = transform.apply_to_normal(normal);
```

### spherical harmonics

```cpp
float value = spherical_harmonics::eval<3, -2>(vec3f(1, 1, 0)); // eval sh

auto rotate_mat = mat3f::rotate(vec3f(1, 2, 3), 0.6);    // 3x3 rotation matrix
float coefs[5] = { ... };                                // 2-order sh coefficients
spherical_harmonics::rotate_sh_coefs(rotate_mat, coefs); // transform sh coefficients
```

### tensor

```cpp
tensor_t<float, 3> t1({ 256, 256, 3 }, 1);
auto t2 = tensor_t<float, 3>::from_linear_indexed_fn(
    			[](int i) { return float(i * i); });
auto t3 = t1 + t2;
```

## image & texture

```cpp
using namespace agz::img;
using namespace agz::texture;

texture2d_t<agz::math::byte> tex(load_grey_from_file("tex.png"));
save_gray_to_png_file("output.png", tex.get_data());
```

