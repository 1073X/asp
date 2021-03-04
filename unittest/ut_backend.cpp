#include <gmock/gmock.h>

#include <shm/tempfs.hpp>

#include "source/lib/backend.hpp"

using miu::asp::callback;
using miu::com::variant;
using testing::Return;

struct ut_backend : public testing::Test {
    void TearDown() override { miu::shm::tempfs::remove("ut_backend.asp"); }
};

TEST_F(ut_backend, init) {
    struct mock {
        MOCK_METHOD(int32_t, get, ());    // test non-const getter, too
    } m;

    {
        miu::asp::frontend frontend;
        frontend.insert_getter({ +"item1" }, callback::make_getter(&mock::get, &m));
        frontend.insert_getter({ +"item2" }, callback::make_getter(&mock::get, &m));

        EXPECT_CALL(m, get()).WillOnce(Return(1)).WillOnce(Return(2));
        miu::asp::backend backend { "ut_backend", &frontend };
    }

    miu::asp::database db { "ut_backend", 4096 };

    // init keys
    miu::com::json keys;
    keys["item1"] = 0;
    keys["item2"] = 1;
    EXPECT_EQ(keys, db.keys());

    // init values
    EXPECT_EQ(variant(1), db[0].variant());
    EXPECT_EQ(variant(2), db[1].variant());
}

TEST_F(ut_backend, dump) {
    struct mock {
        MOCK_METHOD(int32_t, get, ());
    } m;

    {
        miu::asp::frontend frontend;
        frontend.insert_getter({ +"item1" }, callback::make_getter(&mock::get, &m));
        frontend.insert_getter({ +"item2" }, callback::make_getter(&mock::get, &m));

        EXPECT_CALL(m, get()).WillOnce(Return(0)).WillOnce(Return(0)).WillOnce(Return(1)).WillOnce(
            Return(2));
        miu::asp::backend backend { "ut_backend", &frontend };
        backend.dump();
    }

    miu::asp::database db { "ut_backend", miu::shm::mode::READ };
    EXPECT_EQ(variant(1), db[0].variant());
    EXPECT_EQ(variant(2), db[1].variant());
}

TEST_F(ut_backend, reload) {
    miu::com::json keys;
    keys["item1"] = 0;
    keys["item2"] = 1;

    {
        miu::asp::database db { "ut_backend", 4096 };
        db.reset(keys);
        db[0].set(999);
        db[1].set(888);
    }

    struct mock {
        MOCK_METHOD(void, set, (int32_t));
    } m;
    miu::asp::frontend frontend;
    frontend.insert_setter({ +"item1" }, callback::make_setter(&mock::set, &m));
    frontend.insert_setter({ +"item2" }, callback::make_setter(&mock::set, &m));

    EXPECT_CALL(m, set(999));
    EXPECT_CALL(m, set(888));
    miu::asp::backend { "ut_backend", &frontend };
}
