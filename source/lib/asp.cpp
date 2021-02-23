
#include "asp/asp.hpp"

#include "backend.hpp"
#include "frontend.hpp"

namespace miu::asp {

static frontend g_frontend;
static backend* g_backend { nullptr };

void reset(std::string_view name) {
    delete g_backend;
    g_backend = nullptr;

    if (!name.empty()) {
        g_backend = new backend { name, &g_frontend };
    } else {
        g_frontend.clear();
    }
}

void dump() {
    if (g_backend) {
        g_backend->dump();
    }
}

void insert_getter(std::vector<com::variant> const& keys, callback::getter const& getter) {
    g_frontend.insert_getter(keys, getter);
}

void insert_setter(std::vector<com::variant> const& keys, callback::setter const& setter) {
    g_frontend.insert_setter(keys, setter);
}

}    // namespace miu::asp
