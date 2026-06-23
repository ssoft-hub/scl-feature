#include <gtest_utils.h>

#include <scl/feature/concepts/wrapper.h>
#include <scl/feature/type_traits/executor.h>
#include <scl/feature/wrapper.h>

#include <type_traits>
#include <utility>

using namespace ::scl;

// ============================================================================
// Helper: access wrapped value for testing (not part of the public API)
// ============================================================================

template <::scl::feature::concepts::wrapper W>
constexpr decltype(auto) value_of(W && w) noexcept
{
    using RawW = ::std::remove_cvref_t<W>;
    return RawW::executor_type::value(::scl::feature::executor_trait<RawW>::executor(::std::forward<W>(w)));
}

// ============================================================================
// Tests — construction and value round-trip for all fundamental types
// ============================================================================

TEST(WrapperFundamental, Bool)
{
    constexpr wrapper<bool> wt{true};
    constexpr wrapper<bool> wf{false};
    STATIC_EXPECT_EQ(value_of(wt), true);
    STATIC_EXPECT_EQ(value_of(wf), false);
}

TEST(WrapperFundamental, Char)
{
    constexpr wrapper<char> w{'Z'};
    STATIC_EXPECT_EQ(value_of(w), 'Z');
}

TEST(WrapperFundamental, SignedChar)
{
    constexpr wrapper<signed char> w{static_cast<signed char>(-1)};
    STATIC_EXPECT_EQ(value_of(w), static_cast<signed char>(-1));
}

TEST(WrapperFundamental, UnsignedChar)
{
    constexpr wrapper<unsigned char> w{static_cast<unsigned char>(255)};
    STATIC_EXPECT_EQ(value_of(w), static_cast<unsigned char>(255));
}

TEST(WrapperFundamental, Short)
{
    constexpr wrapper<short> w{static_cast<short>(32767)};
    STATIC_EXPECT_EQ(value_of(w), static_cast<short>(32767));
}

TEST(WrapperFundamental, UnsignedShort)
{
    constexpr wrapper<unsigned short> w{static_cast<unsigned short>(65535)};
    STATIC_EXPECT_EQ(value_of(w), static_cast<unsigned short>(65535));
}

TEST(WrapperFundamental, Int)
{
    constexpr wrapper<int> w{42};
    STATIC_EXPECT_EQ(value_of(w), 42);
}

