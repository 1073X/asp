#pragma once

#include <com/variant.hpp>
#include <functional>
#include <nlohmann/json.hpp>
#include <vector>

namespace miu::asp {

class frontend {
  public:
    using callback_type = std::function<com::variant()>;

  public:
    auto size() const { return _callbacks.size(); }
    auto keys() const { return _keys; }

    auto operator[](uint32_t idx) const { return _callbacks[idx]; }

    template<typename T, typename... ARGS>
    auto insert(callback_type const& callback, T const& t, ARGS&&... args) {
        make_index(_keys, t, std::forward<ARGS>(args)...);
        _callbacks.push_back(callback);
    }

  private:
    void make_index(nlohmann::json& root) const { root = _callbacks.size(); }

    template<typename T, typename... ARGS>
    void make_index(nlohmann::json& root, T const& t, ARGS&&... args) const {
        auto name = com::to_string(t);
        make_index(root[name], std::forward<ARGS>(args)...);
    }

  private:
    std::vector<callback_type> _callbacks;
    nlohmann::json _keys;
};

}    // namespace miu::asp
