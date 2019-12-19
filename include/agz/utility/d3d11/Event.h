#pragma once

#include <cassert>
#include <functional>
#include <set>
#include <tuple>
#include <unordered_set>

#include <agz/utility/misc.h>

#include <agz/utility/d3d11/Common.h>

AGZ_D3D11_BEGIN

template<typename Event>
class EventHandlerSet;

template<typename Event>
class EventHandler : public agz::misc::uncopyable_t
{
    friend class EventHandlerSet<Event>;

    std::set<EventHandlerSet<Event>*> containedSets_;

public:

    virtual ~EventHandler();

    virtual void Handle(const Event &param) = 0;
};

template<typename Event>
class EventHandlerSet : public agz::misc::uncopyable_t
{
    std::unordered_set<EventHandler<Event>*> handlers_;

public:

    ~EventHandlerSet()
    {
        DetachAll();
    }

    void InvokeAllHandlers(const Event &param) const
    {
        for(auto handler : handlers_)
        {
            handler->Handle(param);
        }
    }

    void Attach(EventHandler<Event> *handler)
    {
        assert(handler);
        handler->containedSets_.insert(this);
        handlers_.insert(handler);
    }

    void Detach(EventHandler<Event> *handler)
    {
        assert(handler);
        handler->containedSets_.erase(this);
        handlers_.erase(handler);
    }

    void DetachAll()
    {
        while(!handlers_.empty())
        {
            Detach(*handlers_.begin());
        }
    }
};

template<typename Event>
EventHandler<Event>::~EventHandler()
{
    while(!containedSets_.empty())
        (*containedSets_.begin())->Detach(this);
}

template<typename...Events>
class EventManager : public agz::misc::uncopyable_t
{
    std::tuple<EventHandlerSet<Events>...> handlerSets_;

protected:

    template<typename Event>
    void InvokeAllHandlers(const Event &param)
    {
        std::get<EventHandlerSet<Event>>(handlerSets_).InvokeAllHandlers(param);
    }

public:

    template<typename Event>
    void Attach(EventHandler<Event> *handler)
    {
        std::get<EventHandlerSet<Event>>(handlerSets_).Attach(handler);
    }

    template<typename Event>
    void Detach(EventHandler<Event> *handler)
    {
        std::get<EventHandlerSet<Event>>(handlerSets_).Detach(handler);
    }

    template<typename Event>
    void DetachAll()
    {
        std::get<EventHandlerSet<Event>>(handlerSets_).DetachAll();
    }

    void DetachAllTypes()
    {
        std::apply(
            [](auto&&...handlerSet){ ((handlerSet.DetachAll()), ...); },
            handlerSets_);
    }
};

// ========== predefined event handlers ==========

template<typename Event>
class FunctionalEventHandler : public EventHandler<Event>
{
public:

    using Function = std::function<void(const Event &)>;

    explicit FunctionalEventHandler(Function f = Function())
        : func_(std::move(f))
    {
        
    }

    void SetFunction(Function f)
    {
        func_ = std::move(f);
    }

    void Handle(const Event &param) override
    {
        if(func_)
        {
            func_(param);
        }
    }

private:

    Function func_;
};

template<typename Event, typename Class, typename = std::enable_if_t<std::is_class_v<Class>>>
class ClassEventHandler : public EventHandler<Event>
{
public:

    using MemberFunctionPtr = void (Class::*)(const Event &);

    ClassEventHandler() noexcept
        : ClassEventHandler(nullptr, nullptr)
    {
        
    }

    ClassEventHandler(Class *c, MemberFunctionPtr m) noexcept
        : classInstance_(c), memberFunc_(m)
    {

    }

    void SetClassInstance(Class *instance) noexcept
    {
        classInstance_ = instance;
    }

    void SetMemberFunction(MemberFunctionPtr func) noexcept
    {
        memberFunc_ = func;
    }

    void Handle(const Event &e) override
    {
        if(classInstance_ && memberFunc_)
        {
            classInstance_->memberFunc(e);
        }
    }

private:

    Class *classInstance_;
    MemberFunctionPtr memberFunc_;
};

AGZ_D3D11_END
