#include <gtest/gtest.h>

#include "source/lib/record.hpp"

using miu::com::type_id;

TEST(ut_record, init) {
    miu::asp::record record;
    EXPECT_EQ(0U, record.version());
    EXPECT_EQ(type_id<void>::value, record.id());
}

TEST(ut_record, set) {
    miu::asp::record record;
    record.set(1);
    EXPECT_EQ(1U, record.version());
    EXPECT_EQ(1, record.get<int32_t>());

    record.set(+"abc");
    EXPECT_EQ(2U, record.version());
    EXPECT_EQ("abc", record.get<const char*>());
}

TEST(ut_record, no_change) {
    miu::asp::record record;
    record.set(1);
    record.set(1);
    EXPECT_EQ(1U, record.version());
}
