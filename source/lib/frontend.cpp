
#include "frontend.hpp"

#include "database.hpp"

namespace miu::asp {

json const& frontend::keys() const {
    return _keys;
}

uint32_t frontend::size() const {
    return _cbs.size();
}

callback& frontend::at(uint32_t idx) {
    if (idx < size()) {
        return _cbs[idx];
    }

    static callback dummy;
    return dummy;
}

static uint32_t fetch(std::vector<com::variant> const& keys, json& root, uint32_t max_index) {
    if (keys.empty()) {
        auto index = -1U;
        if (root.is_null()) {
            index = max_index;
            root  = index;
        } else if (root.is_number()) {
            index = root.get<uint32_t>();
        }
        return index;
    }

    auto it = keys.begin();
    switch (it->id()) {
    case com::type_id<const char*>::value:
    case com::type_id<std::string>::value: {
        auto next = std::vector(it + 1, keys.end());
        auto name = it->get<std::string>().value();
        return fetch(next, root[name], max_index);
    }
    case com::type_id<int8_t>::value:
    case com::type_id<int16_t>::value:
    case com::type_id<int32_t>::value:
    case com::type_id<int64_t>::value:
    case com::type_id<uint8_t>::value:
    case com::type_id<uint16_t>::value:
    case com::type_id<uint32_t>::value:
    case com::type_id<uint64_t>::value: {
        auto idx = it->get<uint64_t>().value();
        while (idx >= root.size()) {
            root.push_back(json());
        }
        auto next = std::vector(it + 1, keys.end());
        return fetch(next, root[idx], max_index);
    }
    default:
        return -1U;
    }
}

template<typename T>
static bool do_insert(const char* type,
                      std::vector<com::variant> const& keys,
                      json& root,
                      T const& cb,
                      std::vector<callback>& cbs) {
    auto idx = fetch(keys, root, cbs.size());
    if (idx == -1U) {
        log::error(+"asp", type, +"CONFLICT -", keys);
        return false;
    }

    while (cbs.size() <= idx) {
        log::debug(+"asp insert", type, cbs.size(), +"-", keys);
        cbs.emplace_back();
    }
    if (!cbs[idx].reset(cb)) {
        log::error(+"asp", type, +"DUPLICATE -", keys);
        return false;
    }
    return true;
}

bool frontend::insert_getter(std::vector<com::variant> const& keys, callback::getter const& cb) {
    return do_insert("getter", keys, _keys, cb, _cbs);
}

bool frontend::insert_setter(std::vector<com::variant> const& keys, callback::setter const& cb) {
    return do_insert("setter", keys, _keys, cb, _cbs);
}

template<typename... PATH>
static void do_reset(database& db,
                     json const& src,
                     std::vector<callback>& cbs,
                     json const& tag,
                     com::strcat path) {
    if (src.is_object() && tag.is_object()) {
        for (auto const& [src_key, src_obj] : src.items()) {
            do_reset(db, src_obj, cbs, tag[src_key], path + src_key);
        }
    } else if (src.is_array() && tag.is_array()) {
        auto max_idx = std::min(src.size(), tag.size());
        for (auto i = 0U; i < max_idx; i++) {
            do_reset(db, src[i], cbs, tag[i], path + i);
        }
    } else if (src.is_number() && tag.is_number()) {
        auto idx = tag.get<uint32_t>();
        auto var = db[src].variant();
        log::info(+"asp reset", idx, +"- VEC[", path, +"] =", var);
        cbs[idx].set(var);
    } else {
        log::warn(+"asp ignore conflict", path);
    }
}

void frontend::reset(database& db) {
    do_reset(db, db.keys(), _cbs, _keys, "");
}

}    // namespace miu::asp
