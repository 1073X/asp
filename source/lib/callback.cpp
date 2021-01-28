#include "callback.hpp"

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
        log::error(+"unsupported getter");
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

void callback::set(com::variant v) {
    if (!_set) {
        log::error(+"unsupported setter");
        return;
    }
    _set(v);
}

}    // namespace miu::asp
