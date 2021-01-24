## agz-utils

### Usage

In `CMakeLists.txt`:

```cmake
ADD_SUBDIRECTORY(path_to_agz_utils)
TARGET_LINK_LIBRARIES(your_target AGZUtils)
```

### Components

```cpp
#include <agz-utils/alloc.h>

// in namespace agz::alloc

template<typename T> void call_destructor(T &obj) noexcept;
template<typename T> void call_destructor(T *arr, size_t num) noexcept;

void *aligned_alloc(size_t byte_size, size_t align);
void  aligned_free(void *ptr);

// void *ptr = arena.alloc(size, align);
// arena.free(ptr);
// cout << arena.used_bytes();
class mem_arena_t;

// T *obj = arena.create<T>(arg0, arg1, ...);
// arena.release();
// cout << arena.used_bytes();
class obj_arena_t;

// T *obj = pool.create(arg0, arg1, ...);
// pool.destroy(obj);
template<typename T> class obj_pool_t;
```

```cpp
#include <agz-utils/common/common.h>

// in namespace agz

template<typename T, size_t N>
constexpr size_t array_size(const T (&)[N]) noexcept;

// typed offsetof
// not recommend to use
template<typename C, typename M>
ptrdiff_t byte_offset(M (C::*mem_ptr)) noexcept;

// return ptr to the given object
// be careful of lifetime of val
template<typename T>
const T *get_temp_ptr(const T &val) noexcept;

#define AGZ_UNACCESSED(X) ...

// returns minimum x satisfying:
//    (x >= value) and (x % alignto == 0)
template<typename T>
constexpr T upalign_to(T value, T alignto) noexcept;
```

```cpp
#include <agz-utils/console.h>

// in namespace agz::console

// pbar.reset_time();
// for(int i = beg; i < end; ++i)
// {
//     pbar.set_percent(percentage);
//     pbar.display();
// }
// pbar.done();
class progress_bar_t;
class progress_bar_f_t;
```

```cpp
#include <agz-utils/container.h>

// in namespace agz::container

// optional<int> interval = mgr.alloc(interval_size);
// mgr.free(beg, end);
class interval_mgr_t;

// cout << cont.empty() << endl;
// cout << cont.size()  << endl;
// 
// optional<value_t> value = cont.find_and_erase(key);
// value_t *value_ptr = cont.find(key);
// bool is_key_exist = cont.exists(key);
// 
// value_t &front = cont.front();
// cont.pop_back();
// value_t &back  = cont.back();
// cont.push_front(new_key, new_value);
// cont.clear();
// 
// cont.for_each(functor);
// cont.sort_list_by_key(compare_functor);
template<typename Key, typename Value>
class linked_map_t;
```

```cpp
#include <agz-utils/event.h>

// in namespace agz::event

// a simple implementation of 'listener pattern'
// typical usage:
//     struct MyEvent1 { int arg0; int arg1; };
//     struct MyEvent2 { int arg0; float arg1; double arg2; };
//     class MyEventSource
//     {
//     public:
//         AGZ_DECL_EVENT_SENDER_HANDLER(sender_, MyEvent1)
//         AGZ_DECL_EVENT_SENDER_HANDLER(sender_, MyEvent2)
//     private:
//         sender_t<MyEvent1, MyEvent2> sender_;
//     };
//     MyEventSource eventSource;
//     eventSource.attach([](const MyEvent1 &e) { ... });
//     eventSource.attach([](const MyEvent1 &e) { ... });
//     eventSource.attach([](const MyEvent2 &e) { ... });

template<typename Event>
class receiver_t;
template<typename Event>
class receiver_set_t;
template<typename...Events>
class sender_t;

template<typename Event>
class functional_receiver_t;
template<typename Event, typename Class>
class class_receiver_t;

#define AGZ_DECL_EVENT_SENDER_HANDLER(Sender, Event) ...

using keycode_t = ...
namespace keycode_constants
{
    constexpr keycode_t KEY_... = ...
}

#ifdef AGZ_OS_WIN32
keycode_t win_vk_to_keycode(int VK) noexcept;
#endif
```

```cpp
#include <agz-utils/file.h>

// in namespace agz::file

void create_directory_for_file(const std::filesystem::path &filename);

std::vector<unsigned char> read_raw_file(const std::string &filename);

void write_raw_file(
    const std::string &filename, const void *data, size_t byte_size);

std::string read_txt_file(const std::string &filename);
```

