#pragma once

#include <com/variant.hpp>
#include <nlohmann/json.hpp>
#include <shm/buffer.hpp>

#include "layout.hpp"

using json = nlohmann::json;

namespace miu::asp {

extern json default_camera(com::variant const&);

class database {
  public:
    database(std::string_view name, uint32_t size);    // make
    database(std::string_view name, shm::mode);        // open

    void reset(json const& keys);

    std::string name() const;
    uint32_t size() const;

    json keys() const;
    record& operator[](uint32_t);

    using camera_type = std::function<json(com::variant const&)>;
    json capture(uint32_t version, camera_type const& = default_camera) const;

  private:
    uint32_t capture_object(uint32_t ver, json const& keys, json& data, camera_type const&) const;
    uint32_t capture_array(uint32_t ver, json const& keys, json& data, camera_type const&) const;

    record const* records() const;

  private:
    mutable shm::buffer _buf;
};

}    // namespace miu::asp
