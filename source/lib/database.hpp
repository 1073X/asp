#pragma once

#include <com/json.hpp>
#include <com/variant.hpp>
#include <shm/buffer.hpp>

#include "layout.hpp"

namespace miu::asp {

extern com::json default_camera(com::variant const&);

class database {
  public:
    database(std::string_view name, uint32_t size);    // make
    database(std::string_view name, shm::mode);        // open

    void reset(com::json const& keys);

    std::string name() const;
    uint32_t size() const;

    com::json keys() const;
    record& operator[](uint32_t);

    using camera_type = std::function<com::json(com::variant const&)>;
    com::json capture(uint32_t version, camera_type const& = default_camera) const;

  private:
    uint32_t
    capture_object(uint32_t ver, com::json const& keys, com::json& data, camera_type const&) const;
    uint32_t
    capture_array(uint32_t ver, com::json const& keys, com::json& data, camera_type const&) const;

    record const* records() const;

  private:
    mutable shm::buffer _buf;
};

}    // namespace miu::asp
