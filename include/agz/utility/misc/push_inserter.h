#pragma once

#include <iterator>

namespace agz::misc
{

namespace misc_impl
{
    template <class Container>
    class push_insert_iterator
    {
    protected:
    
        Container *container_;
    
    public:
    
        using iterator_category = std::output_iterator_tag;
        using value_type        = void;
        using difference_type   = void;
        using pointer           = void;
        using reference         = void;
        using container_type    = Container;
    
        explicit push_insert_iterator(Container &c) noexcept
            : container_(&c)
        {
            
        }
        
        push_insert_iterator<Container> &operator=(
            typename Container::const_reference value)
        {
            container_->push(value);
            return *this;
        }
    
        push_insert_iterator<Container> &operator*()
        {
            return *this;
        }
        
        push_insert_iterator<Container> &operator++()
        {
            return *this;
        }
        
        push_insert_iterator<Container> operator++(int)
        {
            return *this;
        }
    };
    
    template <class Container>
    class direct_insert_iterator
    {
    protected:
    
        Container *container_;
    
    public:
    
        using iterator_category = std::output_iterator_tag;
        using value_type        = void;
        using difference_type   = void;
        using pointer           = void;
        using reference         = void;
        using container_type    = Container;
    
        explicit direct_insert_iterator(Container &c) noexcept
            : container_(&c)
        {
            
        }
        
        direct_insert_iterator<Container> &operator=(
            typename Container::const_reference value)
        {
            container_->insert(value);
            return *this;
        }
    
        direct_insert_iterator<Container> &operator*()
        {
            return *this;
        }
        
        direct_insert_iterator<Container> &operator++()
        {
            return *this;
        }
        
        direct_insert_iterator<Container> operator++(int)
        {
            return *this;
        }
    };
}

/**
 * @brief 创建一个调用容器的push方法的output iterator
 */
template<typename Container>
misc_impl::push_insert_iterator<Container> push_inserter(Container &container)
{
    return misc_impl::push_insert_iterator<Container>(container);
}

/**
 * @brief 创建一个调用容器的insert方法的output iterator
 */
template<typename Container>
misc_impl::direct_insert_iterator<Container> direct_inserter(Container &container)
{
    return misc_impl::direct_insert_iterator<Container>(container);
}

} // namespace agz::misc
