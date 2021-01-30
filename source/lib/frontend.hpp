#pragma once

#include <com/fatal_error.hpp>
#include <com/variant.hpp>
#include <nlohmann/json.hpp>
#include <vector>

#include "asp/callback.hpp"

using nlohmann::json;

namespace miu::asp {

class database;

class frontend {
  public:
    json const& keys() const;
    uint32_t size() const;
    callback& at(uint32_t idx);

    bool insert_getter(std::vector<com::variant> const&, callback::getter const&);
    bool insert_setter(std::vector<com::variant> const&, callback::setter const&);

    void reset(database&);

  private:
    std::vector<callback> _cbs;
    json _keys;
};

}    // namespace miu::asp
