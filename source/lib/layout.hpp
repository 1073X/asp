#pragma once

#include <com/variant.hpp>

namespace miu::asp {

struct layout {
    uint32_t size;
    char padding[60];

    auto vals() const { return (com::variant*)(this + 1); }
    auto keys() const { return (char*)(vals() + size); }
};
static_assert(CACHE_LINE == sizeof(layout));

}    // namespace miu::asp