TEST(WrapperFundamental, UnsignedInt)
{
    constexpr wrapper<unsigned int> w{4'294'967'295u};
    STATIC_EXPECT_EQ(value_of(w), 4'294'967'295u);
}

TEST(WrapperFundamental, Long)
{
    constexpr wrapper<long> w{1'000'000L};
    STATIC_EXPECT_EQ(value_of(w), 1'000'000L);
}

TEST(WrapperFundamental, UnsignedLong)
{
    constexpr wrapper<unsigned long> w{1'000'000UL};
    STATIC_EXPECT_EQ(value_of(w), 1'000'000UL);
}

TEST(WrapperFundamental, LongLong)
{
    constexpr wrapper<long long> w{9'000'000'000LL};
    STATIC_EXPECT_EQ(value_of(w), 9'000'000'000LL);
}

TEST(WrapperFundamental, UnsignedLongLong)
{
    constexpr wrapper<unsigned long long> w{18'000'000'000ULL};
    STATIC_EXPECT_EQ(value_of(w), 18'000'000'000ULL);
}

TEST(WrapperFundamental, Float)
{
    constexpr wrapper<float> w{3.14f};
    STATIC_EXPECT_EQ(value_of(w), 3.14f);
}

TEST(WrapperFundamental, Double)
{
    constexpr wrapper<double> w{2.718281828};
    STATIC_EXPECT_EQ(value_of(w), 2.718281828);
}

TEST(WrapperFundamental, LongDouble)
{
    constexpr wrapper<long double> w{1.0L};
    STATIC_EXPECT_EQ(value_of(w), 1.0L);
}

// ============================================================================
// Tests — default construction (zero-initializes fundamental types)
// ============================================================================

TEST(WrapperFundamentalDefault, Int)
{
    constexpr wrapper<int> w{};
    STATIC_EXPECT_EQ(value_of(w), 0);
}

TEST(WrapperFundamentalDefault, Double)
{
    constexpr wrapper<double> w{};
    STATIC_EXPECT_EQ(value_of(w), 0.0);
}

TEST(WrapperFundamentalDefault, Bool)
{
    constexpr wrapper<bool> w{};
    STATIC_EXPECT_EQ(value_of(w), false);
}

TEST(WrapperFundamentalDefault, Char)
{
    constexpr wrapper<char> w{};
    STATIC_EXPECT_EQ(value_of(w), char{});
}

TEST(WrapperFundamentalDefault, LongLong)
{
    constexpr wrapper<long long> w{};
    STATIC_EXPECT_EQ(value_of(w), 0LL);
}

// ============================================================================
// Tests — copy and move construction preserve value
// ============================================================================

TEST(WrapperFundamentalCopy, CopyConstructInt)
{
    constexpr wrapper<int> src{99};
    constexpr wrapper<int> dst{src};
    STATIC_EXPECT_EQ(value_of(dst), 99);
}

TEST(WrapperFundamentalCopy, CopyConstructDouble)
{
    constexpr wrapper<double> src{2.718};
    constexpr wrapper<double> dst{src};
    STATIC_EXPECT_EQ(value_of(dst), 2.718);
}

TEST(WrapperFundamentalCopy, CopyConstructBool)
{
    constexpr wrapper<bool> src{true};
    constexpr wrapper<bool> dst{src};
    STATIC_EXPECT_EQ(value_of(dst), true);
}

TEST(WrapperFundamentalCopy, MoveConstructInt)
{
    wrapper<int> src{77};
    wrapper<int> dst{::std::move(src)};
    EXPECT_EQ(value_of(dst), 77);
}

TEST(WrapperFundamentalCopy, CopyAssignInt)
{
    wrapper<int> src{55};
    wrapper<int> dst{0};
    dst = src;
    EXPECT_EQ(value_of(dst), 55);
}

TEST(WrapperFundamentalCopy, MoveAssignInt)
{
    wrapper<int> src{33};
    wrapper<int> dst{0};
    dst = ::std::move(src);
    EXPECT_EQ(value_of(dst), 33);
}

TEST(WrapperFundamentalCopy, MutateValueThroughRef)
{
    wrapper<int> w{10};
    value_of(w) = 20;
    EXPECT_EQ(value_of(w), 20);
}

// ============================================================================
// Tests — type aliases
// ============================================================================

TEST(WrapperFundamentalTypes, IntValueType)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<wrapper<int>::value_type, int>));
}

TEST(WrapperFundamentalTypes, DoubleValueType)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<wrapper<double>::value_type, double>));
}

TEST(WrapperFundamentalTypes, BoolValueType)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<wrapper<bool>::value_type, bool>));
}

TEST(WrapperFundamentalTypes, CharValueType)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<wrapper<char>::value_type, char>));
}

TEST(WrapperFundamentalTypes, LongLongValueType)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<wrapper<long long>::value_type, long long>));
}

TEST(WrapperFundamentalTypes, IntExecutorType)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<wrapper<int>::executor_type, feature::inplace::plain<int>>));
}

TEST(WrapperFundamentalTypes, DoubleExecutorType)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<wrapper<double>::executor_type, feature::inplace::plain<double>>));
}

// ============================================================================
// Tests — constructibility / destructibility traits
// ============================================================================

TEST(WrapperFundamentalTraits, IntCopyConstructible)
{
    STATIC_EXPECT_TRUE(::std::is_copy_constructible_v<wrapper<int>>);
}

TEST(WrapperFundamentalTraits, IntMoveConstructible)
{
    STATIC_EXPECT_TRUE(::std::is_move_constructible_v<wrapper<int>>);
}

TEST(WrapperFundamentalTraits, IntDefaultConstructible)
{
    STATIC_EXPECT_TRUE(::std::is_default_constructible_v<wrapper<int>>);
}

TEST(WrapperFundamentalTraits, DoubleTriviallyDestructible)
{
    STATIC_EXPECT_TRUE(::std::is_trivially_destructible_v<wrapper<double>>);
}

TEST(WrapperFundamentalTraits, BoolNothrowCopyConstructible)
{
    STATIC_EXPECT_TRUE(::std::is_nothrow_copy_constructible_v<wrapper<bool>>);
}

TEST(WrapperFundamentalTraits, LongLongNothrowMoveConstructible)
{
    STATIC_EXPECT_TRUE(::std::is_nothrow_move_constructible_v<wrapper<long long>>);
}
