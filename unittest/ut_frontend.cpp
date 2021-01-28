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

    MOCK_METHOD(variant, get, ());
    MOCK_METHOD(void, set, (variant));
    miu::asp::frontend frontend;
};

TEST_F(ut_frontend, empty) {
    EXPECT_EQ(0U, frontend.size());
    EXPECT_EQ(variant(), frontend.at(0).get());
    frontend.at(1).set(2);
}

TEST_F(ut_frontend, get) {
    auto get = std::bind(&ut_frontend::get, this);
    frontend.insert_getter(get, +"item1");
    frontend.insert_getter(get, +"item2");
    frontend.insert_getter(get, +"group", +"item1");
    frontend.insert_getter(get, +"array", 0, +"item1");
    frontend.insert_getter(get, +"array", 3);
    frontend.insert_getter(get, +"array", 2, +"item1");
    frontend.insert_getter(get, +"array", 0, +"item2");

    EXPECT_EQ(7U, frontend.size());
    EXPECT_CALL(*this, get())
        .WillOnce(Return(variant(1)))
        .WillOnce(Return(variant(+"abc")))
        .WillOnce(Return(variant(1.2)))
        .WillOnce(Return(variant(2)));
    EXPECT_EQ(variant(1), frontend.at(0).get());
    EXPECT_EQ(variant(+"abc"), frontend.at(1).get());
    EXPECT_EQ(variant(1.2), frontend.at(2).get());
    EXPECT_EQ(variant(2), frontend.at(3).get());

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

TEST_F(ut_frontend, set) {
    auto set = std::bind(&ut_frontend::set, this, std::placeholders::_1);
    frontend.insert_setter(set, +"item1");
    frontend.insert_setter(set, +"item2");
    frontend.insert_setter(set, +"group", +"item1");
    frontend.insert_setter(set, +"array", 0, +"item1");
    frontend.insert_setter(set, +"array", 3);
    frontend.insert_setter(set, +"array", 2, +"item1");
    frontend.insert_setter(set, +"array", 0, +"item2");

    EXPECT_EQ(7U, frontend.size());
    EXPECT_CALL(*this, set(variant(1)));
    frontend.at(0).set(1);

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

TEST_F(ut_frontend, duplicate) {
    auto get = std::bind(&ut_frontend::get, this);
    frontend.insert_getter(get, +"item1");
    EXPECT_FALSE(frontend.insert_getter(get, +"item1"));

    frontend.insert_getter(get, +"group", +"item1");
    EXPECT_FALSE(frontend.insert_getter(get, +"group", +"item1"));

    frontend.insert_getter(get, +"array", 0);
    EXPECT_FALSE(frontend.insert_getter(get, +"array", 0));
}

TEST_F(ut_frontend, conflict) {
    auto set = std::bind(&ut_frontend::set, this, std::placeholders::_1);
    frontend.insert_setter(set, +"group", +"item1");
    frontend.insert_setter(set, +"array", 0);

    auto get = std::bind(&ut_frontend::get, this);
    EXPECT_FALSE(frontend.insert_getter(get, +"group"));
    EXPECT_FALSE(frontend.insert_getter(get, +"array"));
}
