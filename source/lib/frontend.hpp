#pragma once

#include <com/fatal_error.hpp>
#include <nlohmann/json.hpp>
#include <vector>

#include "callback.hpp"

using nlohmann::json;

namespace miu::asp {

class frontend {
  public:
    json const& keys() const;
    uint32_t size() const;
    callback& operator[](uint32_t idx);

    template<typename... ARGS>
    auto insert_getter(callback::getter const& cb, ARGS&&... args) {
        return insert(cb, "get", std::forward<ARGS>(args)...);
    }

    template<typename... ARGS>
    auto insert_setter(callback::setter const& cb, ARGS&&... args) {
        return insert(cb, "set", std::forward<ARGS>(args)...);
    }

  private:
    template<typename T, typename... ARGS>
    bool insert(T const& cb, const char* type, ARGS&&... args) {
        auto idx = fetch_index(_keys, std::forward<ARGS>(args)...);
        if (idx == -1U) {
            log::error(+"asp", type, +"CONFLICT -", std::forward<ARGS>(args)...);
            return false;
        }

        while (_cbs.size() <= idx) {
            log::debug(+"asp", type, _cbs.size(), +"-", std::forward<ARGS>(args)...);
            _cbs.emplace_back();
        }
        if (!_cbs[idx].reset(cb)) {
            log::error(+"asp", type, +"DUPLICATE -", std::forward<ARGS>(args)...);
            return false;
        }
        return true;
    }

    uint32_t fetch_index(json&) const;

    template<typename T, typename... ARGS>
    uint32_t fetch_index(json& root, T const& t, ARGS&&... args) const {
        auto name = com::to_string(t);
        return fetch_index(root[name], std::forward<ARGS>(args)...);
    }

    template<typename... ARGS>
    uint32_t fetch_index(json& root, int32_t idx, ARGS&&... args) const {
        while ((uint32_t)idx >= root.size()) {
            root.push_back(json());
        }
        return fetch_index(root[idx], std::forward<ARGS>(args)...);
    }

  private:
    std::vector<callback> _cbs;
    json _keys;
};

}    // namespace miu::asp
