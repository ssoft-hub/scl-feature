#include <gtest_utils.h>

#include <scl/feature/wrapper.h>

#include <utility>

using namespace ::scl;

// ============================================================================
// Tests — member-direction operators are NOT reflected for fundamental types
//
// Fundamental types (int, double, etc.) have no member operator+(), operator==(),
// etc. — only built-in syntax.  The member (wrapper-left) reflected operators are
// constrained on requires { value.operator op(args...); }, which is false for
// non-class types, so no member overload is generated: `w + x`, `w < x` stay invalid.
//
// The reverse-operand hidden friends (wrapper-right) reflect `lhs op value`, which
// IS valid for fundamentals (`x + value`), so `x + w` works.  For `==` the C++20
// reversed-candidate rule additionally makes `w == x` resolve through that friend.
// ============================================================================

template <typename T, typename Arg>
constexpr bool has_binary_plus_v = requires { ::std::declval<T>() + ::std::declval<Arg>(); };

template <typename T, typename Arg>
constexpr bool has_equal_to_v = requires { ::std::declval<T>() == ::std::declval<Arg>(); };

template <typename T, typename Arg>
constexpr bool has_less_v = requires { ::std::declval<T>() < ::std::declval<Arg>(); };

template <typename T>
constexpr bool has_prefix_inc_v = requires { ++::std::declval<T>(); };

template <typename T, typename Arg>
constexpr bool has_plus_assign_v = requires { ::std::declval<T>() += ::std::declval<Arg>(); };

TEST(WrapperFundamentalNoOps, IntNoBinaryPlus)
{
    STATIC_EXPECT_FALSE((has_binary_plus_v<wrapper<int> &, int>));
    STATIC_EXPECT_FALSE((has_binary_plus_v<wrapper<int> const &, int>));
}

TEST(WrapperFundamentalNoOps, IntNoMemberLessThan)
{
    // `w < x` (wrapper-left) has no member overload, and `<` has no C++20 reversed
    // candidate, so it stays invalid even though `x < w` (reverse friend) is valid.
    STATIC_EXPECT_FALSE((has_less_v<wrapper<int> const &, int>));
}

TEST(WrapperFundamentalNoOps, IntNoPrefixIncrement)
{
    STATIC_EXPECT_FALSE(has_prefix_inc_v<wrapper<int> &>);
}

TEST(WrapperFundamentalNoOps, IntNoPlusAssign)
{
    STATIC_EXPECT_FALSE((has_plus_assign_v<wrapper<int> &, int>));
}

TEST(WrapperFundamentalNoOps, DoubleNoMemberBinaryPlus)
{
    STATIC_EXPECT_FALSE((has_binary_plus_v<wrapper<double> &, double>));
}

// ============================================================================
// Tests — reverse-operand operators ARE reflected for fundamental types
// ============================================================================

TEST(WrapperFundamentalReverse, IntReverseBinaryPlus)
{
    // `x + w` reflects `x + value`; `w + x` (member) stays unavailable.
    STATIC_EXPECT_TRUE((has_binary_plus_v<int, wrapper<int> &>));
    STATIC_EXPECT_FALSE((has_binary_plus_v<wrapper<int> &, int>));

    wrapper<int> w{10};
    EXPECT_EQ(5 + w, 15);
}

TEST(WrapperFundamentalReverse, IntReverseLessThan)
{
    STATIC_EXPECT_TRUE((has_less_v<int, wrapper<int> &>));

    wrapper<int> w{10};
    EXPECT_TRUE(5 < w);
    EXPECT_FALSE(20 < w);
}

TEST(WrapperFundamentalReverse, IntReverseEqualTo)
{
    // The reverse friend defines `x == w` (the portable guarantee).  `w == x` may also
    // resolve through it via C++20 reversed candidates, but that is compiler-dependent
    // (GCC selects it, Clang does not), so it is not asserted here.
    STATIC_EXPECT_TRUE((has_equal_to_v<int, wrapper<int> &>));

    wrapper<int> w{42};
    EXPECT_TRUE(42 == w);
    EXPECT_FALSE(7 == w);
}

TEST(WrapperFundamentalReverse, BoolReverseEqualTo)
{
    STATIC_EXPECT_TRUE((has_equal_to_v<bool, wrapper<bool> &>));
}
