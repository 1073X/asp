
#include <cfg/cmd_source.hpp>
#include <cfg/settings.hpp>
#include <cstdint>
#include <iomanip>
#include <meta/info.hpp>
#include <nlohmann/json.hpp>
#include <shm/tempfs.hpp>

#include "asp/version.hpp"
#include "source/lib/database.hpp"

int32_t main(int32_t argc, const char* argv[]) try {
    miu::cfg::cmd_source source { argc, argv };
    miu::cfg::settings settings { &source };

    if (settings.optional<bool>("version", false)) {
        std::cout << miu::asp::version() << std::endl;

    } else if (settings.optional<bool>("usage", false)) {
        std::cout << miu::asp::version() << std::endl;
        std::cout << miu::asp::build_info() << std::endl;
        std::cout << miu::meta::info() << ": dump asp content as json." << std::endl;
        std::cout << "\nUsage: aspview <asp name> [--details]" << std::endl;
        std::cout << "\t--details: show variant type and string value" << std::endl;

    } else {
        auto asp_name = settings.required<std::string>(0);
        if (miu::shm::tempfs::exists(asp_name, "asp")) {
            miu::asp::database db { asp_name, miu::shm::mode::READ };

            nlohmann::json json;
            if (!settings.optional<bool>("details", false)) {
                json = db.capture(0, miu::asp::default_camera);
            } else {
                json = db.capture(0, [](auto var) { return miu::com::to_string(var); });
            }

            std::cout << std::setw(4) << json << std::endl;
        } else {
            FATAL_ERROR<std::invalid_argument>("cann't find asp [", asp_name, "]");
        }
    }

    return 0;
} catch (std::exception const& err) {
    std::cerr << err.what() << std::endl;
    return -1;
}
