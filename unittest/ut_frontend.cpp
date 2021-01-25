#include <gmock/gmock.h>

#include "source/lib/frontend.hpp"

using miu::com::variant;
using testing::Return;

struct ut_frontend : public testing::Test {
    void SetUp() override {
        using miu::log::severity;
        miu::log::log::instance()->reset(severity::DEBUG, 1024);
    }

    void TearDown() override { miu::log::log::instance()->dump(); }

    MOCK_METHOD(variant, callback, ());
    miu::asp::frontend frontend;
};

TEST_F(ut_frontend, insert) {
    auto callback = std::bind(&ut_frontend::callback, this);
    frontend.insert(callback, "item1");
    frontend.insert(callback, "item2");
    frontend.insert(callback, "group", "item1");
    frontend.insert(callback, "array", 0, "item1");
    frontend.insert(callback, "array", 3);
    frontend.insert(callback, "array", 2, "item1");
    frontend.insert(callback, "array", 0, "item2");

    EXPECT_EQ(7U, frontend.size());
    EXPECT_CALL(*this, callback())
        .WillOnce(Return(variant(1)))
        .WillOnce(Return(variant(+"abc")))
        .WillOnce(Return(variant(1.2)))
        .WillOnce(Return(variant(2)));
    EXPECT_EQ(variant(1), frontend[0]());
    EXPECT_EQ(variant(+"abc"), frontend[1]());
    EXPECT_EQ(variant(1.2), frontend[2]());
    EXPECT_EQ(variant(2), frontend[3]());

    nlohmann::json keys;
    keys["item1"]             = 0;
    keys["item2"]             = 1;
    keys["group"]["item1"]    = 2;
    keys["array"][0]["item1"] = 3;
    keys["array"][0]["item2"] = 6;
    keys["array"][1]          = nlohmann::json();
    keys["array"][2]["item1"] = 5;
    keys["array"][3]          = 4;
    EXPECT_EQ(keys, frontend.keys());
}

TEST_F(ut_frontend, duplicated) {
    auto callback = std::bind(&ut_frontend::callback, this);
    frontend.insert(callback, "item1");
    EXPECT_ANY_THROW(frontend.insert(callback, "item1"));

    frontend.insert(callback, "group", "item1");
    EXPECT_ANY_THROW(frontend.insert(callback, "group", "item1"));

    frontend.insert(callback, "array", 0);
    EXPECT_ANY_THROW(frontend.insert(callback, "array", 0));
}

