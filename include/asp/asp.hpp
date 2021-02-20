#pragma once

#include <com/variant.hpp>
#include <vector>

#include "callback.hpp"

namespace miu::asp {

//// Frontend

// setter
template<typename F, typename... ARGS>
void read(std::vector<com::variant> const& keys, F const& f, ARGS&&... args) {
    extern void insert_getter(std::vector<com::variant> const&, callback::getter const&);
    insert_getter(keys, callback::make_getter(f, std::forward<ARGS>(args)...));
}

// getter
template<typename F, typename... ARGS>
void load(std::vector<com::variant> const& keys, F const& f, ARGS&&... args) {
    extern void insert_setter(std::vector<com::variant> const&, callback::setter const&);
    insert_setter(keys, callback::make_setter(f, std::forward<ARGS>(args)...));
}

//// Backend

// reset backend shmem buffer
extern void reset(std::string_view = "");
// dump aspects to shmem buffer
extern void dump();

}    // namespace miu::asp
