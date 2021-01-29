
#include "frontend.hpp"

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

uint32_t frontend::fetch_index(json& root) const {
    if (root.is_null()) {
        root = _cbs.size();
    }

    auto index = -1U;
    if (root.is_number()) {
        index = root.get<uint32_t>();
    }
    return index;
}

struct values {
    auto const& operator[](uint32_t idx) const { return vals[idx]; }
    com::variant const* vals;
    uint32_t size;
};

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
        log::info(+"asp reset", idx, '-', path, '[', var, ']');
        cbs[idx].set(var);
    } else {
        log::warn(+"asp ignore conflict", path);
    }
}

void frontend::reset(database& db) {
    do_reset(db, db.keys(), _cbs, _keys, "");
}

}    // namespace miu::asp
