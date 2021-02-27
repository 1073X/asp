#pragma once

#include <com/strcat.hpp>
#include <log/log.hpp>
#include <nlohmann/json.hpp>

#include "database.hpp"

namespace miu::asp {

class loader {
  public:
    loader(database& db)
        : _db(db) {}

    void load(nlohmann::json const& vals) { load_object(_db.name(), _db.keys(), vals); }

  private:
    template<typename T>
    void set(com::strcat path, nlohmann::json const key, nlohmann::json const& val) {
        if (key.is_number_unsigned()) {
            static std::list<std::string> strings;

            auto old_var = _db[key].variant();
            auto old_val = old_var.get<T>();

            auto new_val = val.get<T>();
            auto new_var = com::variant { new_val };

            if (!old_val.has_value() || new_val != old_val.value()) {
                strings.push_back(com::to_string(old_var));
                auto old_str = strings.back().c_str();

                strings.push_back(com::to_string(new_var));
                auto new_str = strings.back().c_str();

                log::info(+"LOAD {", path, +"}", old_str, +"->", new_str);
                _db[key].set(new_var);
            }
        }
    }

    void load(com::strcat path, nlohmann::json const key, nlohmann::json const& val) {
        using nlohmann::detail::value_t;

        switch (val.type()) {
        case value_t::boolean:
            set<bool>(path, key, val);
            break;
        case value_t::string:
            set<std::string>(path, key, val);
            break;
        case value_t::number_integer:
            set<int64_t>(path, key, val);
            break;
        case value_t::number_unsigned:
            set<uint64_t>(path, key, val);
            break;
        case value_t::number_float:
            set<double>(path, key, val);
            break;
        case value_t::object:
            load_object(path, key, val);
            break;
        case value_t::array:
            load_array(path, key, val);
            break;
        default:
            break;
        }
    }

    void load_object(com::strcat path, nlohmann::json const& keys, nlohmann::json const& vals) {
        for (auto const& [k, v] : vals.items()) {
            if (keys.contains(k)) {
                load(path + k, keys[k], v);
            } else {
                log::warn(+"SKIP {", path + k, +"}");
            }
        }
    }

    void load_array(com::strcat path, nlohmann::json const keys, nlohmann::json const& vals) {
        for (auto i = 0U; i < vals.size(); i++) {
            if (i < keys.size()) {
                load(path + i, keys[i], vals[i]);
            } else {
                log::warn(+"SKIP {", path + i, +"}");
            }
        }
    }

  private:
    database& _db;
};

}    // namespace miu::asp
