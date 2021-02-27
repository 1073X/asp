#include <gtest/gtest.h>

#include <log/log.hpp>
#include <shm/tempfs.hpp>

#include "source/lib/database.hpp"
#include "source/lib/loader.hpp"

using miu::asp::database;
using miu::asp::loader;
using miu::com::variant;

struct ut_loader : public testing::Test {
    void SetUp() override {
        using miu::log::severity;
        // miu::log::reset(severity::DEBUG, 1024);
    }
    void TearDown() override {
        miu::shm::tempfs::remove("ut_loader.asp");
        // miu::log::dump();
    }
};

TEST_F(ut_loader, load) {
    nlohmann::json keys;
    keys["integer"]  = 0;
    keys["unsigned"] = 1;
    keys["double"]   = 2;
    keys["boolean"]  = 3;
    keys["string"]   = 4;

    nlohmann::json vals;
    vals["integer"]  = 123;
    vals["unsigned"] = 123U;
    vals["double"]   = 1.2;
    vals["boolean"]  = true;
    vals["string"]   = "string";

    database db { "ut_loader", 4096 };
    db.reset(keys);
    loader(db).load(vals);

    EXPECT_EQ(variant((int64_t)123), db[0].variant());
    EXPECT_EQ(variant((uint64_t)123), db[1].variant());
    EXPECT_EQ(variant(1.2), db[2].variant());
    EXPECT_EQ(variant(true), db[3].variant());
    EXPECT_EQ(variant(std::string("string")), db[4].variant());
}

TEST_F(ut_loader, mismatch) {
    nlohmann::json keys;
    keys["item0"]        = 0;
    keys["item2"]["obj"] = 1;
    keys["obj"]          = 2;

    nlohmann::json vals;
    vals["item1"]        = 123;    // key missing
    vals["item2"]        = 321;    // inconsistent
    vals["obj"]["item1"] = 321;    // inconsistent

    database db { "ut_loader", 4096 };
    db.reset(keys);
    EXPECT_NO_THROW(loader(db).load(vals));
}

TEST_F(ut_loader, object) {
    nlohmann::json keys;
    keys["obj"]["item0"] = 0;

    nlohmann::json vals;
    vals["obj"]["item0"] = 123;

    database db { "ut_loader", 4096 };
    db.reset(keys);
    loader(db).load(vals);

    EXPECT_EQ(variant((int64_t)123), db[0].variant());
}

TEST_F(ut_loader, array) {
    nlohmann::json keys;
    keys["array"].push_back(0);

    nlohmann::json vals;
    vals["array"].push_back(123);
    vals["array"].push_back(321);    // mismatch

    database db { "ut_loader", 4096 };
    db.reset(keys);
    loader(db).load(vals);

    EXPECT_EQ(variant((int64_t)123), db[0].variant());
}

TEST_F(ut_loader, change_value) {
    nlohmann::json keys;
    keys["item0"] = 0;
    keys["item1"] = 1;

    nlohmann::json vals;
    vals["item0"] = "xyz";    // change value
    vals["item1"] = 123;      // change value and type

    database db { "ut_loader", 4096 };
    db.reset(keys);
    db[0].set(+"abc");
    db[1].set(+"str");

    loader(db).load(vals);

    EXPECT_EQ(variant(std::string { "xyz" }), db[0].variant());
    EXPECT_EQ(variant((int64_t)123), db[1].variant());
}

