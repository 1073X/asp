#pragma once

#include <com/variant.hpp>

namespace miu::asp {

class record {
  public:
    template<typename T>
    auto operator=(T const& v) {
        auto var = com::variant { v };
        if (var != _var) {
            _var = var;
            _ver++;
        }
    }

    auto version() const { return _ver; }

    auto id() const { return _var.id(); }

    template<typename T>
    auto get() const {
        return _var.get<T>();
    }

    auto padding() { return _padding; }

  private:
    com::variant _var;
    uint32_t _ver { 0 };
    uint32_t _padding[3] {};
};
static_assert(sizeof(record) == 32);

}    // namespace miu::asp
