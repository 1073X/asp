#pragma once

#include <com/variant.hpp>

namespace miu::asp {

struct layout {
    uint32_t size;
    char padding[12];

    auto vals() const { return (com::variant*)(this + 1); }
    auto keys() const { return (char*)(vals() + size); }
};
static_assert(sizeof(com::variant) == sizeof(layout));

}    // namespace miu::asp
