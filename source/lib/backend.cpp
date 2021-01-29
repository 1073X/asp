
#include "backend.hpp"

namespace miu::asp {

backend::backend(std::string_view name, frontend* f)
    : _database(name)
    , _frontend(f) {
    _frontend->reset(_database);

    _database.reset(_frontend->keys());
    dump();
}

void backend::dump() {
    for (auto i = 0U; i < _frontend->size(); i++) {
        auto val = _frontend->at(i).get();
        _database[i].set(val);
    }
}

}    // namespace miu::asp
