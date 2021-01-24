#pragma once

#include <com/variant.hpp>
#include <log/log.hpp>
#include <nlohmann/json.hpp>
#include <shm/buffer.hpp>

#include "layout.hpp"

namespace miu::asp {

class database {
  public:
    database(std::string_view name)
        : _buf({ name, "asp" }, 4096) {}

    auto name() const { return _buf.name(); }

    auto reset(nlohmann::json const& keys) {
        std::ostringstream ss;
        ss << keys;
        auto raw = ss.str();

        auto vals_count = collect(keys);
        auto vals_total = sizeof(com::variant) * vals_count;
        std::vector<com::variant> vals(vals_count, com::variant());

        _buf.resize(sizeof(com::variant) * vals_count + raw.size());
        auto l = new (_buf.data()) layout { vals_count };
        std::memcpy(l->vals(), vals.data(), vals_total);
        std::strncpy(l->keys(), raw.c_str(), raw.size());
    }

    com::variant get(std::string_view key) const {
        int32_t idx = keys()[key.data()];
        return vals()[idx];
    }

  private:
    uint32_t collect(nlohmann::json const& json) const {
        auto vals = 0U;
        for (auto const& [key, val] : json.items()) {
            if (val.is_number_integer()) {
                vals++;
            } else if (val.is_structured()) {
                vals += collect(val);
            } else {
                log::error(name(), +"illegal index", key);
            }
        }
        return vals;
    }

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
