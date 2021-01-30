#pragma once

#include <com/variant.hpp>
#include <vector>

#include "callback.hpp"

namespace miu::asp {

class asp {
  public:
    template<typename F, typename... ARGS>
    static void read(std::vector<com::variant> const& keys, F const& f, ARGS&&... args) {
        insert_getter(keys, callback::make_getter(f, std::forward<ARGS>(args)...));
    }

    template<typename F, typename... ARGS>
    static void load(std::vector<com::variant> const& keys, F const& f, ARGS&&... args) {
        insert_setter(keys, callback::make_setter(f, std::forward<ARGS>(args)...));
    }

    static void reset(std::string_view = "");
    static void dump();

  private:
    static void insert_getter(std::vector<com::variant> const&, callback::getter const&);
    static void insert_setter(std::vector<com::variant> const&, callback::setter const&);
};

}    // namespace miu::asp
