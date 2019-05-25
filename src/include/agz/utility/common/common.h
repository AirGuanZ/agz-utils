#pragma once

namespace agz
{
    
struct uninitialized_t { };
inline uninitialized_t UNINIT;

template<typename T>
using rm_rcv_t = std::remove_cv_t<std::remove_reference_t<T>>;

} // namespace agz
