#pragma once

#include "record.hpp"

namespace miu::asp {

struct layout {
    uint32_t size;
    uint32_t padding[15];

    auto records() const { return (record*)(this + 1); }
    auto keys() const { return (char*)(records() + size); }
};
static_assert(CACHE_LINE == sizeof(layout));

}    // namespace miu::asp
