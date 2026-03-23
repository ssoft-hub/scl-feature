#include <gtest/gtest.h>

#include <scl/feature/detail/wrapper_constructor_resolver.h>
#include <scl/feature/inplace/plain.h>
#include <scl/feature/wrapper.h>

#include <utility>

#define TEST_EXPECT_TRUE(X) \
    static_assert(X, #X);   \
    EXPECT_TRUE(X);

#define TEST_EXPECT_FALSE(X) \
    static_assert(!(X), #X); \
    EXPECT_FALSE(X);

using namespace ::scl;

namespace fd = ::scl::feature::detail;

// Use fd::wrapper directly to avoid wrapper_fold collapsing same-executor types.
using W = fd::wrapper<int, feature::inplace::plain>;
using WW = fd::wrapper<W, feature::inplace::plain>;
using WWW = fd::wrapper<WW, feature::inplace::plain>;

// ---------------------------------------------------------------------------
// Self-type constructors (SCL_WRAPPER_CONSTRUCTOR_FOR_SELF)
// All 8 cv-ref variants go through the resolver — guard()/unguard() are called.
// ---------------------------------------------------------------------------

TEST(WrapperConstructorSelf, FromLvalueRef)
{
    constexpr auto result = [] {
        W src{42};
        W dst{src};
        return W::executor_type::value(fd::executor_access::get(dst)) == 42;
    }();
    TEST_EXPECT_TRUE(result);
}

TEST(WrapperConstructorSelf, FromConstLvalueRef)
{
    constexpr auto result = [] {
        W const src{42};
        W dst{src};
        return W::executor_type::value(fd::executor_access::get(dst)) == 42;
    }();
    TEST_EXPECT_TRUE(result);
}

TEST(WrapperConstructorSelf, FromVolatileLvalueRef)
{
    W volatile src{42};
    W dst{src};
    EXPECT_EQ(W::executor_type::value(fd::executor_access::get(dst)), 42);
}

TEST(WrapperConstructorSelf, FromConstVolatileLvalueRef)
{
    W const volatile src{42};
    W dst{src};
    EXPECT_EQ(W::executor_type::value(fd::executor_access::get(dst)), 42);
}

TEST(WrapperConstructorSelf, FromRvalueRef)
{
    constexpr auto result = [] {
        W src{42};
        W dst{::std::move(src)};
        return W::executor_type::value(fd::executor_access::get(dst)) == 42;
    }();
    TEST_EXPECT_TRUE(result);
}

TEST(WrapperConstructorSelf, FromConstRvalueRef)
{
    constexpr auto result = [] {
        W const src{42};
        W dst{::std::move(src)};
        return W::executor_type::value(fd::executor_access::get(dst)) == 42;
    }();
    TEST_EXPECT_TRUE(result);
}

// Verify that copy and move constructors are NOT implicitly generated
// (i.e. they go through the resolver, not the compiler-generated defaults).
TEST(WrapperConstructorSelf, CopyConstructorIsNotTrivial)
{
    TEST_EXPECT_FALSE(::std::is_trivially_copy_constructible_v<W>);
}

TEST(WrapperConstructorSelf, MoveConstructorIsNotTrivial)
{
    TEST_EXPECT_FALSE(::std::is_trivially_move_constructible_v<W>);
}

TEST(WrapperConstructorSelf, IsCopyConstructible)
{
    TEST_EXPECT_TRUE(::std::is_copy_constructible_v<W>);
}

TEST(WrapperConstructorSelf, IsMoveConstructible)
{
    TEST_EXPECT_TRUE(::std::is_move_constructible_v<W>);
}

// ---------------------------------------------------------------------------
// Other-wrapper constructors (SCL_WRAPPER_CONSTRUCTOR_FOR_OTHER)
// ---------------------------------------------------------------------------

// Explicit construction from compatible wrapper (strategy 2: WW wraps W)
TEST(WrapperConstructorOther, ExplicitFromInnerWrapper)
{
    constexpr auto result = [] {
        W src{42};
        WW dst{src};
        return W::executor_type::value(fd::executor_access::get(
                   WW::executor_type::value(fd::executor_access::get(dst)))) == 42;
    }();
    TEST_EXPECT_TRUE(result);
}

// Implicit conversion from compatible wrapper
TEST(WrapperConstructorOther, ImplicitFromInnerWrapper)
{
    constexpr auto result = [] {
        W src{42};
        WW dst = src; // implicit — only works via SCL_WRAPPER_CONSTRUCTOR_FOR_OTHER
        return W::executor_type::value(fd::executor_access::get(
                   WW::executor_type::value(fd::executor_access::get(dst)))) == 42;
    }();
    TEST_EXPECT_TRUE(result);
}

// Construction from outer wrapper (strategy 3: W from WW, peel one layer)
TEST(WrapperConstructorOther, FromOuterWrapper)
{
    constexpr auto result = [] {
        WW src{W{42}};
        W dst{src};
        return W::executor_type::value(fd::executor_access::get(dst)) == 42;
    }();
    TEST_EXPECT_TRUE(result);
}

// Deep construction: W from WWW (peel two layers)
TEST(WrapperConstructorOther, FromDeepOuterWrapper)
{
    constexpr auto result = [] {
        WWW src{WW{W{42}}};
        W dst{src};
        return W::executor_type::value(fd::executor_access::get(dst)) == 42;
    }();
    TEST_EXPECT_TRUE(result);
}

// Construction from rvalue wrapper
TEST(WrapperConstructorOther, FromRvalueOtherWrapper)
{
    constexpr auto result = [] {
        W dst{WW{W{99}}};
        return W::executor_type::value(fd::executor_access::get(dst)) == 99;
    }();
    TEST_EXPECT_TRUE(result);
}
