
#include "database.hpp"

#include <log/log.hpp>
#include <sstream>

namespace miu::com {
template<>
std::optional<json> variant::get<json>() const;
}

namespace miu::asp {

com::json default_camera(com::variant const& var) {
    return var.get<com::json>().value();
}

static uint32_t collect(std::string_view name, com::json const& keys) {
    auto count = 0U;
    for (auto const& [key, val] : keys.items()) {
        if (val.is_number_integer()) {
            count++;
        } else if (val.is_structured()) {
            count += collect(name, val);
        } else {
            log::error(name, +"illegal index", key);
        }
    }
    return count;
}

database::database(std::string_view name, uint32_t size)
    : _buf({ name, "asp" }, size) {
}

database::database(std::string_view name, shm::mode mode)
    : _buf({ name, "asp" }, mode) {
}

void database::reset(com::json const& keys) {
    std::ostringstream ss;
    ss << keys;
    auto raw = ss.str();

    auto cnt = collect(name(), keys);
    auto len = sizeof(record) * cnt;

    _buf.resize(len + raw.size() + 1);
    auto l  = new (_buf.data()) layout {};
    l->size = cnt;
    std::memcpy(l->records(), std::vector(cnt, record()).data(), len);
    std::strncpy(l->keys(), raw.c_str(), raw.size());
    l->keys()[raw.size()] = '\0';
}

std::string database::name() const {
    return _buf.name();
}

uint32_t database::size() const {
    auto l = (layout*)_buf.data();
    return l->size;
}

record& database::operator[](uint32_t idx) {
    auto l = (layout*)_buf.data();
    if (idx < l->size) {
        return l->records()[idx];
    }

    log::error(name(), +"access overflow", idx, +">=", l->size);
    static record dummy;
    return dummy;
}

com::json database::capture(uint32_t version, camera_type const& camera) const {
    com::json data(com::json::value_t::object);
    data["_VER_"] = capture_object(version, keys(), data, camera);
    return data;
}

uint32_t database::capture_object(uint32_t ver,
                                  com::json const& keys,
                                  com::json& data,
                                  camera_type const& camera) const {
    auto max_ver = 0U;
    for (auto const& [key, val] : keys.items()) {
        if (val.is_number_integer()) {
            auto rec = records()[val];
            if (rec.version() > ver) {
                data[key] = camera(rec.variant());
                max_ver   = std::max(max_ver, rec.version());
            }
        } else if (val.is_object()) {
            com::json object(com::json::value_t::object);
            max_ver   = std::max(max_ver, capture_object(ver, val, object, camera));
            data[key] = object;
        } else if (val.is_array()) {
            com::json array(com::json::value_t::array);
            max_ver   = std::max(max_ver, capture_array(ver, val, array, camera));
            data[key] = array;
        }
    }
    return max_ver;
}

uint32_t database::capture_array(uint32_t ver,
                                 com::json const& keys,
                                 com::json& data,
                                 camera_type const& camera) const {
    auto max_ver = 0U;
    for (auto const& [key, val] : keys.items()) {
        if (val.is_number_integer()) {
            auto rec = records()[val];
            if (rec.version() > ver) {
                data.push_back(camera(rec.variant()));
                max_ver = std::max(max_ver, rec.version());
            }
        } else if (val.is_object()) {
            com::json object(com::json::value_t::object);
            max_ver = std::max(max_ver, capture_object(ver, val, object, camera));
            data.push_back(object);
        } else if (val.is_array()) {
            com::json array(com::json::value_t::array);
            max_ver = std::max(max_ver, capture_array(ver, val, array, camera));
            data.push_back(array);
        }
    }
    return max_ver;
}

com::json database::keys() const {
    auto l = (layout const*)_buf.data();
    try {
        return com::json::parse(l->keys());
    } catch (nlohmann::detail::parse_error) {
        log::error(name(), +"failed to read keys", std::string(l->keys()));
        return {};
    }
}

record const* database::records() const {
    auto l = (layout const*)_buf.data();
    return l->records();
}

}    // namespace miu::asp
