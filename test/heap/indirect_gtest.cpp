#include <gtest/gtest.h>

#include <scl/feature/heap/indirect.h>
#include <scl/feature/wrapper.h>

#include <string>
#include <utility>

using namespace ::scl;
using namespace ::scl::feature::heap;

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

TEST(IndirectExecutor, ConstructInt)
{
    [[maybe_unused]]
    indirect<int> e{42};
}

TEST(IndirectExecutor, ConstructString)
{
    [[maybe_unused]]
    indirect<::std::string> e{"hello"};
}

TEST(IndirectExecutor, DefaultConstruct)
{
    [[maybe_unused]]
    indirect<int> e{};
}

TEST(IndirectExecutor, ValueAfterConstruct)
{
    indirect<int> e{99};
    EXPECT_EQ(indirect<int>::value(e), 99);
}

// ---------------------------------------------------------------------------
// Deep copy
// ---------------------------------------------------------------------------

TEST(IndirectExecutor, CopyConstructorDeepCopies)
{
    indirect<int> src{7};
    indirect<int> dst{src}; // copy ctor (const&)

    EXPECT_EQ(indirect<int>::value(dst), 7);

    // Mutate dst — src must be unaffected
    indirect<int>::value(dst) = 99;
    EXPECT_EQ(indirect<int>::value(src), 7);
    EXPECT_EQ(indirect<int>::value(dst), 99);
}

TEST(IndirectExecutor, CopyAssignDeepCopies)
{
    indirect<int> src{3};
    indirect<int> dst{0};
    dst = src; // copy assign

    EXPECT_EQ(indirect<int>::value(dst), 3);

    indirect<int>::value(dst) = 77;
    EXPECT_EQ(indirect<int>::value(src), 3);
    EXPECT_EQ(indirect<int>::value(dst), 77);
}

// ---------------------------------------------------------------------------
// Move — transfers ownership
// ---------------------------------------------------------------------------

TEST(IndirectExecutor, MoveConstructorTransfersOwnership)
{
    indirect<::std::string> src{"world"};
    indirect<::std::string> dst{::std::move(src)}; // move ctor

    EXPECT_EQ(indirect<::std::string>::value(dst), "world");
}

TEST(IndirectExecutor, MoveAssignTransfersOwnership)
{
    indirect<::std::string> src{"foo"};
    indirect<::std::string> dst{""};
    dst = ::std::move(src); // move assign

    EXPECT_EQ(indirect<::std::string>::value(dst), "foo");
}

// ---------------------------------------------------------------------------
// execute() — invokes callable with held value
// ---------------------------------------------------------------------------

TEST(IndirectExecutor, ExecutePassesValueToCallable)
{
    indirect<int> e{21};
    auto result = indirect<int>::execute(e, [](int v) { return v * 2; });
    EXPECT_EQ(result, 42);
}

TEST(IndirectExecutor, ExecutePassesValueAndExtraArgs)
{
    indirect<int> e{10};
    auto result = indirect<int>::execute(e, [](int v, int extra) { return v + extra; }, 5);
    EXPECT_EQ(result, 15);
}

TEST(IndirectExecutor, ExecuteMutatesViaReference)
{
    indirect<int> e{1};
    indirect<int>::execute(e, [](int & v) { v = 100; });
    EXPECT_EQ(indirect<int>::value(e), 100);
}

// ---------------------------------------------------------------------------
// Use via wrapper<>
// ---------------------------------------------------------------------------

TEST(IndirectWrapper, WrapperConstruct)
{
    [[maybe_unused]]
    wrapper<int, feature::heap::indirect> w{42};
}

TEST(IndirectWrapper, WrapperCopyConstructible)
{
    // wrapper should be copy-constructible since indirect supports deep copy
    static_assert(::std::is_copy_constructible_v<wrapper<int, feature::heap::indirect>>);
    EXPECT_TRUE((::std::is_copy_constructible_v<wrapper<int, feature::heap::indirect>>));
}

TEST(IndirectWrapper, WrapperMoveConstructible)
{
    static_assert(::std::is_move_constructible_v<wrapper<int, feature::heap::indirect>>);
    EXPECT_TRUE((::std::is_move_constructible_v<wrapper<int, feature::heap::indirect>>));
}
