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
    auto size() const { return _cbs.size(); }
    auto keys() const { return _keys; }

    auto operator[](uint32_t idx) const { return _cbs[idx]; }

    template<typename... ARGS>
    auto insert_getter(callback::getter const& cb, ARGS&&... args) {
        auto idx = make_index(_keys, std::forward<ARGS>(args)...);
        while (_cbs.size() <= idx) {
            log::debug(+"asp get", _cbs.size(), +"-", std::forward<ARGS>(args)...);
            _cbs.emplace_back();
        }
        if (!_cbs[idx].reset(cb)) {
            FATAL_ERROR<std::logic_error>(+"asp get DUP -", std::forward<ARGS>(args)...);
        }
    }

    template<typename... ARGS>
    auto insert_setter(callback::setter const& cb, ARGS&&... args) {
        auto idx = make_index(_keys, std::forward<ARGS>(args)...);
        while (_cbs.size() <= idx) {
            log::debug(+"asp set", _cbs.size(), +"-", std::forward<ARGS>(args)...);
            _cbs.emplace_back();
        }
        if (!_cbs[idx].reset(cb)) {
            FATAL_ERROR<std::logic_error>(+"asp set DUP -", std::forward<ARGS>(args)...);
        }
    }

  private:
    uint32_t make_index(json& root) const {
        if (root.is_null()) {
            root = _cbs.size();
        }
        return root.get<uint32_t>();
    }

    template<typename T, typename... ARGS>
    uint32_t make_index(json& root, T const& t, ARGS&&... args) const {
        auto name = com::to_string(t);
        return make_index(root[name], std::forward<ARGS>(args)...);
    }

    template<typename... ARGS>
    uint32_t make_index(json& root, int32_t idx, ARGS&&... args) const {
        while (idx >= root.size()) {
            root.push_back(json());
        }
        return make_index(root[idx], std::forward<ARGS>(args)...);
    }

  private:
    std::vector<callback> _cbs;
    json _keys;
};

}    // namespace miu::asp