```cpp
// define macro AGZ_ENABLE_D3D11 in compiler settings
#include <agz-utils/graphics_api.h>

// in namespace agz::d3d11

using Float2 = ...;
using Float3 = ...;
using Float4 = ...;
using Color4 = ...;
using Int2 = ...;
using Int3 = ...;
using Mat4 = ...;
using Trans4 = ...;

using Microsoft::WRL::ComPtr;

class D3D11Exception;

template<typename Vertex> class VertexBuffer;
template<typename Index>  class IndexBuffer;
template<typename Struct> class ConstantBuffer;

class Device;
class DeviceContext;
class Immediate2D;
class InputLayoutBuilder;
class Keyboard;
class Mouse;
class RenderTarget;
class Shader;
class Texture2DLoader;
class Window;
```

```cpp
// define macro AGZ_ENABLE_D3D12 in compiler settings
#include <agz-utils/graphics_api.h>

// in namespace agz::d3d12
// TODO: detailed documentation

void enableDebugLayer(bool gpuValidation);
void enableDebugLayerInDebugMode(bool gpuValidation);

class Buffer;
class CommandList;
class D3D12Context;
class DescriptorAllocator;
class DescriptorHeap;
class Device;
class FrameFence;
class ImGuiIntegration;
class Input;
class MipmapGenerator;
class PipelineBuilder;
class CommandQueueWaiter;
class ResourceManager;
class ResourceUploader;
class RootSignatureBuilder;
class FXC;
class DXC;
class SwapChain;
class TextureLoader;
class Window;
```

```cpp
// define macro AGZ_ENABLE_OPENGL in compiler settings
#include <agz-utils/graphics_api.h>

// in namespace agz::gl
// TODO: opengl components doc
```

