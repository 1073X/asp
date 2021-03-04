#include <gtest/gtest.h>

#include <time/stamp.hpp>

#include "source/lib/database.hpp"

using miu::com::json;

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
                                 miu::time::delta,
                                 miu::time::days,
                                 miu::time::date,
                                 miu::time::daytime,
                                 miu::time::stamp>;

TYPED_TEST_SUITE(ut_var_json_num, num_types);

TYPED_TEST(ut_var_json_num, get) {
    auto val = TypeParam { 123 };
    auto var = miu::com::variant { val };
    EXPECT_EQ(json(123), var.get<json>());
}

//////////////////////////////////////////////////////
TEST(ut_var_json, void) {
    EXPECT_EQ(json("N/A"), miu::com::variant().get<json>());
}

TEST(ut_var_json, string) {
    EXPECT_EQ(json("abc"), miu::com::variant(std::string("abc")).get<json>());
}

TEST(ut_var_json, c_str) {
    EXPECT_EQ(json("LOC_STR"), miu::com::variant(+"abc").get<json>());
}

TEST(ut_var_json, bool) {
    EXPECT_EQ(json(true), miu::com::variant(true).get<json>());
}
