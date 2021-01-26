#pragma once

#include <com/fatal_error.hpp>
#include <log/log.hpp>
#include <nlohmann/json.hpp>
#include <vector>

#include "callback.hpp"

using nlohmann::json;

namespace miu::asp {

class frontend {
  public:
    auto size() const { return _callbacks.size(); }
    auto keys() const { return _keys; }

    auto operator[](uint32_t idx) const { return _callbacks[idx]; }

    template<typename T, typename... ARGS>
    auto insert(callback::getter const& get, T const& t, ARGS&&... args) {
        make_index("", _keys, t, std::forward<ARGS>(args)...);
        _callbacks.push_back({ get });
    }

  private:
    void make_index(com::strcat const& path, json& root) const {
        if (root.is_null()) {
            root = _callbacks.size();
            log::debug(+"asp", _callbacks.size(), path);
        } else {
            FATAL_ERROR<std::logic_error>("asp(dup)", path.str());
        }
    }

    template<typename T, typename... ARGS>
    void make_index(com::strcat const& path, json& root, T const& t, ARGS&&... args) const {
        auto name = com::to_string(t);
        make_index({ path, name }, root[name], std::forward<ARGS>(args)...);
    }

    template<typename... ARGS>
    void make_index(com::strcat const& path, json& root, int32_t idx, ARGS&&... args) const {
        while (idx >= root.size()) {
            root.push_back(json());
        }
        make_index({ path, idx }, root[idx], std::forward<ARGS>(args)...);
    }

  private:
    std::vector<callback> _callbacks;
    json _keys;
};

}    // namespace miu::asp
