#pragma once

#include <com/variant.hpp>

namespace miu::asp {

class record {
  public:
    template<typename T>
    auto set(T const& v) {
        auto var = com::variant { v };
        if (var != _var) {
            _var = var;
            _ver++;
        }
    }

    auto version() const { return _ver; }
    auto variant() const { return _var; }
    auto padding() { return _padding; }

  private:
    com::variant _var;
    uint32_t _ver { 0 };
    char _padding[12] { '-' };
};
static_assert(sizeof(record) == 32);

}    // namespace miu::asp
