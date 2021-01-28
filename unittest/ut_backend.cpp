#include <gmock/gmock.h>

#include "source/lib/backend.hpp"

using miu::asp::callback;
using testing::Return;

TEST(ut_backend, init) {
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
    EXPECT_EQ(1, db[0].get<int32_t>());
    EXPECT_EQ(2, db[1].get<int32_t>());
}
