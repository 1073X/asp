
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

}    // namespace miu::asp
