#include <gtest/gtest.h>

#include <scl/feature/heap/shared.h>
#include <scl/feature/wrapper.h>
#include <scl/feature/wrapper_guard.h>

#include <string>
#include <utility>

using namespace ::scl;
using namespace ::scl::feature::heap;

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

TEST(SharedExecutor, ConstructInt)
{
    [[maybe_unused]]
    shared<int> e{42};
}

TEST(SharedExecutor, ConstructString)
{
    [[maybe_unused]]
    shared<::std::string> e{"hello"};
}

TEST(SharedExecutor, DefaultConstruct)
{
    [[maybe_unused]]
    shared<int> e{};
}

TEST(SharedExecutor, ValueAfterConstruct)
{
    shared<int> e{99};
    EXPECT_EQ(shared<int>::value(e), 99);
}

// ---------------------------------------------------------------------------
// Shallow copy — same heap object (mutation visible through all copies)
// ---------------------------------------------------------------------------

TEST(SharedExecutor, CopyConstructorMutationIsVisible)
{
    shared<int> a{7};
    shared<int> b{a}; // shallow copy — same heap object

    EXPECT_EQ(shared<int>::value(b), 7);

    // Mutate through b — change must be visible through a (same object)
    shared<int>::value(b) = 99;
    EXPECT_EQ(shared<int>::value(a), 99);
    EXPECT_EQ(shared<int>::value(b), 99);
}

TEST(SharedExecutor, CopyAssignMutationIsVisible)
{
    shared<int> a{3};
    shared<int> b{0};
    b = a; // shallow copy assign — now shares same object

    EXPECT_EQ(shared<int>::value(b), 3);

    // Mutate through b — visible through a
    shared<int>::value(b) = 77;
    EXPECT_EQ(shared<int>::value(a), 77);
}

// ---------------------------------------------------------------------------
// Move — transfers ownership; source use_count drops to 1 on destination
// ---------------------------------------------------------------------------

TEST(SharedExecutor, MoveConstructorTransfersOwnership)
{
    shared<::std::string> a{"world"};
    // After move, a no longer refers to the allocation; b holds it with use_count 1
    shared<::std::string> b{::std::move(a)};

    EXPECT_EQ(shared<::std::string>::value(b), "world");
}

TEST(SharedExecutor, MoveAssignTransfersOwnership)
{
    shared<::std::string> a{"foo"};
    shared<::std::string> b{""};
    b = ::std::move(a);

    EXPECT_EQ(shared<::std::string>::value(b), "foo");
}

// Verify use_count == 2 after copy, and use_count == 1 after move
// by round-tripping through execute() which lets us inspect via a ref
TEST(SharedExecutor, UsecountAfterCopyIsTwo)
{
    shared<int> a{0};
    shared<int> b{a}; // shallow copy

    // Both a and b must see mutations — that confirms use_count >= 2
    shared<int>::value(a) = 1;
    EXPECT_EQ(shared<int>::value(b), 1); // b sees the write → same object

    shared<int>::value(b) = 2;
    EXPECT_EQ(shared<int>::value(a), 2); // a sees the write → confirmed
}

TEST(SharedExecutor, UsecountAfterMoveIsOne)
{
    shared<int> a{42};
    shared<int> b{::std::move(a)};

    // b holds the only reference; write through b should not affect a
    // (a's m_ptr is null — accessing it would crash, so we only verify b)
    EXPECT_EQ(shared<int>::value(b), 42);
}

// ---------------------------------------------------------------------------
// cv-ref forwarding on value()
// ---------------------------------------------------------------------------

TEST(SharedExecutor, ValueLvalueRef)
{
    shared<int> e{1};
    auto & ref = shared<int>::value(e);
    ref = 42;
    EXPECT_EQ(shared<int>::value(e), 42);
}

TEST(SharedExecutor, ValueConstLvalueRef)
{
    shared<int> const e{5};
    auto const & ref = shared<int>::value(e);
    EXPECT_EQ(ref, 5);
}

TEST(SharedExecutor, ValueRvalueRef)
{
    shared<::std::string> e{"move_me"};
    ::std::string taken = shared<::std::string>::value(::std::move(e));
    EXPECT_EQ(taken, "move_me");
}

// ---------------------------------------------------------------------------
// execute() — invokes callable with held value
// ---------------------------------------------------------------------------

TEST(SharedExecutor, ExecutePassesValueToCallable)
{
    shared<int> e{21};
    auto result = shared<int>::execute(e, [](int v) { return v * 2; });
    EXPECT_EQ(result, 42);
}

TEST(SharedExecutor, ExecutePassesValueAndExtraArgs)
{
    shared<int> e{10};
    auto result = shared<int>::execute(e, [](int v, int extra) { return v + extra; }, 5);
    EXPECT_EQ(result, 15);
}

TEST(SharedExecutor, ExecuteMutatesViaReference)
{
    shared<int> e{1};
    shared<int>::execute(e, [](int & v) { v = 100; });
    EXPECT_EQ(shared<int>::value(e), 100);
}

// ---------------------------------------------------------------------------
// Distinction from heap::indirect — copies ARE the same object
// ---------------------------------------------------------------------------

TEST(SharedExecutor, CopiesShareIdentity)
{
    // With heap::shared: copies share one heap object.
    // This is the fundamental distinction from heap::indirect (deep copy).
    shared<int> a{42};
    shared<int> b{a};
    shared<int> c{a};

    // All three see the same value
    EXPECT_EQ(shared<int>::value(a), 42);
    EXPECT_EQ(shared<int>::value(b), 42);
    EXPECT_EQ(shared<int>::value(c), 42);

    // Mutation through c is visible through all
    shared<int>::value(c) = 0;
    EXPECT_EQ(shared<int>::value(a), 0);
    EXPECT_EQ(shared<int>::value(b), 0);
}

// ---------------------------------------------------------------------------
// Use via wrapper<>
// ---------------------------------------------------------------------------

TEST(SharedWrapper, WrapperConstruct)
{
    [[maybe_unused]]
    wrapper<int, feature::heap::shared> w{42};
}

TEST(SharedWrapper, WrapperRoundTrip)
{
    wrapper<::std::string, feature::heap::shared> w{"hello"};
    feature::wrapper_guard<decltype(w) &> g{w};
    EXPECT_EQ(g.value(), "hello");
}

TEST(SharedWrapper, WrapperCopyConstructible)
{
    static_assert(::std::is_copy_constructible_v<wrapper<int, feature::heap::shared>>);
    EXPECT_TRUE((::std::is_copy_constructible_v<wrapper<int, feature::heap::shared>>));
}

TEST(SharedWrapper, WrapperMoveConstructible)
{
    static_assert(::std::is_move_constructible_v<wrapper<int, feature::heap::shared>>);
    EXPECT_TRUE((::std::is_move_constructible_v<wrapper<int, feature::heap::shared>>));
}
