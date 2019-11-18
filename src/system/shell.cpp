#include <agz/utility/system/platform.h>
#include <agz/utility/system/shell.h>

#ifdef AGZ_OS_WIN32
#   include <Windows.h>
#elif defined(AGZ_OS_LINUX)
#   include <cstdlib>
#endif

namespace agz::sys
{
    
void open_with_default_app(const std::string &filename)
{
#ifdef AGZ_OS_WIN32
    ShellExecuteA(0, 0, filename.c_str(), 0, 0, SW_SHOW);
#elif defined(AGZ_OS_LINUX)
    [[maybe_unused]] int dummy_ret = std::system(("xdg-open " + filename).c_str());
#endif
}

} // namespace agz::sys
