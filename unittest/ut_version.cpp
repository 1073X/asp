#include <gtest/gtest.h>

#include <iostream>

namespace miu::asp {
extern std::string_view version();
extern std::string_view build_info();
}    // namespace miu::asp

TEST(ut_version, version) {
    std::cout << miu::asp::version() << std::endl;
    std::cout << miu::asp::build_info() << std::endl;
}
