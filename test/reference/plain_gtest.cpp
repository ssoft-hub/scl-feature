#include <gtest/gtest.h>

#include <scl/feature/reference/plain.h>
#include <scl/feature/wrapper.h>
#include <scl/feature/wrapper_guard.h>

#include <string>
#include <type_traits>

using namespace ::scl;

// ---------------------------------------------------------------------------
// Compile-time constraints
// ---------------------------------------------------------------------------

// rvalue binding must be deleted to prevent dangling references
static_assert(!std::constructible_from<feature::reference::plain<int>, int &&>);
static_assert(std::constructible_from<feature::reference::plain<int>, int &>);
static_assert(std::copy_constructible<feature::reference::plain<int>>);
static_assert(std::move_constructible<feature::reference::plain<int>>);

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

TEST(ReferencePlain, ConstructFromLvalue)
{
    int x = 42;
    auto const r = feature::reference::plain<int>{x};
    EXPECT_EQ(feature::reference::plain<int>::value(r), 42);
}

TEST(ReferencePlain, RvalueBindingDeleted)
{
    // Verified at compile time via static_assert above; runtime guard
    SUCCEED();
}

// ---------------------------------------------------------------------------
// Shallow copy semantics — both point to the same object
// ---------------------------------------------------------------------------

TEST(ReferencePlain, CopySharesObject)
{
    int x = 10;
    auto a = feature::reference::plain<int>{x};
    auto b = a; // copy — shallow

    x = 99;
    EXPECT_EQ(feature::reference::plain<int>::value(a), 99);
    EXPECT_EQ(feature::reference::plain<int>::value(b), 99);
}

TEST(ReferencePlain, MutationThroughOneCopyVisibleThroughOther)
{
    int x = 0;
    auto a = feature::reference::plain<int>{x};
    auto b = a;

    // mutate via a's value reference
    feature::reference::plain<int>::value(a) = 77;
    EXPECT_EQ(feature::reference::plain<int>::value(b), 77);
    EXPECT_EQ(x, 77);
}

// ---------------------------------------------------------------------------
// cv-ref forwarding of value()
// ---------------------------------------------------------------------------

TEST(ReferencePlain, ValueLvalueRefFromLvalueExecutor)
{
    int x = 5;
    auto r = feature::reference::plain<int>{x};
    // lvalue executor → lvalue ref
    static_assert(std::is_same_v<decltype(feature::reference::plain<int>::value(r)), int &>);
    EXPECT_EQ(feature::reference::plain<int>::value(r), 5);
}

TEST(ReferencePlain, ValueConstRefFromConstExecutor)
{
    int x = 5;
    auto const r = feature::reference::plain<int>{x};
    // const lvalue executor → const ref
    static_assert(std::is_same_v<decltype(feature::reference::plain<int>::value(r)), int const &>);
    EXPECT_EQ(feature::reference::plain<int>::value(r), 5);
}

TEST(ReferencePlain, ValueRvalueRefFromRvalueExecutor)
{
    int x = 5;
    // rvalue executor → rvalue ref
    static_assert(std::is_same_v<
        decltype(feature::reference::plain<int>::value(feature::reference::plain<int>{x})), int &&>);
}

// ---------------------------------------------------------------------------
// execute() — no guard/unguard overhead
// ---------------------------------------------------------------------------

TEST(ReferencePlain, ExecuteInvokesFunc)
{
    int x = 10;
    auto r = feature::reference::plain<int>{x};
    auto rc = feature::reference::plain<int>::execute(r, []() { return 42; });
    EXPECT_EQ(rc, 42);
}

// ---------------------------------------------------------------------------
// Integration with wrapper
// ---------------------------------------------------------------------------

TEST(ReferencePlain, WrapperObservesOriginal)
{
    ::std::string s = "hello";
    wrapper<::std::string, feature::reference::plain> ref{s};

    // Execute mutates via wrapper_guard
    feature::wrapper_guard<decltype(ref) &> g{ref};
    g.value() += " world";

    EXPECT_EQ(s, "hello world");
}

TEST(ReferencePlain, WrapperCopySharesOriginal)
{
    int x = 0;
    wrapper<int, feature::reference::plain> ra{x};
    auto rb = ra; // copy — both observe x

    feature::wrapper_guard<decltype(ra) &>{ra}.value() = 55;
    EXPECT_EQ(feature::wrapper_guard<decltype(rb) &>{rb}.value(), 55);
    EXPECT_EQ(x, 55);
}
