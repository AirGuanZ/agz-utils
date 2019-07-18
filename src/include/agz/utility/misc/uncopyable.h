#pragma once

namespace agz::misc
{
    
/**
 * @brief ���ɸ�������
 * 
 * ���̳д�������Ĭ�ϸ��ƹ���/��ֵ����
 */
class uncopyable_t
{
public:

    uncopyable_t()                                   = default;
    uncopyable_t(const uncopyable_t&)                = delete;
    uncopyable_t &operator=(const uncopyable_t&)     = delete;
    uncopyable_t(uncopyable_t&&)            noexcept = default;
    uncopyable_t &operator=(uncopyable_t&&) noexcept = default;
};

} // namespace agz::misc
