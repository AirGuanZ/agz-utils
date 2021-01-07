#include <cassert>
#include <climits>

#include <agz-utils/file/file_raw.h>
#include <agz-utils/image/load_image.h>
#include <agz-utils/misc.h>

//=============================== begin stb SIMD includes ===============================

#if defined(__GNUC__) && defined(STBI__X86_TARGET) && !defined(__SSE2__) && !defined(STBI_NO_SIMD)
// gcc doesn't support sse2 intrinsics unless you compile with -msse2,
// which in turn means it gets to use SSE2 everywhere. This is unfortunate,
// but previous attempts to provide the SSE2 functions with runtime
// detection caused numerous issues. The way architecture extensions are
// exposed in GCC/Clang is, sadly, not really suited for one-file libs.
// New behavior: if compiled with -msse2, we use SSE2 without any
// detection; if not, we don't use it at all.
#define STBI_NO_SIMD
#endif

#if defined(__MINGW32__) && defined(STBI__X86_TARGET) && !defined(STBI_MINGW_ENABLE_SSE2) && !defined(STBI_NO_SIMD)
// Note that __MINGW32__ doesn't actually mean 32-bit, so we have to avoid STBI__X64_TARGET
//
// 32-bit MinGW wants ESP to be 16-byte aligned, but this is not in the
// Windows ABI and VC++ as well as Windows DLLs don't maintain that invariant.
// As a result, enabling SSE2 on 32-bit MinGW is dangerous when not
// simultaneously enabling "-mstackrealign".
//
// See https://github.com/nothings/stb/issues/81 for more information.
//
// So default to no SSE2 on 32-bit MinGW. If you've read this far and added
// -mstackrealign to your build settings, feel free to #define STBI_MINGW_ENABLE_SSE2.
#define STBI_NO_SIMD
#endif

#if !defined(STBI_NO_SIMD) && (defined(STBI__X86_TARGET) || defined(STBI__X64_TARGET))
#define STBI_SSE2
#include <emmintrin.h>

#ifdef _MSC_VER

#if _MSC_VER >= 1400  // not VC6
#include <intrin.h> // __cpuid
static int stbi__cpuid3(void)
{
    int info[4];
    __cpuid(info, 1);
    return info[3];
}
#else
static int stbi__cpuid3(void)
{
    int res;
    __asm {
        mov  eax, 1
        cpuid
        mov  res, edx
    }
    return res;
}
#endif

#define STBI_SIMD_ALIGN(type, name) __declspec(align(16)) type name

static int stbi__sse2_available(void)
{
    int info3 = stbi__cpuid3();
    return ((info3 >> 26) & 1) != 0;
}
#else // assume GCC-style if not VC++
#define STBI_SIMD_ALIGN(type, name) type name __attribute__((aligned(16)))

static int stbi__sse2_available(void)
{
    // If we're even attempting to compile this on GCC/Clang, that means
    // -msse2 is on, which means the compiler is allowed to use SSE2
    // instructions at will, and so are we.
    return 1;
}
#endif
#endif

// ARM NEON
#if defined(STBI_NO_SIMD) && defined(STBI_NEON)
#undef STBI_NEON
#endif

#ifdef STBI_NEON
#include <arm_neon.h>
// assume GCC or Clang on ARM targets
#define STBI_SIMD_ALIGN(type, name) type name __attribute__((aligned(16)))
#endif

#ifndef STBI_SIMD_ALIGN
#define STBI_SIMD_ALIGN(type, name) type name
#endif

//=============================== end stb SIMD includes ===============================

namespace
{

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

}

namespace agz::img
{

template<typename P>
using image_buffer = math::tensor_t<P, 2>;

std::vector<math::byte> load_bytes_from_memory(
    const void *data, size_t byte_length,
    int *width, int *height, int *channels)
{
    int w, h, c;
    unsigned char *bytes = stbi_load_from_memory(
        static_cast<const stbi_uc *>(data),
        static_cast<int>(byte_length),
        &w, &h, &c, 0);
    if(!bytes)
        return {};

    AGZ_SCOPE_GUARD({ stbi_image_free(bytes); });

    *width    = w;
    *height   = h;
    *channels = c;
    return std::vector<math::byte>(bytes, bytes + w * h * c);
}

image_buffer<math::byte> load_gray_from_memory(const void *data, size_t byte_length)
{
    int w, h, channels;
    unsigned char *bytes = stbi_load_from_memory(
        static_cast<const stbi_uc*>(data),
        static_cast<int>(byte_length),
        &w, &h, &channels, STBI_grey);
    if(!bytes)
        return image_buffer<math::byte>();

    AGZ_SCOPE_GUARD({ stbi_image_free(bytes); });

    assert(w > 0 && h > 0);

    return image_buffer<math::byte>::from_linear_indexed_fn(
        { h, w }, [&](int i) { return bytes[i]; });
}

namespace img_impl
{

