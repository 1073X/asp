#pragma once

#include "database.hpp"
#include "frontend.hpp"

namespace miu::asp {

class backend {
  public:
    backend(std::string_view name, frontend*);

    void dump();

  private:
    database _database;
    frontend* _frontend;
};

}    // namespace miu::asp
