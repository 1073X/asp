#include <gtest/gtest.h>

#include <iostream>

#include "asp/version.hpp"

TEST(ut_version, version) {
    std::cout << miu::asp::version() << std::endl;
    std::cout << miu::asp::build_info() << std::endl;
}
