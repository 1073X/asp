#include <gtest/gtest.h>

#include "source/lib/record.hpp"

using miu::com::variant;

TEST(ut_record, init) {
    miu::asp::record record;
    EXPECT_EQ(0U, record.version());
    EXPECT_EQ(variant(), record.variant());
}

TEST(ut_record, set) {
    miu::asp::record record;
    record.set(1);
    EXPECT_EQ(1U, record.version());
    EXPECT_EQ(variant(1), record.variant());

    record.set(+"abc");
    EXPECT_EQ(2U, record.version());
    EXPECT_EQ(variant(+"abc"), record.variant());
}

TEST(ut_record, no_change) {
    miu::asp::record record;
    record.set(1);
    record.set(1);
    EXPECT_EQ(1U, record.version());
}
