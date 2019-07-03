# agzm

## usage

In `CMakeLists.txt`:

```cmake
ADD_SUBDIRECTORY(path_to_agzm/src)
INCLUDE_DIRECTORIES(${AGZUtils_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(your_target AGZUtils)
```

In C++:

```cpp
#include <agz/utility/utility.h>
```
