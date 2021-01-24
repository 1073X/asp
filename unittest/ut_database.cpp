#include <gtest/gtest.h>

#include <shm/tempfs.hpp>
#include <vector>

#include "source/lib/database.hpp"
#include "source/lib/layout.hpp"

using miu::com::type_id;

struct ut_database : public testing::Test {
    void SetUp() override {
        using miu::log::severity;
        miu::log::log::instance()->reset(severity::DEBUG, 1024);
    }
    void TearDown() override {
        miu::shm::tempfs::remove("ut_database.asp");
        miu::log::log::instance()->dump();
    }
};

TEST_F(ut_database, open) {
    {
        miu::shm::buffer buf { "ut_database.asp", 4096 };
        auto layout = (miu::asp::layout*)buf.data();

        layout->size         = 2;
        layout->records()[0] = 1;
        layout->records()[1] = std::string("abc");

        nlohmann::json keys;
        keys["item0"] = 0;
        keys["item1"] = 1;

        std::ostringstream ss;
        ss << keys.dump();
        std::strncpy(layout->keys(), ss.str().c_str(), 1024);
    }

    miu::asp::database db { "ut_database" };
    EXPECT_EQ("ut_database.asp", db.name());
    EXPECT_EQ(2U, db.size());

    EXPECT_EQ(1, db[0].get<int32_t>());
    EXPECT_EQ("abc", db[1].get<std::string>());
    EXPECT_NO_THROW(db[2]);
}

TEST_F(ut_database, reset) {
    nlohmann::json keys;
    keys["item0"]             = 0;
    keys["item1"]             = 1;
    keys["group"]["item1"]    = 2;
    keys["array"][0]["item1"] = 3;
    keys["array"][1]["item1"] = 0.1;

    miu::asp::database { "ut_database" }.reset(keys);

    miu::shm::buffer buf { "ut_database.asp" };
    auto layout = (miu::asp::layout*)buf.data();
    EXPECT_EQ(4U, layout->size);
    EXPECT_EQ(type_id<void>::value, layout->records()[0].id());
    EXPECT_EQ(type_id<void>::value, layout->records()[1].id());
    EXPECT_EQ(type_id<void>::value, layout->records()[2].id());
    EXPECT_EQ(type_id<void>::value, layout->records()[3].id());

    std::ostringstream ss;
    ss << keys;
    EXPECT_EQ(ss.str(), layout->keys());
}

TEST_F(ut_database, set_by_idx) {
    nlohmann::json keys;
    keys["item0"] = 0;
    keys["item1"] = 1;

    miu::asp::database db { "ut_database" };
    db.reset(keys);

    db[1] = 2;
    EXPECT_EQ(2, db[1].get<int32_t>());
}

TEST_F(ut_database, capture) {
    nlohmann::json keys;
    keys["item0"]             = 0;
    keys["item1"]             = 1;
    keys["group"]["item1"]    = 2;
    keys["array"][0]["item1"] = 3;
    keys["array"][1][0]       = 4;

    miu::asp::database db { "ut_database" };
    db.reset(keys);
    db[0] = 0;
    db[1] = 1;
    db[2] = 2;
    db[3] = 3;
    db[4] = 4;

    auto data = db.capture(0);
    EXPECT_EQ(0, data["item0"]);
    EXPECT_EQ(1, data["item1"]);
    EXPECT_EQ(2, data["group"]["item1"]);
    EXPECT_EQ(3, data["array"][0]["item1"]);
    EXPECT_EQ(4, data["array"][1][0]);
    EXPECT_EQ(1, data["_VER_"]);
}

TEST_F(ut_database, capture_delta) {
    nlohmann::json keys;
    keys["item0"] = 0;
    keys["item1"] = 1;

    miu::asp::database db { "ut_database" };
    db.reset(keys);
    db[0] = 0;
    db[1] = 1;
    db[1] = 2;

    auto data = db.capture(1);
    EXPECT_FALSE(data.contains("item0"));
    EXPECT_EQ(2, data["item1"]);
    EXPECT_EQ(2, data["_VER_"]);
}
