#pragma once

#include <com/fatal_error.hpp>
#include <com/json.hpp>
#include <com/variant.hpp>
#include <vector>

#include "asp/callback.hpp"

namespace miu::asp {

class database;

class frontend {
  public:
    com::json const& keys() const;
    uint32_t size() const;
    callback& at(uint32_t idx);

    bool insert_getter(std::vector<com::variant> const&, callback::getter const&);
    bool insert_setter(std::vector<com::variant> const&, callback::setter const&);

    void reset(database&);
    void clear();

  private:
    std::vector<callback> _cbs;
    com::json _keys;
};

}    // namespace miu::asp
