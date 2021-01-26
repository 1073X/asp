#pragma once

#include <com/variant.hpp>
#include <functional>

namespace miu::asp {

class callback {
  public:
    using getter = std::function<com::variant()>;

    auto reset(getter const& get) {
        if (!_get) {
            _get = get;
            return true;
        }
        return false;
    }

    auto get() const { return _get(); }

  private:
    getter _get;
};

}    // namespace miu::asp
