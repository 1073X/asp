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
        layout->vals()[1] = +"abc";

        nlohmann::json keys;
        keys["item0"] = 0;
        keys["item1"] = 1;

        std::ostringstream ss;
        ss << keys.dump();
        std::strncpy(layout->keys(), ss.str().c_str(), 1024);
    }

    miu::asp::database db { "ut_database" };
    EXPECT_EQ(1, db.get("item0").get<int32_t>());
}

