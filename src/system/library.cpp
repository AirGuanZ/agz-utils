#include <string>

#include <agz-utils/misc/scope_guard.h>
#include <agz-utils/system/library.h>
#include <agz-utils/system/platform.h>

#ifdef AGZ_OS_WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

namespace agz::sys
{

struct shared_lib_handle_t
{
#ifdef AGZ_OS_WIN32
    HINSTANCE handle;
#else
    void *handle;
#endif
};
    
shared_lib_handle_t *load_lib(const std::string &filename, std::string *err_msg)
{
    auto ret = new shared_lib_handle_t;
    misc::scope_guard_t guard([&] { delete ret; });

#ifdef AGZ_OS_WIN32

    ret->handle = LoadLibraryA(filename.c_str());
    if(!ret->handle)
    {
        if(err_msg)
            *err_msg = "failed to load lib from " + filename +
                ". err code = " + std::to_string(GetLastError());
        return nullptr;
    }

#else

    ret->handle = dlopen(filename.c_str(), RTLD_LAZY);
    if(!ret->handle)
    {
        if(err_msg)
            *err_msg = "failed to load lib from " + filename +
                ". err msg = " + dlerror();
        return nullptr;
    }

#endif

    guard.dismiss();
    return ret;
}

void destroy_lib(shared_lib_handle_t *handle)
{
    assert(handle);
    assert(handle->handle);
    AGZ_SCOPE_GUARD({ delete handle; });

#ifdef AGZ_OS_WIN32

    FreeLibrary(handle->handle);

#else

    dlclose(handle->handle);

#endif
}

void_func_ptr get_lib_process(
    shared_lib_handle_t *handle, const char *proc_name, std::string *err_msg)
{
    assert(handle && handle->handle);

#ifdef AGZ_OS_WIN32

    auto ret = reinterpret_cast<void_func_ptr>(
        GetProcAddress(handle->handle, proc_name));
    if(!ret)
    {
        if(err_msg)
            *err_msg = std::string("failed to get proc ") + proc_name +
                ". err code = " + std::to_string(GetLastError());
        return nullptr;
    }

#else

    auto ret = reinterpret_cast<void_func_ptr>(dlsym(handle->handle, proc_name));
    if(!ret)
    {
        if(err_msg)
            *err_msg = std::string("failed to get proc ") + proc_name +
                ". err code = " + dlerror();
        return nullptr;
    }

#endif

    return ret;
}

} // namespace agz::sys
