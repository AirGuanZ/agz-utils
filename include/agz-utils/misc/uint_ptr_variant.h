#pragma once

#include <optional>
#include <memory>
#include <type_traits>

namespace agz::misc
{

/**
 * @brief 指针和无符号整数的union
 *
 * 利用指针的对齐特性用最低位作为tag
 */
template<typename TUInt, typename TPtrDest>
class uint_ptr_variant_t
{
    static_assert(sizeof(TUInt) < sizeof(size_t) - 1);
    static_assert(sizeof(size_t) == sizeof(TPtrDest));

    size_t data_;

    static size_t uint_to_data(TUInt uint_value) noexcept;

public:

    using uint_t         = TUInt;
    using pointer_t      = TPtrDest*;
    using pointer_dest_t = TPtrDest;

    using self_t = uint_ptr_variant_t<uint_t, pointer_t>;

    /**
     * @brief 默认初始化为uint(0)
     */
    uint_ptr_variant_t() noexcept;

    /**
     * @brief 初始化为指定uint值
     */
    explicit uint_ptr_variant_t(uint_t uint_value) noexcept;

    /**
     * @brief 初始化为指定指针，带所有权
     */
    explicit uint_ptr_variant_t(std::unique_ptr<TPtrDest> owner_ptr);

    /**
     * @brief 初始化为指定指针，带所有权
     */
    explicit uint_ptr_variant_t(TPtrDest *owner_ptr);

    /**
     * @brief 由于指针独占所有权，可能调用存储对象的clone()方法
     *  获得新的std::unique_ptr<TPtrDest>
     */
    uint_ptr_variant_t(const self_t &copy_from);

    uint_ptr_variant_t(self_t &&move_from) noexcept;

    /**
     * @brief 由于指针独占所有权，可能调用存储对象的clone()方法
     *  获得新的std::unique_ptr<TPtrDest>
     */
    uint_ptr_variant_t &operator=(const self_t &copy_from);

    uint_ptr_variant_t &operator=(self_t &&move_from) noexcept;

    ~uint_ptr_variant_t();

    void swap(self_t &swap_with) noexcept;

    /**
     * @brief 内部存储的是否是一个指针
     */
    bool is_ptr() const noexcept;

    /**
     * @brief 内部存储的是否是一个uint
     */
    bool is_uint() const noexcept;

    /**
     * @brief 尝试取得内部存储的指针
     *
     * 若存储的是整数，返回std::nullopt
     */
    std::optional<const TPtrDest*> get_ptr() const;

    /**
     * @brief 尝试取得内部存储的指针
     *
     * 若存储的是整数，返回std::nullopt
     */
    std::optional<TPtrDest *> get_ptr();

    /**
     * @brief 尝试取得内部存储的整数
     *
     * 若存储的是指针，返回std::nullopt
     */
    std::optional<uint_t> get_uint() const;

    /**
     * @brief 取得内部存储的指针
     *
     * 若内部实际存储的是整数，将导致未定义行为
     */
    const TPtrDest *get_ptr_unchecked() const noexcept;

    /**
     * @brief 取得内部存储的指针
     *
     * 若内部实际存储的是整数，将导致未定义行为
     */
    TPtrDest *get_ptr_unchecked() noexcept;

    /**
     * @brief 取得内部存储的整数
     *
     * 若内部实际存储的是指针，将导致未定义行为
     */
    uint_t get_uint_unchecked() const noexcept;

    /**
     * @brief 存储指定指针，持有其所有权
     *
     * 若之前存储了其他带所有权的指针，其资源将被释放
     */
    void set(std::unique_ptr<TPtrDest> owner_ptr);

    /**
     * @brief 存储指定指针，持有其所有权
     *
     * 若之前存储了其他带所有权的指针，其资源将被释放
     */
    void set(TPtrDest *owner_ptr);

