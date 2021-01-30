#pragma once

#include <com/variant.hpp>
#include <functional>

namespace miu::asp {

template<typename>
struct wrapper;

template<typename R, typename T, typename... ARGS>
struct wrapper<R (T::*)(ARGS...) const> {
    static com::variant bounce(R (T::*f)(ARGS...) const, T const* t, ARGS... args) {
        return (t->*f)(args...);
    }
};

template<typename R, typename T, typename... ARGS>
struct wrapper<R (T::*)(ARGS...)> {
    static com::variant bounce(R (T::*f)(ARGS...), T* t, ARGS... args) { return (t->*f)(args...); }
};

template<typename R, typename... ARGS>
struct wrapper<R (*)(ARGS...)> {
    static com::variant bounce(R (*f)(ARGS...), ARGS... args) {
        return (*f)(std::forward<ARGS>(args)...);
    }
};

template<typename T, typename A>
struct wrapper<void (T::*)(A)> {
    static void bounce(void (T::*f)(A), T* t, com::variant var) { (t->*f)(var.get<A>().value()); }
};

template<typename A>
struct wrapper<void (*)(A)> {
    static void bounce(void (*f)(A), com::variant var) { (*f)(var.get<A>().value()); }
};

}    // namespace miu::asp
