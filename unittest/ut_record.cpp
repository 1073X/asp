#include <gtest/gtest.h>

#include "source/lib/record.hpp"

using miu::com::variant;

TEST(ut_record, init) {
    miu::asp::record record;
    EXPECT_EQ(0U, record.version());
    EXPECT_EQ(variant(), record.value());
}

TEST(ut_record, set) {
    miu::asp::record record;
    record = 1;
    EXPECT_EQ(1U, record.version());
    EXPECT_EQ(1, record.value().get<int32_t>());

    record = +"abc";
    EXPECT_EQ(2U, record.version());
    EXPECT_EQ("abc", record.value().get<const char*>());
}

TEST(ut_record, no_change) {
    miu::asp::record record;
    record = 1;
    record = 1;
    EXPECT_EQ(1U, record.version());
}
