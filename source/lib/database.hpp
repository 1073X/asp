#pragma once

#include <com/variant.hpp>
#include <nlohmann/json.hpp>
#include <shm/buffer.hpp>

#include "layout.hpp"

namespace miu::asp {

class database {
  public:
    database(std::string_view name)
        : _buf({ name, "asp" }) {}

    com::variant get(std::string_view key) const {
        int32_t idx = keys()[key.data()];
        return vals()[idx];
    }

  private:
    nlohmann::json keys() const {
        auto l = (layout const*)_buf.data();
        return nlohmann::json::parse(l->keys());
    }

    com::variant const* vals() const {
        auto l = (layout const*)_buf.data();
        return l->vals();
    }

  private:
    mutable shm::buffer _buf;
};

}    // namespace miu::asp