```cpp
#include <agz-utils/image.h>

// in namespace agz::img

std::vector<math::byte> load_bytes_from_memory(
    const void *data, size_t byte_length,
    int *width, int *height, int *channels);
    
math::tensor_t<math::byte, 2> load_gray_from_memory(
    const void *data, size_t byte_length);

math::tensor_t<math::color2b, 2> load_gray_alpha_from_memory(
    const void *data, size_t byte_length);

math::tensor_t<math::color3b, 2> load_rgb_from_memory(
    const void *data, size_t byte_length);

math::tensor_t<math::color4b, 2> load_rgba_from_memory(
    const void *data, size_t byte_length);

math::tensor_t<math::color3f, 2> load_rgb_from_hdr_memory(
    const void *data, size_t byte_length);

std::vector<math::byte> load_bytes_from_file(
    const std::string &filename,
    int *width, int *height, int *channels);

math::tensor_t<math::byte, 2> load_gray_from_file(
    const std::string &filename);

math::tensor_t<math::color2b, 2> load_gray_alpha_from_file(
    const std::string &filename);

math::tensor_t<math::color3b, 2> load_rgb_from_file(
    const std::string &filename);

math::tensor_t<math::color4b, 2> load_rgba_from_file(
    const std::string &filename);

math::tensor_t<math::color3f, 2> load_rgb_from_hdr_file(
    const std::string &filename);

std::vector<unsigned char> save_gray_to_png_in_memory(
    const math::byte *data, int w, int h);

std::vector<unsigned char> save_gray_alpha_to_png_in_memory(
    const math::color2b *data, int w, int h);

std::vector<unsigned char> save_rgb_to_png_in_memory(
    const math::color3b *data, int w, int h);

std::vector<unsigned char> save_rgba_to_png_in_memory(
    const math::color4b *data, int w, int h);

void save_gray_to_png_file(
    const std::string &filename, const math::byte *data, int w, int h);

void save_gray_to_png_file(
    const std::string &filename, const math::tensor_t<math::byte, 2> &data);

void save_gray_alpha_to_png_file(
    const std::string &filename, const math::color2b *data, int w, int h);

void save_gray_alpha_to_png_file(
    const std::string &filename, const math::tensor_t<math::color2b, 2> &data);

void save_rgb_to_png_file(
    const std::string &filename, const math::color3b *data, int w, int h);

void save_rgb_to_png_file(
    const std::string &filename, const math::tensor_t<math::color3b, 2> &data);

void save_rgba_to_png_file(
    const std::string &filename, const math::color4b *data, int w, int h);

void save_rgba_to_png_file(
    const std::string &filename, const math::tensor_t<math::color4b, 2> &data);

std::vector<unsigned char> save_gray_to_jpg_in_memory(
    const math::byte *data, int w, int h);

std::vector<unsigned char> save_gray_alpha_to_jpg_in_memory(
    const math::color2b *data, int w, int h);

std::vector<unsigned char> save_rgb_to_jpg_in_memory(
    const math::color3b *data, int w, int h);

std::vector<unsigned char> save_rgba_to_jpg_in_memory(
    const math::color4b *data, int w, int h);

void save_gray_to_jpg_file(
    const std::string &filename, const math::byte *data, int w, int h);

void save_gray_to_jpg_file(
    const std::string &filename, const math::tensor_t<math::byte, 2> &data);

void save_gray_alpha_to_jpg_file(
    const std::string &filename, const math::color2b *data, int w, int h);

void save_gray_alpha_to_jpg_file(
    const std::string &filename, const math::tensor_t<math::color2b, 2> &data);

void save_rgb_to_jpg_file(
    const std::string &filename, const math::color3b *data, int w, int h);

void save_rgb_to_jpg_file(
    const std::string &filename, const math::tensor_t<math::color3b, 2> &data);

void save_rgba_to_jpg_file(
    const std::string &filename, const math::color4b *data, int w, int h);

void save_rgba_to_jpg_file(
    const std::string &filename, const math::tensor_t<math::color4b, 2> &data);

std::vector<unsigned char> save_gray_to_bmp_in_memory(
    const math::byte *data, int w, int h);

std::vector<unsigned char> save_gray_alpha_to_bmp_in_memory(
    const math::color2b *data, int w, int h);

std::vector<unsigned char> save_rgb_to_bmp_in_memory(
    const math::color3b *data, int w, int h);

std::vector<unsigned char> save_rgba_to_bmp_in_memory(
    const math::color4b *data, int w, int h);

void save_gray_to_bmp_file(
    const std::string &filename, const math::byte *data, int w, int h);

void save_gray_to_bmp_file(
    const std::string &filename, const math::tensor_t<math::byte, 2> &data);

void save_gray_alpha_to_bmp_file(
    const std::string &filename, const math::color2b *data, int w, int h);

void save_gray_alpha_to_bmp_file(
    const std::string &filename, const math::tensor_t<math::color2b, 2> &data);

void save_rgb_to_bmp_file(
    const std::string &filename, const math::color3b *data, int w, int h);

void save_rgb_to_bmp_file(
    const std::string &filename, const math::tensor_t<math::color3b, 2> &data);

void save_rgba_to_bmp_file(
    const std::string &filename, const math::color4b *data, int w, int h);

void save_rgba_to_bmp_file(
    const std::string &filename, const math::tensor_t<math::color4b, 2> &data);

std::vector<unsigned char> save_rgb_to_hdr_in_memory(
    const float *data, int w, int h);

std::vector<unsigned char> save_rgb_to_hdr_in_memory(
    const math::color3f *data, int w, int h);

std::vector<unsigned char> save_rgb_to_hdr_in_memory(
    const math::vec3f *data, int w, int h);

std::vector<unsigned char> save_rgb_to_hdr_in_memory(
    const math::tensor_t<math::color3f, 2> &data);

void save_rgb_to_hdr_file(
    const std::string &filename, const float *data, int w, int h);

void save_rgb_to_hdr_file(
    const std::string &filename, const math::color3f *data, int w, int h);

void save_rgb_to_hdr_file(
    const std::string &filename, const math::vec3f *data, int w, int h);

void save_rgb_to_hdr_file(
    const std::string &filename, const math::tensor_t<math::color3f, 2> &data);
```

```cpp
#include <agz-utils/math.h>
```

```cpp
#include <agz-utils/mesh.h>
```

```cpp
#include <agz-utils/misc.h>
```

```cpp
#include <agz-utils/string.h>
```

```cpp
#include <agz-utils/system.h>
```

```cpp
#include <agz-utils/texture.h>
```

```cpp
#include <agz-utils/thread.h>
```

```cpp
#include <agz-utils/time.h>
```

