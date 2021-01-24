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
    auto value() const { return _var; }

  private:
    com::variant _var;
    uint32_t _ver { 0 };
    uint32_t _padding[3];
};
static_assert(sizeof(record) == 32);

}    // namespace miu::asp
