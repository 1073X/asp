#pragma once

#include "database.hpp"
#include "frontend.hpp"

namespace miu::asp {

class backend {
  public:
    backend(std::string_view name, frontend* f)
        : _db(name)
        , _frontend(f) {
        _db.reset(_frontend->keys());
        for (auto i = 0U; i < _frontend->size(); i++) {
            _db[i] = _frontend->at(i).get();
        }
    }

  private:
    database _db;
    frontend* _frontend;
};

}    // namespace miu::asp
