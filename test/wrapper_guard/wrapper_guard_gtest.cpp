#include <counting_executor.h>
#include <gtest_utils.h>

#include <scl/feature/wrapper.h>
#include <scl/feature/wrapper_guard.h>

#include <string_view>

using namespace ::scl;

// ---------------------------------------------------------------------------
// Value specialisation
// ---------------------------------------------------------------------------

TEST(WrapperGuardValue, NotCopyable)
{
    STATIC_EXPECT_FALSE(::std::is_copy_constructible_v<wrapper_guard<int &>>);
}

TEST(WrapperGuardValue, NotMovable)
{
    STATIC_EXPECT_FALSE(::std::is_move_constructible_v<wrapper_guard<int &>>);
}

TEST(WrapperGuardValue, HoldsIntRef)
{
    constexpr auto result = [] {
        int x = 42;
        wrapper_guard<int &> g{x};
        return g.value() == 42;
    }();
    STATIC_EXPECT_TRUE(result);
}

TEST(WrapperGuardValue, HoldsConstRef)
{
    constexpr auto result = [] {
        int const x = 7;
        wrapper_guard<int const &> g{x};
        return g.value() == 7;
    }();
    STATIC_EXPECT_TRUE(result);
}

TEST(WrapperGuardValue, HoldsStringViewRef)
{
    constexpr auto result = [] {
        std::string_view s = "hello";
        wrapper_guard<std::string_view &> g{s};
        return g.value() == "hello";
    }();
    STATIC_EXPECT_TRUE(result);
}

// ---------------------------------------------------------------------------
// Wrapper specialisation — plain executor (no guard/unguard)
// ---------------------------------------------------------------------------

TEST(WrapperGuardWrapper, NotCopyable)
{
    using W = wrapper<int, feature::inplace::plain>;
    STATIC_EXPECT_FALSE(::std::is_copy_constructible_v<wrapper_guard<W &>>);
}

TEST(WrapperGuardWrapper, NotMovable)
{
    using W = wrapper<int, feature::inplace::plain>;
    STATIC_EXPECT_FALSE(::std::is_move_constructible_v<wrapper_guard<W &>>);
}

TEST(WrapperGuardWrapper, PlainExecutorValue)
{
    constexpr auto result = [] {
        wrapper<int, feature::inplace::plain> w{42};
        wrapper_guard<decltype(w) &> g{w};
        return g.value() == 42;
    }();
    STATIC_EXPECT_TRUE(result);
}

TEST(WrapperGuardWrapper, PlainExecutorConstValue)
{
    constexpr auto result = [] {
        wrapper<int, feature::inplace::plain> const w{99};
        wrapper_guard<const decltype(w) &> g{w};
        return g.value() == 99;
    }();
    STATIC_EXPECT_TRUE(result);
}

// ---------------------------------------------------------------------------
// Wrapper specialisation — counting executor (guard/unguard)
// ---------------------------------------------------------------------------

TEST(WrapperGuardWrapper, GuardCalledOnConstruction)
{
    guard_counters counters;
    wrapper<int, counting_executor> w{42, counters};
    {
        wrapper_guard<decltype(w) &> g{w};
        EXPECT_EQ(counters.guard_count, 1);
        EXPECT_EQ(counters.unguard_count, 0);
    }
    EXPECT_EQ(counters.guard_count, 1);
    EXPECT_EQ(counters.unguard_count, 1);
}

TEST(WrapperGuardWrapper, CountingExecutorValue)
{
    constexpr auto result = [] {
        guard_counters counters;
        wrapper<int, counting_executor> w{123, counters};
        wrapper_guard<decltype(w) &> g{w};
        return g.value() == 123;
    }();
    STATIC_EXPECT_TRUE(result);
}
