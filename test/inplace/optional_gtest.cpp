#include <gtest/gtest.h>

#include <scl/feature/inplace/optional.h>
#include <scl/feature/wrapper.h>

#include <string>

// ---------------------------------------------------------------------------
// Minimal gtest_utils subset — avoids a dependency on a file that may not
// exist in the test include path for this subdirectory.
// ---------------------------------------------------------------------------
#define STATIC_EXPECT_TRUE(expr)  \
    static_assert((expr), #expr); \
    EXPECT_TRUE(expr)

#define STATIC_EXPECT_FALSE(expr)           \
    static_assert(!(expr), "!(" #expr ")"); \
    EXPECT_FALSE(expr)

#define STATIC_EXPECT_EQ(a, b)               \
    static_assert((a) == (b), #a " == " #b); \
    EXPECT_EQ(a, b)

using namespace ::scl;

// ---------------------------------------------------------------------------
// Default construction — empty state
// ---------------------------------------------------------------------------

TEST(InplaceOptional, DefaultConstructEmpty)
{
    wrapper<int, feature::inplace::optional> w{};
    EXPECT_FALSE(w.has_value());
    EXPECT_FALSE(static_cast<bool>(w));
}

// ---------------------------------------------------------------------------
// value() on empty throws std::bad_optional_access
// ---------------------------------------------------------------------------

TEST(InplaceOptional, ValueOnEmptyThrows)
{
    wrapper<int, feature::inplace::optional> w{};
    EXPECT_THROW((void)feature::inplace::optional<int>::value(feature::detail::executor_access::get(w)),
        ::std::bad_optional_access);
}

// ---------------------------------------------------------------------------
// emplace / has_value / value
// ---------------------------------------------------------------------------

TEST(InplaceOptional, EmplaceAndHasValue)
{
    wrapper<int, feature::inplace::optional> w{};
    w.emplace(42);
    EXPECT_TRUE(w.has_value());
    EXPECT_TRUE(static_cast<bool>(w));
    auto & exec = feature::detail::executor_access::get(w);
    EXPECT_EQ(feature::inplace::optional<int>::value(exec), 42);
}

// ---------------------------------------------------------------------------
// reset
// ---------------------------------------------------------------------------

TEST(InplaceOptional, Reset)
{
    wrapper<int, feature::inplace::optional> w{};
    w.emplace(7);
    EXPECT_TRUE(w.has_value());
    w.reset();
    EXPECT_FALSE(w.has_value());
}

// ---------------------------------------------------------------------------
// value_or
// ---------------------------------------------------------------------------

TEST(InplaceOptional, ValueOrEmpty)
{
    wrapper<int, feature::inplace::optional> w{};
    EXPECT_EQ(w.value_or(0), 0);
}

TEST(InplaceOptional, ValueOrEngaged)
{
    wrapper<int, feature::inplace::optional> w{99};
    EXPECT_EQ(w.value_or(0), 99);
}

// ---------------------------------------------------------------------------
// Direct-value construction
// ---------------------------------------------------------------------------

TEST(InplaceOptional, ValueConstructEngaged)
{
    wrapper<int, feature::inplace::optional> w{42};
    EXPECT_TRUE(w.has_value());
    auto & exec = feature::detail::executor_access::get(w);
    EXPECT_EQ(feature::inplace::optional<int>::value(exec), 42);
}

// ---------------------------------------------------------------------------
// Copy: independent values (deep copy)
// ---------------------------------------------------------------------------

TEST(InplaceOptional, CopyEngagedIsDeep)
{
    wrapper<int, feature::inplace::optional> a{10};
    wrapper<int, feature::inplace::optional> b{a};

    EXPECT_TRUE(b.has_value());

    // Mutate a to verify independence
    a.emplace(20);
    auto & exec_b = feature::detail::executor_access::get(b);
    EXPECT_EQ(feature::inplace::optional<int>::value(exec_b), 10);
}

TEST(InplaceOptional, CopyEmptyStaysEmpty)
{
    wrapper<int, feature::inplace::optional> a{};
    wrapper<int, feature::inplace::optional> b{a};
    EXPECT_FALSE(b.has_value());
}

// ---------------------------------------------------------------------------
// cv-ref forwarding on value()
// ---------------------------------------------------------------------------

TEST(InplaceOptional, CvRefForwarding)
{
    using exec_t = feature::inplace::optional<int>;

    // lvalue → lvalue ref
    exec_t e{};
    e.emplace(5);
    decltype(auto) lv = exec_t::value(e);
    static_assert(::std::is_lvalue_reference_v<decltype(lv)>);
    EXPECT_EQ(lv, 5);

    // rvalue → rvalue ref
    exec_t e2{};
    e2.emplace(7);
    decltype(auto) rv = exec_t::value(::std::move(e2));
    static_assert(::std::is_rvalue_reference_v<decltype(rv)>);
    EXPECT_EQ(rv, 7);

    // const lvalue → const lvalue ref
    exec_t const ce{};
    // ce is empty (default-ctor) — use emplace via const_cast only to set up state
    exec_t ce2{};
    ce2.emplace(9);
    exec_t const & cref = ce2;
    decltype(auto) clv = exec_t::value(cref);
    static_assert(::std::is_lvalue_reference_v<decltype(clv)>);
    static_assert(::std::is_const_v<::std::remove_reference_t<decltype(clv)>>);
    EXPECT_EQ(clv, 9);
}

// ---------------------------------------------------------------------------
// wrapper<std::string, inplace::optional> round-trip
// ---------------------------------------------------------------------------

TEST(InplaceOptional, StringRoundTrip)
{
    wrapper<::std::string, feature::inplace::optional> w{};
    EXPECT_FALSE(w.has_value());

    w.emplace("hello");
    EXPECT_TRUE(w.has_value());

    auto & exec = feature::detail::executor_access::get(w);
    EXPECT_EQ(feature::inplace::optional<::std::string>::value(exec), "hello");

    w.reset();
    EXPECT_FALSE(w.has_value());
}
