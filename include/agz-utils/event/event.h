#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <set>
#include <type_traits>
#include <tuple>
#include <unordered_set>

#include "../misc/uncopyable.h"

namespace agz::event
{

template<typename Event>
class receiver_set_t;

template<typename Event>
class receiver_t :
    public std::enable_shared_from_this<receiver_t<Event>>,
    public misc::uncopyable_t
{
    friend class receiver_set_t<Event>;

    std::set<receiver_set_t<Event> *> contained_sets_;
    std::set<receiver_set_t<Event> *> owned_contained_sets_;

public:

    virtual ~receiver_t();

    virtual void handle(const Event &event) = 0;
};

template<typename Event>
class receiver_set_t : public misc::uncopyable_t
{
    std::unordered_set<receiver_t<Event> *>                handlers_;
    std::unordered_set<std::shared_ptr<receiver_t<Event>>> owned_handlers_;

public:

    ~receiver_set_t();

    void send(const Event &e) const;

    void attach(receiver_t<Event> *handler);

    void attach(std::shared_ptr<receiver_t<Event>> owned_handler);

    void detach(receiver_t<Event> *handler);

    void detach(const std::shared_ptr<receiver_t<Event>> &owned_handler);

    void detach_all();
};

template<typename...Events>
class sender_t : public misc::uncopyable_t
{
    std::tuple<receiver_set_t<Events>...> sets_;

public:

    template<typename Event>
    void send(const Event &e) const;

    template<typename Event>
    void attach(receiver_t<Event> *handler);

    template<typename Event>
    void attach(std::shared_ptr<receiver_t<Event>> owned_handler);

    template<typename Event>
    void detach(receiver_t<Event> *handler);

    template<typename Event>
    void detach(const std::shared_ptr<receiver_t<Event>> &owned_handler);

    template<typename Event>
    void detach_all();

    void detach_all_types();
};

template<typename Event>
class functional_receiver_t : public receiver_t<Event>
{
public:

    using function_t = std::function<void(const Event &)>;

    explicit functional_receiver_t(std::function<void()> f);

    explicit functional_receiver_t(function_t f = function_t());

    void set_function(std::function<void()> f);

    void set_function(function_t f);

    void handle(const Event &e) override;

private:

    function_t f_;
};

template<typename Event, typename Class,
         typename = std::enable_if_t<std::is_class_v<Class>>>
class class_receiver_t : public receiver_t<Event>
{
public:

    using mem_func_ptr_t = void(Class:: *)(const Event &);

    class_receiver_t() noexcept : class_receiver_t(nullptr, nullptr) { }

    class_receiver_t(Class *c, mem_func_ptr_t m) noexcept : c_(c), m_(m) { }

    void set_class_instance(Class *c) noexcept { c_ = c; }

    void set_mem_func(mem_func_ptr_t m) noexcept { m_ = m; }

    void handle(const Event &e) override { if(c_ && m_) (c_->*m_)(e); }

private:

