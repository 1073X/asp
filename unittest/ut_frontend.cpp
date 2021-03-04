#include <gmock/gmock.h>

#include "source/lib/backend.hpp"
#include "source/lib/frontend.hpp"

using miu::com::variant;
using testing::Return;

struct ut_frontend : public testing::Test {
    void SetUp() override {
        using miu::log::severity;
        // miu::log::reset(severity::DEBUG, 1024);
    }

    void TearDown() override {
        // miu::log::dump();
    }

    MOCK_METHOD(variant, get, ());
    MOCK_METHOD(void, set, (variant));
    miu::asp::frontend frontend;
};

TEST_F(ut_frontend, empty) {
    EXPECT_EQ(0U, frontend.size());
    EXPECT_EQ(variant(), frontend.at(0).get());
    frontend.at(1).set(2);
}

TEST_F(ut_frontend, getter) {
    auto get = std::bind(&ut_frontend::get, this);
    frontend.insert_getter({ +"item1" }, get);
    frontend.insert_getter({ +"item2" }, get);
    frontend.insert_getter({ +"group", +"item1" }, get);
    frontend.insert_getter({ +"array", 0, +"item1" }, get);
    frontend.insert_getter({ +"array", 3U }, get);
    frontend.insert_getter({ +"array", 2, +"item1" }, get);
    frontend.insert_getter({ +"array", 0, +"item2" }, get);

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

    miu::com::json keys;
    keys["item1"]             = 0;
    keys["item2"]             = 1;
    keys["group"]["item1"]    = 2;
    keys["array"][0]["item1"] = 3;
    keys["array"][0]["item2"] = 6;
    keys["array"][1]          = miu::com::json();
    keys["array"][2]["item1"] = 5;
    keys["array"][3]          = 4;
    EXPECT_EQ(keys, frontend.keys());
}

TEST_F(ut_frontend, setter) {
    auto set = std::bind(&ut_frontend::set, this, std::placeholders::_1);
    frontend.insert_setter({ +"item1" }, set);
    frontend.insert_setter({ +"item2" }, set);
    frontend.insert_setter({ +"group", +"item1" }, set);
    frontend.insert_setter({ +"array", 0, +"item1" }, set);
    frontend.insert_setter({ +"array", 3 }, set);
    frontend.insert_setter({ +"array", 2, +"item1" }, set);
    frontend.insert_setter({ +"array", 0, +"item2" }, set);

    EXPECT_EQ(7U, frontend.size());
    EXPECT_CALL(*this, set(variant(1)));
    frontend.at(0).set(1);

    miu::com::json keys;
    keys["item1"]             = 0;
    keys["item2"]             = 1;
    keys["group"]["item1"]    = 2;
    keys["array"][0]["item1"] = 3;
    keys["array"][0]["item2"] = 6;
    keys["array"][1]          = miu::com::json();
    keys["array"][2]["item1"] = 5;
    keys["array"][3]          = 4;
    EXPECT_EQ(keys, frontend.keys());
}

TEST_F(ut_frontend, duplicate) {
    auto get = std::bind(&ut_frontend::get, this);
    frontend.insert_getter({ +"item1" }, get);
    EXPECT_FALSE(frontend.insert_getter({ +"item1" }, get));

    frontend.insert_getter({ +"group", +"item1" }, get);
    EXPECT_FALSE(frontend.insert_getter({ +"group", +"item1" }, get));

    frontend.insert_getter({ +"array", 0 }, get);
    EXPECT_FALSE(frontend.insert_getter({ +"array", 0 }, get));
}

TEST_F(ut_frontend, conflict) {
    auto set = std::bind(&ut_frontend::set, this, std::placeholders::_1);
    frontend.insert_setter({ +"group", +"item1" }, set);
    frontend.insert_setter({ +"array", 0 }, set);

    auto get = std::bind(&ut_frontend::get, this);
    EXPECT_FALSE(frontend.insert_getter({ +"group" }, get));
    EXPECT_FALSE(frontend.insert_getter({ +"array" }, get));
}

TEST_F(ut_frontend, reset) {
    miu::com::json keys;
    keys["item1"]             = 0;
    keys["group"]["item1"]    = 1;
    keys["array"][0]["item1"] = 2;

    miu::asp::database db { "ut_frontend", 4096 };
    db.reset(keys);
    db[0].set(1);
    db[1].set(+"xyz");
    db[2].set(1.2);

    auto set = std::bind(&ut_frontend::set, this, std::placeholders::_1);
    frontend.insert_setter({ +"item1" }, set);
    frontend.insert_setter({ +"group", +"item1" }, set);
    frontend.insert_setter({ +"array", 0, +"item1" }, set);

    EXPECT_CALL(*this, set(variant(1)));
    EXPECT_CALL(*this, set(variant(+"xyz")));
    EXPECT_CALL(*this, set(variant(1.2)));
    frontend.reset(db);
}

TEST_F(ut_frontend, reset_conflict) {
    auto set = std::bind(&ut_frontend::set, this, std::placeholders::_1);
    frontend.insert_setter({ +"group" }, set);
    frontend.insert_setter({ +"item1", +"group" }, set);
    frontend.insert_setter({ +"array" }, set);
    frontend.insert_setter({ +"array2", 0 }, set);

    miu::com::json keys;
    keys["group"]["item1"] = 0;
    keys["array"][0]       = 1;
    keys["array2"][1]      = 2;
    keys["item1"]          = 3;

    miu::asp::database db { "ut_frontend", 4096 };
    db.reset(keys);

    EXPECT_NO_THROW(frontend.reset(db));
}
