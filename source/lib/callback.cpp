#include "asp/callback.hpp"

namespace miu::asp {

bool callback::reset(getter const& get) {
    if (!_get) {
        _get = get;
        return true;
    }
    return false;
}

com::variant callback::get() const {
    if (!_get) {
        return com::variant {};
    }
    return _get();
}

bool callback::reset(setter const& set) {
    if (!_set) {
        _set = set;
        return true;
    }
    return false;
}

bool callback::set(com::variant v) {
    if (!_set) {
        return false;
    }
    _set(v);
    return true;
}

}    // namespace miu::asp
