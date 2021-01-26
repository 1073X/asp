#pragma once

#include <com/variant.hpp>
#include <functional>

namespace miu::asp {

struct callback {
    using getter = std::function<com::variant()>;
    getter get;
};

}    // namespace miu::asp
