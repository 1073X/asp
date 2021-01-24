#pragma once

#include <com/variant.hpp>
#include <log/log.hpp>
#include <nlohmann/json.hpp>
#include <shm/buffer.hpp>

#include "layout.hpp"

using json = nlohmann::json;

namespace miu::com {
template<>
std::optional<json> variant::get<json>() const;
}

namespace miu::asp {

class database {
  public:
    database(std::string_view name)
        : _buf({ name, "asp" }, 4096) {}

    auto reset(json const& keys) {
        std::ostringstream ss;
        ss << keys;
        auto raw = ss.str();

        auto cnt = collect(keys);
        auto len = sizeof(record) * cnt;

        _buf.resize(len + raw.size());
        auto l = new (_buf.data()) layout { cnt };
        std::memcpy(l->records(), std::vector(cnt, record()).data(), len);
        std::strncpy(l->keys(), raw.c_str(), raw.size());
    }

    auto name() const { return _buf.name(); }
    auto size() const {
        auto l = (layout*)_buf.data();
        return l->size;
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

    auto capture(uint32_t version) const {
        json data(json::value_t::object);
        data["_VER_"] = cap_object(version, keys(), data);
        return data;
    }

  private:
    uint32_t collect(json const& keys) const {
        auto count = 0U;
        for (auto const& [key, val] : keys.items()) {
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

    uint32_t cap_object(uint32_t ver, json const& keys, json& data) const {
        auto max_ver = 0U;
        for (auto const& [key, val] : keys.items()) {
            if (val.is_number_integer()) {
                auto rec = records()[val];
                if (rec.version() > ver) {
                    data[key] = rec.get<json>().value();
                    max_ver   = std::max(max_ver, rec.version());
                }
            } else if (val.is_object()) {
                json object(json::value_t::object);
                max_ver   = std::max(max_ver, cap_object(ver, val, object));
                data[key] = object;
            } else if (val.is_array()) {
                json array(json::value_t::array);
                max_ver   = std::max(max_ver, cap_array(ver, val, array));
                data[key] = array;
            }
        }
        return max_ver;
    }

    uint32_t cap_array(uint32_t ver, json const& keys, json& data) const {
        auto max_ver = 0U;
        for (auto const& [key, val] : keys.items()) {
            if (val.is_number_integer()) {
                auto rec = records()[val];
                if (rec.version() > ver) {
                    data.push_back(rec.get<json>().value());
                    max_ver = std::max(max_ver, rec.version());
                }
            } else if (val.is_object()) {
                json object(json::value_t::object);
                max_ver = std::max(max_ver, cap_object(ver, val, object));
                data.push_back(object);
            } else if (val.is_array()) {
                json array(json::value_t::array);
                max_ver = std::max(max_ver, cap_array(ver, val, array));
                data.push_back(array);
            }
        }
        return max_ver;
    }

    json keys() const {
        auto l = (layout const*)_buf.data();
        return json::parse(l->keys());
    }

    record const* records() const {
        auto l = (layout const*)_buf.data();
        return l->records();
    }

  private:
    mutable shm::buffer _buf;
};

}    // namespace miu::asp
