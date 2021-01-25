#include <gtest/gtest.h>

#include "source/lib/database.hpp"

template<typename>
struct ut_var_json_num : public testing::Test {};

using num_types = testing::Types<int8_t,
                                 int16_t,
                                 int32_t,
                                 int64_t,
                                 uint8_t,
                                 uint16_t,
                                 uint32_t,
                                 uint64_t,
                                 float,
                                 double,
                                 miu::com::microseconds,
                                 miu::com::days,
                                 miu::com::date,
                                 miu::com::daytime,
                                 miu::com::datetime>;

TYPED_TEST_SUITE(ut_var_json_num, num_types);

TYPED_TEST(ut_var_json_num, get) {
    auto val = TypeParam { 123 };
    auto var = miu::com::variant { val };
    EXPECT_EQ(json(123), var.get<json>());
}

//////////////////////////////////////////////////////

template<typename>
struct ut_var_json_str : public testing::Test {};

using str_types = testing::Types<const char*, std::string>;

TYPED_TEST_SUITE(ut_var_json_str, str_types);

TYPED_TEST(ut_var_json_str, get) {
    auto val = TypeParam { "abc" };
    auto var = miu::com::variant { val };
    EXPECT_EQ(json("abc"), var.get<json>());
}

