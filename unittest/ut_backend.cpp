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
        frontend.insert_getter(callback::make_getter(&mock::get, &m), +"item1");
        frontend.insert_getter(callback::make_getter(&mock::get, &m), +"item2");

        EXPECT_CALL(m, get()).WillOnce(Return(1)).WillOnce(Return(2));
        miu::asp::backend backend { "ut_backend", &frontend };
    }

    miu::asp::database db { "ut_backend" };

    // init keys
    nlohmann::json keys;
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
        frontend.insert_getter(callback::make_getter(&mock::get, &m), +"item1");
        frontend.insert_getter(callback::make_getter(&mock::get, &m), +"item2");

        EXPECT_CALL(m, get()).WillOnce(Return(0)).WillOnce(Return(0)).WillOnce(Return(1)).WillOnce(
            Return(2));
        miu::asp::backend backend { "ut_backend", &frontend };
        backend.dump();
    }

    miu::asp::database db { "ut_backend" };
    EXPECT_EQ(variant(1), db[0].variant());
    EXPECT_EQ(variant(2), db[1].variant());
}

#if 0
TEST_F(ut_backend, reload) {
    nlohmann::json keys;
    keys["item1"] = 0;
    keys["item2"] = 1;

    {
        miu::asp::database db { "ut_backend" };
        db.reset(keys);
        db[0].set(999);
        db[1].set(888);
    }

    struct mock {
        MOCK_METHOD(void, set, (int32_t));
    } m;
    miu::asp::frontend frontend;
    frontend.insert_setter(callback::make_setter(&mock::set, &m), +"item1");
    frontend.insert_setter(callback::make_setter(&mock::set, &m), +"item2");

    EXPECT_CALL(m, set(999));
    EXPECT_CALL(m, set(888));
    miu::asp::backend { "ut_backend", &frontend };
}
#endif