    template<typename P, int CH, int STBchannel>
    image_buffer<P> load_from_memory(const void *data, size_t byte_length)
    {
        int w, h, channels;
        unsigned char *bytes = stbi_load_from_memory(
            static_cast<const stbi_uc*>(data),
            static_cast<int>(byte_length),
            &w, &h, &channels, STBchannel);
        if(!bytes)
            return image_buffer<P>();

        AGZ_SCOPE_GUARD({ stbi_image_free(bytes); });

        assert(w > 0 && h > 0);

        int idx = 0;

        return image_buffer<P>::from_linear_indexed_fn(
            { h, w }, [&](int i)
        {
            P ret(UNINIT);
            for(int j = 0; j < CH; ++j)
                ret[j] = bytes[idx + j];
            idx += CH;
            return ret;
        });
    }

} // namespace img_impl

image_buffer<math::color2b> load_gray_alpha_from_memory(
    const void *data, size_t byte_length)
{
    return img_impl::load_from_memory<math::color2b, 2, STBI_grey_alpha>(
        data, byte_length);
}

image_buffer<math::color3b> load_rgb_from_memory(
    const void *data, size_t byte_length)
{
    return img_impl::load_from_memory<math::color3b, 3, STBI_rgb>(
        data, byte_length);
}

image_buffer<math::color4b> load_rgba_from_memory(
    const void *data, size_t byte_length)
{
    return img_impl::load_from_memory<math::color4b, 4, STBI_rgb_alpha>(
        data, byte_length);
}

math::tensor_t<math::color3f, 2> load_rgb_from_hdr_memory(
    const void *data_ptr, size_t byte_length)
{
    int w, h, channels;

    float *data = stbi_loadf_from_memory(
        static_cast<const stbi_uc *>(data_ptr),
        static_cast<int>(byte_length),
        &w, &h, &channels, STBI_rgb);
    if(!data)
        return image_buffer<math::color3f>();

    AGZ_SCOPE_GUARD({ stbi_image_free(data); });

    assert(w > 0 && h > 0);

    int idx = 0;
    return image_buffer<math::color3f>::from_linear_indexed_fn(
        { h, w }, [&](int i)
    {
        math::color3f ret(UNINIT);
        for(int j = 0; j < 3; ++j)
            ret[j] = data[idx + j];
        idx += 3;
        return ret;
    });
}

std::vector<math::byte> load_bytes_from_file(
    const std::string &filename,
    int *width, int *height, int *channels)
{
    const auto content = file::read_raw_file(filename);
    return load_bytes_from_memory(
        content.data(), content.size(), width, height, channels);
}

image_buffer<math::byte> load_gray_from_file(const std::string &filename)
{
    auto content = file::read_raw_file(filename);
    return load_gray_from_memory(content.data(), content.size());
}

image_buffer<math::color2b> load_gray_alpha_from_file(
    const std::string &filename)
{
    auto content = file::read_raw_file(filename);
    return load_gray_alpha_from_memory(content.data(), content.size());
}

image_buffer<math::color3b> load_rgb_from_file(const std::string &filename)
{
    auto content = file::read_raw_file(filename);
    return load_rgb_from_memory(content.data(), content.size());
}

image_buffer<math::color4b> load_rgba_from_file(const std::string &filename)
{
    auto content = file::read_raw_file(filename);
    return load_rgba_from_memory(content.data(), content.size());
}

math::tensor_t<math::color3f, 2> load_rgb_from_hdr_file(
    const std::string &filename)
{
    auto content = file::read_raw_file(filename);
    return load_rgb_from_hdr_memory(content.data(), content.size());
}

} // namespace agz::img