    /**
     * @brief 存储指定整数
     *
     * 若之前存储了其他带所有权的指针，其资源将被释放
     */
    void set(uint_t uint_value);
};

template<typename TUInt, typename TPtrDest>
size_t uint_ptr_variant_t<TUInt, TPtrDest>::uint_to_data(
    TUInt uint_value) noexcept
{
    return (uint_value << 8) | 0x01;
}

template<typename TUInt, typename TPtrDest>
uint_ptr_variant_t<TUInt, TPtrDest>::uint_ptr_variant_t() noexcept
    : data_(uint_to_data(0))
{
    
}

template<typename TUInt, typename TPtrDest>
uint_ptr_variant_t<TUInt, TPtrDest>::uint_ptr_variant_t(
    uint_t uint_value) noexcept
    : data_(uint_to_data(uint_value))
{
    
}

template<typename TUInt, typename TPtrDest>
uint_ptr_variant_t<TUInt, TPtrDest>::uint_ptr_variant_t(TPtrDest *owner_ptr)
{
    assert(!(size_t(owner_ptr) & 0x01));
    data_ = size_t(owner_ptr);
}

template<typename TUInt, typename TPtrDest>
uint_ptr_variant_t<TUInt, TPtrDest>::uint_ptr_variant_t(
    std::unique_ptr<TPtrDest> owner_ptr)
    : uint_ptr_variant_t(owner_ptr.get())
{
    owner_ptr.release();
}

template<typename TUInt, typename TPtrDest>
uint_ptr_variant_t<TUInt, TPtrDest>::uint_ptr_variant_t(
    const self_t &copy_from)
{
    if(copy_from.is_ptr())
    {
        std::unique_ptr<TPtrDest> clone = copy_from.get_ptr_unchecked()->clone();
        assert(!(size_t(clone.get) & 0x01));
        data_ = size_t(clone.get());
        clone.release();
    }
    else
        data_ = copy_from.get_uint_unchecked();
}

template<typename TUInt, typename TPtrDest>
uint_ptr_variant_t<TUInt, TPtrDest>::uint_ptr_variant_t(
    self_t &&move_from) noexcept
{
    this->swap(move_from);
}

template<typename TUInt, typename TPtrDest>
uint_ptr_variant_t<TUInt, TPtrDest> &
    uint_ptr_variant_t<TUInt, TPtrDest>::operator=(const self_t &copy_from)
{
    if(is_ptr())
        delete get_ptr_unchecked();

    if(copy_from.is_ptr())
    {
        std::unique_ptr<TPtrDest> clone = copy_from.get_ptr_unchecked()->clone();
        assert(!(size_t(clone.get) & 0x01));
        data_ = size_t(clone.get());
        clone.release();
    }
    else
        data_ = copy_from.get_uint_unchecked();

    return *this;
}

template<typename TUInt, typename TPtrDest>
uint_ptr_variant_t<TUInt, TPtrDest> &
    uint_ptr_variant_t<TUInt, TPtrDest>::operator=(self_t &&move_from) noexcept
{
    this->swap(move_from);
    return *this;
}

template<typename TUInt, typename TPtrDest>
uint_ptr_variant_t<TUInt, TPtrDest>::~uint_ptr_variant_t()
{
    if(is_ptr())
        delete get_ptr_unchecked();
}

template<typename TUInt, typename TPtrDest>
void uint_ptr_variant_t<TUInt, TPtrDest>::swap(self_t &swap_with) noexcept
{
    std::swap(data_, swap_with.data_);
}

template<typename TUInt, typename TPtrDest>
bool uint_ptr_variant_t<TUInt, TPtrDest>::is_ptr() const noexcept
{
    return (data_ & 0x01) == 0;
}

template<typename TUInt, typename TPtrDest>
bool uint_ptr_variant_t<TUInt, TPtrDest>::is_uint() const noexcept
{
    return !is_ptr();
}

template<typename TUInt, typename TPtrDest>
std::optional<TPtrDest *> uint_ptr_variant_t<TUInt, TPtrDest>::get_ptr()
{
    if(is_ptr())
        return reinterpret_cast<TPtrDest *>(data_);
    return std::nullopt;
}

template<typename TUInt, typename TPtrDest>
std::optional<const TPtrDest *>
    uint_ptr_variant_t<TUInt, TPtrDest>::get_ptr() const
{
    if(is_ptr())
        return reinterpret_cast<TPtrDest *>(data_);
    return std::nullopt;
}

template<typename TUInt, typename TPtrDest>
std::optional<typename uint_ptr_variant_t<TUInt, TPtrDest>::uint_t>
        uint_ptr_variant_t<TUInt, TPtrDest>::get_uint() const
{
    if(is_uint())
        return get_uint_unchecked();
    return std::nullopt;
}

template<typename TUInt, typename TPtrDest>
TPtrDest *uint_ptr_variant_t<TUInt, TPtrDest>::get_ptr_unchecked() noexcept
{
    return reinterpret_cast<TPtrDest *>(data_);
}

template<typename TUInt, typename TPtrDest>
const TPtrDest *uint_ptr_variant_t<TUInt, TPtrDest>::get_ptr_unchecked() const noexcept
{
    return reinterpret_cast<TPtrDest *>(data_);
}

template<typename TUInt, typename TPtrDest>
typename uint_ptr_variant_t<TUInt, TPtrDest>::uint_t
        uint_ptr_variant_t<TUInt, TPtrDest>::get_uint_unchecked() const noexcept
{
    return uint_t(data_ >> 8);
}

template<typename TUInt, typename TPtrDest>
void uint_ptr_variant_t<TUInt, TPtrDest>::set(
    std::unique_ptr<TPtrDest> owner_ptr)
{
    this->set(owner_ptr.get());
    owner_ptr.release();
}

template<typename TUInt, typename TPtrDest>
void uint_ptr_variant_t<TUInt, TPtrDest>::set(TPtrDest *owner_ptr)
{
    if(is_ptr())
        delete get_ptr_unchecked();
    assert(!(size_t(owner_ptr) & 0x01));
    data_ = size_t(owner_ptr);
}

template<typename TUInt, typename TPtrDest>
void uint_ptr_variant_t<TUInt, TPtrDest>::set(uint_t uint_value)
{
    if(is_ptr())
        delete get_ptr_unchecked();
    data_ = uint_to_data(uint_value);
}

} // namespace agz::misc
