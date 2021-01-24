#include <gtest/gtest.h>

#include <shm/tempfs.hpp>
#include <vector>

#include "source/lib/database.hpp"
#include "source/lib/layout.hpp"

struct ut_database : public testing::Test {
    void TearDown() override { miu::shm::tempfs::remove("ut_database.asp"); }
};

TEST_F(ut_database, open) {
    {
        miu::shm::buffer buf { "ut_database.asp", 4096 };
        auto layout = (miu::asp::layout*)buf.data();

        layout->size      = 2;
        layout->vals()[0] = 1;
        layout->vals()[1] = std::string("abc");

        nlohmann::json keys;
        keys["item0"] = 0;
        keys["item1"] = 1;

        std::ostringstream ss;
        ss << keys.dump();
        std::strncpy(layout->keys(), ss.str().c_str(), 1024);
    }

    miu::asp::database db { "ut_database" };
    EXPECT_EQ(1, db.get("item0").get<int32_t>());
    EXPECT_EQ("abc", db.get("item1").get<std::string>());
}

TEST_F(ut_database, reset) {
    nlohmann::json keys;
    keys["item0"] = 0;
    keys["item1"] = 1;

    std::vector<miu::com::variant> vals { 1, std::string("abc") };

    {
        miu::asp::database db { "ut_database" };
        db.reset(keys, vals);
    }

    miu::shm::buffer buf { "ut_database.asp" };
    auto layout = (miu::asp::layout*)buf.data();
    EXPECT_EQ(2U, layout->size);
    EXPECT_EQ(1, layout->vals()[0].get<int32_t>());
    EXPECT_EQ("abc", layout->vals()[1].get<std::string>());

    std::ostringstream ss;
    ss << keys;
    EXPECT_EQ(ss.str(), layout->keys());
}

