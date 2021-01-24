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
    auto size() const {
        auto l = (layout*)_buf.data();
        return l->size;
    }

    auto reset(nlohmann::json const& keys) {
        std::ostringstream ss;
        ss << keys;
        auto raw = ss.str();

        auto records_count = collect(keys);
        auto records_total = sizeof(record) * records_count;
        std::vector<record> records(records_count, record());

        _buf.resize(sizeof(com::variant) * records_count + raw.size());
        auto l = new (_buf.data()) layout { records_count };
        std::memcpy(l->records(), records.data(), records_total);
        std::strncpy(l->keys(), raw.c_str(), raw.size());
    }

    auto& operator[](uint32_t idx) {
        auto l = (layout*)_buf.data();
        if (idx < l->size) {
            return l->records()[idx];
        }

        log::error(name(), +"access overflow", idx, +">=", l->size);
        static record dummy;
        return dummy;
    }

  private:
    uint32_t collect(nlohmann::json const& json) const {
        auto count = 0U;
        for (auto const& [key, val] : json.items()) {
            if (val.is_number_integer()) {
                count++;
            } else if (val.is_structured()) {
                count += collect(val);
            } else {
                log::error(name(), +"illegal index", key);
            }
        }
        return count;
    }

    nlohmann::json keys() const {
        auto l = (layout const*)_buf.data();
        return nlohmann::json::parse(l->keys());
    }

    record const* records() const {
        auto l = (layout const*)_buf.data();
        return l->records();
    }

  private:
    mutable shm::buffer _buf;
};

}    // namespace miu::asp
