
#include <cfg/cmd_source.hpp>
#include <cfg/settings.hpp>
#include <com/json.hpp>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <log/log.hpp>
#include <meta/info.hpp>
#include <shm/tempfs.hpp>

#include "asp/version.hpp"
#include "source/lib/database.hpp"
#include "source/lib/loader.hpp"

int32_t main(int32_t argc, const char* argv[]) try {
    miu::log::reset(miu::log::severity::DEBUG, 2048);

    miu::cfg::cmd_source source { argc, argv };
    miu::cfg::settings settings { &source };

    if (settings.optional<bool>("version", false)) {
        std::cout << miu::asp::version() << std::endl;

    } else if (settings.optional<bool>("usage", false)) {
        std::cout << miu::asp::version() << std::endl;
        std::cout << miu::asp::build_info() << std::endl;
        std::cout << miu::meta::info() << ": fill asp with json file." << std::endl;
        std::cout << "\nUsage: loadasp <asp name> <path/to/json/file.json>" << std::endl;

    } else {
        auto asp_name  = settings.required<std::string>(0);
        auto json_file = settings.required<std::string>(1);
        if (!miu::shm::tempfs::exists(asp_name, "asp")) {
            FATAL_ERROR<std::invalid_argument>("cann't find asp [", asp_name, "]");
        }
        if (!std::filesystem::exists(json_file)) {
            FATAL_ERROR<std::invalid_argument>("cann't find json [", json_file, "]");
        }

        {
            miu::asp::database db { asp_name, miu::shm::mode::RDWR };
            std::ifstream ss { json_file };
            miu::asp::loader { db }.load(miu::com::json::parse(ss));
        }

        miu::log::dump();
    }

    return 0;
} catch (std::exception const& err) {
    miu::log::dump();
    std::cerr << err.what() << std::endl;
    return -1;
} catch (...) {
    miu::log::dump();
    std::cerr << "unknown error" << std::endl;
    return -1;
}
