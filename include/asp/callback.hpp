#pragma once

#include <log/log.hpp>

#include "wrapper.hpp"

namespace miu::asp {

class callback {
  public:
    using getter = std::function<com::variant()>;
    using setter = std::function<void(com::variant)>;

    bool reset(getter const&);
    com::variant get() const;

    bool reset(setter const&);
    bool set(com::variant);

  public:
    template<typename F, typename... ARGS>
    static getter make_getter(F const& func, ARGS&&... args) {
        return std::bind(&get_wrapper<F>::bounce, func, std::forward<ARGS>(args)...);
    }

    template<typename F, typename... ARGS>
    static setter make_setter(F const& func, ARGS&&... args) {
        using namespace std::placeholders;
        return std::bind(&set_wrapper<F>::bounce, func, std::forward<ARGS>(args)..., _1);
    }

  private:
    getter _get;
    setter _set;
};

}    // namespace miu::asp
