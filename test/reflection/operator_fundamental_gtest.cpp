#include <gtest_utils.h>

#include <scl/feature/wrapper.h>

#include <utility>

// ============================================================================
// Fundamental types under the full operator-reflection contract.
//
// Fundamental types (int, double, pointers) expose only built-in operator
// syntax — no member operators.  Under the contract these built-in operators
// are reflected via the FREE path in every direction:
//   FR3.forward  `w + x`, `w < x`
//   FR3.reverse  `x + w`, `x < w`
//   FR3.both     `w1 + w2`, `w1 < w2`
//   FR11 unary   `-w`, `~w`, `!w`
//   FR13 compound `w += x`
//   FR14 built-in subscript `wrapper<int*> w; w[i]`
// ============================================================================

using namespace ::scl;

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

// ============================================================================
// FR3.forward — `w op x` reflects the value's built-in operator (AC1)
// ============================================================================

TEST(WrapperFundamentalForward, IntBinaryPlus)
{
    STATIC_EXPECT_TRUE((has_binary_plus_v<wrapper<int> &, int>));
    STATIC_EXPECT_TRUE((has_binary_plus_v<wrapper<int> const &, int>));

    constexpr wrapper<int> w{10};
    STATIC_EXPECT_EQ(w + 5, 15);
}

TEST(WrapperFundamentalForward, IntLessThan)
{
    STATIC_EXPECT_TRUE((has_less_v<wrapper<int> const &, int>));

    constexpr wrapper<int> w{5};
    STATIC_EXPECT_TRUE(w < 6);
    STATIC_EXPECT_FALSE(w < 4);
}

TEST(WrapperFundamentalForward, IntEqualTo)
{
    constexpr wrapper<int> w{42};
    STATIC_EXPECT_TRUE(w == 42);
    STATIC_EXPECT_FALSE(w == 7);
}

TEST(WrapperFundamentalForward, DoubleBinaryPlus)
{
    STATIC_EXPECT_TRUE((has_binary_plus_v<wrapper<double> &, double>));

    constexpr wrapper<double> w{1.5};
    STATIC_EXPECT_TRUE((w + 0.5) == 2.0);
}

// ============================================================================
// FR3.reverse — `x op w` reflects `x op v` (AC2)
// ============================================================================

TEST(WrapperFundamentalReverse, IntReverseBinaryPlus)
{
    STATIC_EXPECT_TRUE((has_binary_plus_v<int, wrapper<int> &>));

    constexpr wrapper<int> w{10};
    STATIC_EXPECT_EQ(5 + w, 15);
}

TEST(WrapperFundamentalReverse, IntReverseLessThan)
{
    STATIC_EXPECT_TRUE((has_less_v<int, wrapper<int> &>));

    constexpr wrapper<int> w{10};
    STATIC_EXPECT_TRUE(5 < w);
    STATIC_EXPECT_FALSE(20 < w);
}

TEST(WrapperFundamentalReverse, IntReverseEqualTo)
{
    STATIC_EXPECT_TRUE((has_equal_to_v<int, wrapper<int> &>));

    constexpr wrapper<int> w{42};
    STATIC_EXPECT_TRUE(42 == w);
    STATIC_EXPECT_FALSE(7 == w);
}

// ============================================================================
// FR3.both — `w1 op w2` reflects `v1 op v2` (AC3)
// ============================================================================

TEST(WrapperFundamentalBoth, IntPlus)
{
    constexpr wrapper<int> a{2};
    constexpr wrapper<int> b{3};
    STATIC_EXPECT_EQ(a + b, 5);
}

TEST(WrapperFundamentalBoth, IntLessThan)
{
    constexpr wrapper<int> a{2};
    constexpr wrapper<int> b{3};
    STATIC_EXPECT_TRUE(a < b);
    STATIC_EXPECT_FALSE(b < a);
}

// ============================================================================
// FR11 — unary built-in operators (AC13)
// ============================================================================

TEST(WrapperFundamentalUnary, NegateComplementNot)
{
    constexpr wrapper<int> w{5};
    STATIC_EXPECT_EQ(-w, -5);
    STATIC_EXPECT_EQ(~w, ~5);
    STATIC_EXPECT_FALSE(!w); // 5 is truthy
}

TEST(WrapperFundamentalUnary, PrefixIncrement)
{
    STATIC_EXPECT_TRUE(has_prefix_inc_v<wrapper<int> &>);

    wrapper<int> w{5};
    ++w;
    EXPECT_EQ(w + 0, 6);
}

// ============================================================================
// FR13 — compound assignment (AC20)
// ============================================================================

TEST(WrapperFundamentalCompound, PlusAssign)
{
    STATIC_EXPECT_TRUE((has_plus_assign_v<wrapper<int> &, int>));

    wrapper<int> w{10};
    w += 5;
    EXPECT_EQ(w + 0, 15);
}

// ============================================================================
// FR14 — built-in subscript on a pointer value (AC14)
// ============================================================================

TEST(WrapperFundamentalSubscript, PointerBuiltinSubscript)
{
    static int arr[] = {10, 20, 30};
    wrapper<int *> w{arr};
    EXPECT_EQ(w[2], arr[2]); // built-in int* subscript, reflected under guard
    EXPECT_EQ(w[0], 10);
}
