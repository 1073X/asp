#pragma once

#include <com/variant.hpp>
#include <functional>
#include <log/log.hpp>

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

  private:
    getter _get;
    setter _set;
};

}    // namespace miu::asp
