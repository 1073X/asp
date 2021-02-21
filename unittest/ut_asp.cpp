#include <gmock/gmock.h>

#include <shm/tempfs.hpp>

#include "asp/asp.hpp"
#include "source/lib/database.hpp"

using testing::Return;

struct ut_asp : public testing::Test {
    MOCK_METHOD(int32_t, get, ());
    MOCK_METHOD(void, set, (int32_t));
};

TEST_F(ut_asp, asp) {
    {
        miu::asp::database db { "ut_asp", 4096 };

        nlohmann::json keys;
        keys["item1"] = 0;

        db.reset(keys);
        db[0].set(99);
    }

    miu::asp::read({ +"item1" }, &ut_asp::get, this);
    miu::asp::load({ +"item1" }, &ut_asp::set, this);

    EXPECT_CALL(*this, get())
        .WillOnce(Return(1))     // reset
        .WillOnce(Return(2));    // dump
    EXPECT_CALL(*this, set(99));
    miu::asp::reset("ut_asp");
    miu::asp::dump();

    miu::asp::reset();
    miu::shm::tempfs::remove("ut_asp.asp");
}
