#pragma once

#include <com/variant.hpp>
#include <nlohmann/json.hpp>
#include <shm/buffer.hpp>

#include "layout.hpp"

using json = nlohmann::json;

namespace miu::asp {

class database {
  public:
    database(std::string_view name);

    void reset(json const& keys);

    std::string name() const;
    uint32_t size() const;

    record& operator[](uint32_t idx);

    json capture(uint32_t version) const;

  private:
    uint32_t collect(json const& keys) const;

    uint32_t cap_object(uint32_t ver, json const& keys, json& data) const;
    uint32_t cap_array(uint32_t ver, json const& keys, json& data) const;

    json keys() const;
    record const* records() const;

  private:
    mutable shm::buffer _buf;
};

}    // namespace miu::asp
