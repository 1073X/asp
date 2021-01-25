#include <gmock/gmock.h>

#include "source/lib/frontend.hpp"

using miu::com::variant;
using testing::Return;

struct ut_frontend : public testing::Test {
    MOCK_METHOD(variant, callback, ());

    miu::asp::frontend frontend;
};

TEST_F(ut_frontend, insert) {
    auto callback = std::bind(&ut_frontend::callback, this);
    frontend.insert(callback, "item1");
    frontend.insert(callback, "item2");
    frontend.insert(callback, "group", "item1");

    EXPECT_EQ(3U, frontend.size());
    EXPECT_CALL(*this, callback())
        .WillOnce(Return(variant(1)))
        .WillOnce(Return(variant(+"abc")))
        .WillOnce(Return(variant(1.2)));
    EXPECT_EQ(variant(1), frontend[0]());
    EXPECT_EQ(variant(+"abc"), frontend[1]());
    EXPECT_EQ(variant(1.2), frontend[2]());

    nlohmann::json keys;
    keys["item1"]          = 0;
    keys["item2"]          = 1;
    keys["group"]["item1"] = 2;
    EXPECT_EQ(keys, frontend.keys());
}
