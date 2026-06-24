#include <gtest/gtest.h>

#include "gtest_utils.h"

#include <scl/feature/heap/unique.h>
#include <scl/feature/wrapper.h>

#include <concepts>
#include <string>
#include <utility>

using namespace ::scl;

// ---------------------------------------------------------------------------
// Type properties
// ---------------------------------------------------------------------------

TEST(UniqueExecutor, NotCopyable)
{
    STATIC_EXPECT_FALSE(::std::copyable<feature::heap::unique<int>>);
}

TEST(UniqueExecutor, Movable) { STATIC_EXPECT_TRUE(::std::movable<feature::heap::unique<int>>); }

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

TEST(UniqueExecutor, Construction)
{
    feature::heap::unique<int> e{42};
    EXPECT_EQ(feature::heap::unique<int>::value(e), 42);
}

TEST(UniqueExecutor, ConstructionString)
{
    feature::heap::unique<::std::string> e{"hello"};
    EXPECT_EQ(feature::heap::unique<::std::string>::value(e), "hello");
}

// ---------------------------------------------------------------------------
// Move semantics
// ---------------------------------------------------------------------------

TEST(UniqueExecutor, MoveTransfersOwnership)
{
    feature::heap::unique<int> src{99};
    feature::heap::unique<int> dst{::std::move(src)};
    EXPECT_EQ(feature::heap::unique<int>::value(dst), 99);
}

TEST(UniqueExecutor, MoveAssignTransfersOwnership)
{
    feature::heap::unique<int> src{77};
    feature::heap::unique<int> dst{0};
    dst = ::std::move(src);
    EXPECT_EQ(feature::heap::unique<int>::value(dst), 77);
}

// ---------------------------------------------------------------------------
// Execute
// ---------------------------------------------------------------------------

TEST(UniqueExecutor, Execute)
{
    feature::heap::unique<int> e{7};
    auto result = feature::heap::unique<int>::execute(e, [](int v) { return v * 2; });
    EXPECT_EQ(result, 14);
}

TEST(UniqueExecutor, ExecuteWithArgs)
{
    feature::heap::unique<int> e{10};
    auto result = feature::heap::unique<int>::execute(e, [](int v, int factor) {
        return v * factor;
    }, 3);
    EXPECT_EQ(result, 30);
}

// ---------------------------------------------------------------------------
// Via wrapper
// ---------------------------------------------------------------------------

TEST(UniqueExecutor, WrapperUsage)
{
    wrapper<int, feature::heap::unique> w{42};
    EXPECT_EQ(feature::heap::unique<int>::value(feature::detail::executor_access::get(w)), 42);
}
