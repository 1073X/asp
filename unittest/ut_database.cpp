#include <gtest/gtest.h>

#include <log/log.hpp>
#include <shm/tempfs.hpp>
#include <vector>

#include "source/lib/database.hpp"
#include "source/lib/layout.hpp"

using miu::com::variant;

struct ut_database : public testing::Test {
    void SetUp() override {
        using miu::log::severity;
        // miu::log::log::instance()->reset(severity::DEBUG, 1024);
    }
    void TearDown() override {
        miu::shm::tempfs::remove("ut_database.asp");
        // miu::log::log::instance()->dump();
    }
};

TEST_F(ut_database, open_rdwr) {
    {
        miu::shm::buffer buf { "ut_database.asp", 4096 };
        auto layout = (miu::asp::layout*)buf.data();

        layout->size = 2;
        layout->records()[0].set(1);
        layout->records()[1].set(std::string("abc"));

        nlohmann::json keys;
        keys["item0"] = 0;
        keys["item1"] = 1;

        std::ostringstream ss;
        ss << keys.dump();
        std::strncpy(layout->keys(), ss.str().c_str(), 1024);
    }

    miu::asp::database db { "ut_database", 4096 };
    EXPECT_EQ("ut_database.asp", db.name());
    EXPECT_EQ(2U, db.size());

    EXPECT_EQ(variant(1), db[0].variant());
    EXPECT_EQ(variant(std::string("abc")), db[1].variant());
    EXPECT_NO_THROW(db[2]);
}

TEST_F(ut_database, open_read) {
    { miu::asp::database db { "ut_database", 4096 }; }

    miu::asp::database db { "ut_database", miu::shm::mode::READ };
    EXPECT_EQ("ut_database.asp", db.name());
}

TEST_F(ut_database, invalid_keys) {
    {
        miu::shm::buffer buf { "ut_database.asp", 4096 };
        auto layout       = (miu::asp::layout*)buf.data();
        layout->size      = 1;
        layout->keys()[0] = '{';
    }
    miu::asp::database db { "ut_database", 4096 };
    EXPECT_EQ(nlohmann::json(), db.keys());
}

TEST_F(ut_database, reset) {
    nlohmann::json keys;
    keys["item0"]             = 0;
    keys["item1"]             = 1;
    keys["group"]["item1"]    = 2;
    keys["array"][0]["item1"] = 3;
    keys["array"][1]["item1"] = 0.1;

    miu::asp::database { "ut_database", 4096 }.reset(keys);

    miu::shm::buffer buf { "ut_database.asp", miu::shm::mode::READ };
    auto layout = (miu::asp::layout*)buf.data();
    EXPECT_EQ(4U, layout->size);
    EXPECT_EQ(variant(), layout->records()[0].variant());
    EXPECT_EQ(variant(), layout->records()[1].variant());
    EXPECT_EQ(variant(), layout->records()[2].variant());
    EXPECT_EQ(variant(), layout->records()[3].variant());

    std::ostringstream ss;
    ss << keys;
    EXPECT_EQ(ss.str(), layout->keys());
}

TEST_F(ut_database, set_by_idx) {
    nlohmann::json keys;
    keys["item0"] = 0;
    keys["item1"] = 1;

    miu::asp::database db { "ut_database", 4096 };
    db.reset(keys);

    db[1].set(2);
    EXPECT_EQ(variant(2), db[1].variant());
}

TEST_F(ut_database, capture) {
    nlohmann::json keys;
    keys["item0"]             = 0;
    keys["item1"]             = 1;
    keys["group"]["item1"]    = 2;
    keys["array"][0]["item1"] = 3;
    keys["array"][1][0]       = 4;

    miu::asp::database db { "ut_database", 4096 };
    db.reset(keys);
    db[0].set(0);
    db[1].set(1);
    db[2].set(2);
    db[3].set(3);
    db[4].set(4);

    auto data = db.capture(0);
    EXPECT_EQ(0, data["item0"]);
    EXPECT_EQ(1, data["item1"]);
    EXPECT_EQ(2, data["group"]["item1"]);
    EXPECT_EQ(3, data["array"][0]["item1"]);
    EXPECT_EQ(4, data["array"][1][0]);
    EXPECT_EQ(1, data["_VER_"]);
}

TEST_F(ut_database, capture_invalid_type) {
    nlohmann::json keys;
    keys["item0"] = 0;
    keys["item1"] = 1;

    miu::asp::database db { "ut_database", 4096 };
    db.reset(keys);
    db[0].set(+"abc");

    // update record1 version
    db[1].set(1);
    db[1].set(variant());

    auto data = db.capture(0);
    EXPECT_EQ("LOC_STR", data["item0"]);
    EXPECT_EQ("N/A", data["item1"]);
}

TEST_F(ut_database, capture_delta) {
    nlohmann::json keys;
    keys["item0"] = 0;
    keys["item1"] = 1;

    miu::asp::database db { "ut_database", 4096 };
    db.reset(keys);
    db[0].set(0);
    db[1].set(1);
    db[1].set(2);

    auto data = db.capture(1);
    EXPECT_FALSE(data.contains("item0"));
    EXPECT_EQ(2, data["item1"]);
    EXPECT_EQ(2, data["_VER_"]);
}

TEST_F(ut_database, camera) {
    nlohmann::json keys;
    keys["item0"] = 0;
    keys["item1"] = 1;

    miu::asp::database db { "ut_database", 4096 };
    db.reset(keys);
    db[0].set(0);
    db[1].set(miu::time::date { 2021, 2, 26 });

    auto data = db.capture(0, [](auto var) { return miu::com::to_string(var); });
    EXPECT_EQ("[int32_t:0]", data["item0"]);
    EXPECT_EQ("[time::date:20210226]", data["item1"]);
}
