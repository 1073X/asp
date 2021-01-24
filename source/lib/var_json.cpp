
#include <com/variant.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace miu::com {

template<>
std::optional<json> variant::get<json>() const {
    switch (id()) {
    case type_id<int32_t>::value:
        return json(get<int32_t>().value());
    default:
        return std::nullopt;
    }
}

}    // namespace miu::com
