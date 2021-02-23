#pragma once

#include <com/variant.hpp>
#include <functional>

namespace miu::asp {

///// Getter

// lambda
template<typename F>
struct get_wrapper {
    static com::variant bounce(F const& f) { return f(); }
};

// global
template<typename R, typename... ARGS>
struct get_wrapper<R (*)(ARGS...)> {
    static com::variant bounce(R (*f)(ARGS...), ARGS... args) {
        return (*f)(std::forward<ARGS>(args)...);
    }
};

// member
template<typename R, typename T, typename... ARGS>
struct get_wrapper<R (T::*)(ARGS...)> {
    static com::variant bounce(R (T::*f)(ARGS...), T* t, ARGS... args) { return (t->*f)(args...); }
};

// const member
template<typename R, typename T, typename... ARGS>
struct get_wrapper<R (T::*)(ARGS...) const> {
    static com::variant bounce(R (T::*f)(ARGS...) const, T const* t, ARGS... args) {
        return (t->*f)(args...);
    }
};

///// Setter

// lambda
template<typename A>
struct set_wrapper {
    static void bounce(std::function<void(A)> const& f, com::variant var) {
        f(var.get<A>().value());
    }
};

// global
template<typename A>
struct set_wrapper<void (*)(A)> {
    static void bounce(void (*f)(A), com::variant var) { (*f)(var.get<A>().value()); }
};

// member
template<typename T, typename A>
struct set_wrapper<void (T::*)(A)> {
    static void bounce(void (T::*f)(A), T* t, com::variant var) { (t->*f)(var.get<A>().value()); }
};

}    // namespace miu::asp
