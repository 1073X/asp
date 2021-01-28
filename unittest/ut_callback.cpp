#include <gmock/gmock.h>

#include "source/lib/callback.hpp"

using miu::asp::callback;
using miu::com::variant;

TEST(ut_callback, member_getter) {
    struct object {
        int32_t get() const { return 123; }
    } obj;

    auto func = callback::make_getter(&object::get, &obj);
    EXPECT_EQ(variant(123), func());
}

TEST(ut_callback, member_getter_arg) {
    struct object {
        int32_t get(int32_t a, int32_t b) const { return a + b; }
    } obj;

    auto func = callback::make_getter(&object::get, &obj, 1, 2);
    EXPECT_EQ(variant(3), func());
}

TEST(ut_callback, static_getter) {
    struct object {
        static int32_t get() { return 123; }
    };

    auto func = callback::make_getter(&object::get);
    EXPECT_EQ(variant(123), func());
}

TEST(ut_callback, static_getter_arg) {
    struct object {
        static int32_t get(int32_t a, int32_t b) { return a + b; }
    };

    auto func = callback::make_getter(&object::get, 1, 2);
    EXPECT_EQ(variant(3), func());
}

TEST(ut_callback, member_setter) {
    struct object {
        MOCK_METHOD(void, set, (int32_t));
    } obj;

    auto func = callback::make_setter(&object::set, &obj);
    EXPECT_CALL(obj, set(1));
    func(variant(1));
}

struct object {
    object() { instance = this; }
    ~object() {
        if (instance == this) {
            instance = nullptr;
        }
    }
    MOCK_METHOD(void, do_set, (int32_t));
    static void set(int32_t v) { instance->do_set(v); }
    inline static object* instance;
};

TEST(ut_callback, static_setter) {
    object obj;
    auto func = callback::make_setter(&object::set);
    EXPECT_CALL(obj, do_set(1));
    func(variant(1));
}
