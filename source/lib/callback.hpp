#pragma once

#include <log/log.hpp>

#include "wrapper.hpp"

namespace miu::asp {

class callback {
  public:
    using getter = std::function<com::variant()>;
    using setter = std::function<void(com::variant)>;

    auto reset(getter const& get) {
        if (!_get) {
            _get = get;
            return true;
        }
        return false;
    }

    auto get() const {
        if (!_get) {
            log::error(+"unsupported getter");
            return com::variant {};
        }
        return _get();
    }

    auto reset(setter const& set) {
        if (!_set) {
            _set = set;
            return true;
        }
        return false;
    }

    auto set(com::variant v) {
        if (!_set) {
            log::error(+"unsupported setter");
            return;
        }
        _set(v);
    }

  public:
    template<typename F, typename... ARGS>
    static getter make_getter(F const& func, ARGS&&... args) {
        return std::bind(&wrapper<F>::bounce, func, std::forward<ARGS>(args)...);
    }

    template<typename F, typename... ARGS>
    static setter make_setter(F const& func, ARGS&&... args) {
        using namespace std::placeholders;
        return std::bind(&wrapper<F>::bounce, func, std::forward<ARGS>(args)..., _1);
    }

  private:
    getter _get;
    setter _set;
};

}    // namespace miu::asp