    Class *c_;
    mem_func_ptr_t m_;
};

#define AGZ_DECL_EVENT_SENDER_HANDLER(EventSender, EventName)                   \
    void attach(agz::event::receiver_t<EventName> *handler)                     \
        { EventSender.attach(handler); }                                        \
    void attach(std::shared_ptr<agz::event::receiver_t<EventName>> handler)     \
        { EventSender.attach(std::move(handler)); }                             \
    void attach(std::function<void(const EventName&)> f)                        \
        { this->attach(std::make_shared<                                        \
            agz::event::functional_receiver_t<EventName>>(std::move(f))); }     \
    void detach(agz::event::receiver_t<EventName> *handler)                     \
        { EventSender.detach(handler); }                                        \
    void detach(std::shared_ptr<agz::event::receiver_t<EventName>> handler)     \
        { EventSender.detach(handler); }

template<typename Event>
receiver_t<Event>::~receiver_t()
{
    while(!contained_sets_.empty())
        (*contained_sets_.begin())->detach(this);

    if(!owned_contained_sets_.empty())
    {
        auto shared_this = this->shared_from_this();
        while(!owned_contained_sets_.empty())
            (*owned_contained_sets_.begin())->detach(this);
    }
}

template<typename Event>
receiver_set_t<Event>::~receiver_set_t()
{
    detach_all();
}

template<typename Event>
void receiver_set_t<Event>::send(const Event &e) const
{
    for(auto h : handlers_)
        h->handle(e);

    for(auto &owned_h : owned_handlers_)
        owned_h->handle(e);
}

template<typename Event>
void receiver_set_t<Event>::attach(receiver_t<Event> *handler)
{
    assert(handler);
    handler->contained_sets_.insert(this);
    handlers_.insert(handler);
}

template<typename Event>
void receiver_set_t<Event>::attach(
    std::shared_ptr<receiver_t<Event>> owned_handler)
{
    assert(owned_handler);
    owned_handler->owned_contained_sets_.insert(this);
    owned_handlers_.insert(std::move(owned_handler));
}

template<typename Event>
void receiver_set_t<Event>::detach(receiver_t<Event> *handler)
{
    assert(handler);
    handler->contained_sets_.erase(this);
    handlers_.erase(handler);
}

template<typename Event>
void receiver_set_t<Event>::detach(
    const std::shared_ptr<receiver_t<Event>> &owned_handler)
{
    assert(owned_handler);
    owned_handler->owned_contained_sets_.erase(this);
    owned_handlers_.erase(owned_handler);
}

template<typename Event>
void receiver_set_t<Event>::detach_all()
{
    while(!handlers_.empty())
        this->detach(*handlers_.begin());

    while(!owned_handlers_.empty())
        this->detach(*owned_handlers_.begin());
}

template<typename...Events>
template<typename Event>
void sender_t<Events...>::send(const Event &e) const
{
    std::get<receiver_set_t<Event>>(sets_).send(e);
}

template<typename...Events>
template<typename Event>
void sender_t<Events...>::attach(receiver_t<Event> *handler)
{
    std::get<receiver_set_t<Event>>(sets_).attach(handler);
}

template<typename...Events>
template<typename Event>
void sender_t<Events...>::attach(
    std::shared_ptr<receiver_t<Event>> owned_handler)
{
    std::get<receiver_set_t<Event>>(sets_).attach(std::move(owned_handler));
}

template<typename...Events>
template<typename Event>
void sender_t<Events...>::detach(receiver_t<Event> *handler)
{
    std::get<receiver_set_t<Event>>(sets_).detach(handler);
}

template<typename...Events>
template<typename Event>
void sender_t<Events...>::detach(
    const std::shared_ptr<receiver_t<Event>> &owned_handler)
{
    std::get<receiver_set_t<Event>>(sets_).detach(std::move(owned_handler));
}

template<typename...Events>
template<typename Event>
void sender_t<Events...>::detach_all()
{
    std::get<receiver_set_t<Event>>(sets_).detach_all();
}

template<typename...Events>
void sender_t<Events...>::detach_all_types()
{
    std::apply(
        [](auto &&...s) { ((s.detach_all()), ...); },
        sets_);
}

template<typename Event>
functional_receiver_t<Event>::functional_receiver_t(std::function<void()> f)
{
    if(f)
        f_ = [nf = std::move(f)](const Event &) { nf(); };
}

template<typename Event>
functional_receiver_t<Event>::functional_receiver_t(function_t f)
    : f_(std::move(f))
{
    
}

template<typename Event>
void functional_receiver_t<Event>::set_function(std::function<void()> f)
{
    if(f)
        f_ = [nf = std::move(f)](const Event &) { nf(); };
    else
        f_ = function_t();
}

template<typename Event>
void functional_receiver_t<Event>::set_function(function_t f)
{
    f_ = std::move(f);
}

template<typename Event>
void functional_receiver_t<Event>::handle(const Event &e)
{
    if(f_)
        f_(e);
}

} // namespace agz::event
