
#include <com/variant.hpp>
#include <nlohmann/json.hpp>
#include <time/stamp.hpp>

using json = nlohmann::json;

namespace miu::com {

template<>
std::optional<json> variant::get<json>() const {
    switch (id()) {
    case type_id<int8_t>::value:
    case type_id<int16_t>::value:
    case type_id<int32_t>::value:
    case type_id<int64_t>::value:
    case type_id<time::days>::value:
    case type_id<time::delta>::value:
        return json(get<int64_t>().value());
    case type_id<uint8_t>::value:
    case type_id<uint16_t>::value:
    case type_id<uint32_t>::value:
    case type_id<uint64_t>::value:
    case type_id<time::date>::value:
    case type_id<time::daytime>::value:
    case type_id<time::stamp>::value:
        return json(get<uint64_t>().value());
    case type_id<float>::value:
    case type_id<double>::value:
        return json(get<double>().value());
    case type_id<const char*>::value:
    case type_id<std::string>::value:
        return json(get<std::string>().value());
    default:
        return std::nullopt;
    }
}

}    // namespace miu::com
