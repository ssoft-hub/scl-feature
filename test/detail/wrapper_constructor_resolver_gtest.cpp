#include <gtest_utils.h>

#include <scl/feature/detail/wrapper_constructor_resolver.h>
#include <scl/feature/executor/inplace/plain.h>
#include <scl/feature/wrapper.h>

#include <memory>

using namespace ::scl;

namespace fd = ::scl::feature::detail;

// ---------------------------------------------------------------------------
// Type aliases
//
// Use fd::wrapper directly (bypasses wrapper_fold collapsing) so that nested
// wrappers with the same executor are genuinely distinct types:
//   W   = detail::wrapper<int, plain>
//   WW  = detail::wrapper<W,   plain>   — wraps W, not collapsed
//   WWW = detail::wrapper<WW,  plain>   — wraps WW, not collapsed
// ---------------------------------------------------------------------------

using W = fd::wrapper<int, feature::inplace::plain>;
using WW = fd::wrapper<W, feature::inplace::plain>;
using WWW = fd::wrapper<WW, feature::inplace::plain>;
using W_double = fd::wrapper<double, feature::inplace::plain>;

// ---------------------------------------------------------------------------
// Strategy 1 — is_compatible_with_v<Left, Right>:
//   returns executor_type cv-ref of right
// ---------------------------------------------------------------------------

TEST(WrapperConstructorResolverStrategy1, ReturnType)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<W::executor_type &,
        decltype(::std::declval<fd::wrapper_constructor_resolver<W, W &>>().resolve())>));
}

TEST(WrapperConstructorResolverStrategy1, ConstReturnType)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<W::executor_type const &,
        decltype(::std::declval<fd::wrapper_constructor_resolver<W, W const &>>().resolve())>));
}

TEST(WrapperConstructorResolverStrategy1, VolatileReturnType)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<W::executor_type volatile &,
        decltype(::std::declval<fd::wrapper_constructor_resolver<W, W volatile &>>().resolve())>));
}

TEST(WrapperConstructorResolverStrategy1, ConstVolatileReturnType)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<W::executor_type const volatile &,
        decltype(::std::declval<fd::wrapper_constructor_resolver<W, W const volatile &>>().resolve())>));
}

TEST(WrapperConstructorResolverStrategy1, Value)
{
    constexpr auto result = [] {
        W src{42};
        fd::wrapper_constructor_resolver<W, W &> r{src};
        return W::executor_type::value(r.resolve()) == 42;
    }();
    STATIC_EXPECT_TRUE(result);
}

TEST(WrapperConstructorResolverStrategy1, SameObject)
{
    constexpr auto result = [] {
        W src{42};
        fd::wrapper_constructor_resolver<W, W &> r{src};
        W::executor_type::value(r.resolve()) = 99;
        return W::executor_type::value(fd::executor_access::get(src)) == 99;
    }();
    STATIC_EXPECT_TRUE(result);
}

// ---------------------------------------------------------------------------
// Strategy 2 — is_compatible_with_part_of_v<Left, Right>:
//   Right is compatible with an inner part of Left — forward Right as-is
// ---------------------------------------------------------------------------

TEST(WrapperConstructorResolverStrategy2, ReturnType)
{
    // WW wraps W, so W is compatible with the inner part of WW
    STATIC_EXPECT_TRUE((::std::is_same_v<W &,
        decltype(::std::declval<fd::wrapper_constructor_resolver<WW, W &>>().resolve())>));
}

TEST(WrapperConstructorResolverStrategy2, SameObject)
{
    constexpr auto result = [] {
        W src{42};
        fd::wrapper_constructor_resolver<WW, W &> r{src};
        return ::std::addressof(r.resolve()) == ::std::addressof(src);
    }();
    STATIC_EXPECT_TRUE(result);
}

// ---------------------------------------------------------------------------
// Strategy 3 — is_wrapper_v<Right>: right is a wrapper (any), guard it and
//              recurse on its inner value
// ---------------------------------------------------------------------------

// left = W, right = WW (WW wraps W → strategy 3 peels WW → inner W → strategy 1)
TEST(WrapperConstructorResolverStrategy3, ReturnType)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<W::executor_type &,
        decltype(::std::declval<fd::wrapper_constructor_resolver<W, WW &>>().resolve())>));
}

TEST(WrapperConstructorResolverStrategy3, Value)
{
    constexpr auto result = [] {
        WW src{W{42}};
        fd::wrapper_constructor_resolver<W, WW &> r{src};
        return W::executor_type::value(r.resolve()) == 42;
    }();
    STATIC_EXPECT_TRUE(result);
}

// Incompatible wrapper (W_double, W): strategy 3 peels W → inner plain<int>& → strategy 4 → int&
TEST(WrapperConstructorResolverStrategy3, IncompatibleWrapperPeelsToInnerValue)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<int &,
        decltype(::std::declval<fd::wrapper_constructor_resolver<W_double, W &>>().resolve())>));
}

// ---------------------------------------------------------------------------
// Strategy 4 — plain (non-wrapper) value: forwarded as-is
// ---------------------------------------------------------------------------

TEST(WrapperConstructorResolverStrategy4, ReturnType)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<int &,
        decltype(::std::declval<fd::wrapper_constructor_resolver<W, int &>>().resolve())>));
}

TEST(WrapperConstructorResolverStrategy4, ConstReturnType)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<int const &,
        decltype(::std::declval<fd::wrapper_constructor_resolver<W, int const &>>().resolve())>));
}

TEST(WrapperConstructorResolverStrategy4, VolatileReturnType)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<int volatile &,
        decltype(::std::declval<fd::wrapper_constructor_resolver<W, int volatile &>>().resolve())>));
}

TEST(WrapperConstructorResolverStrategy4, ConstVolatileReturnType)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<int const volatile &,
        decltype(::std::declval<fd::wrapper_constructor_resolver<W, int const volatile &>>().resolve())>));
}

TEST(WrapperConstructorResolverStrategy4, SameObject)
{
    constexpr auto result = [] {
        int x = 42;
        fd::wrapper_constructor_resolver<W, int &> r{x};
        return ::std::addressof(r.resolve()) == ::std::addressof(x);
    }();
    STATIC_EXPECT_TRUE(result);
}

// ---------------------------------------------------------------------------
// Deep recursion — three levels: left = W, right = WWW
//   WWW → WW → W → executor of W
// ---------------------------------------------------------------------------

TEST(WrapperConstructorResolverDeepRecursion, ReturnType)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<W::executor_type &,
        decltype(::std::declval<fd::wrapper_constructor_resolver<W, WWW &>>().resolve())>));
}

TEST(WrapperConstructorResolverDeepRecursion, Value)
{
    constexpr auto result = [] {
        WWW src{WW{W{42}}};
        fd::wrapper_constructor_resolver<W, WWW &> r{src};
        return W::executor_type::value(r.resolve()) == 42;
    }();
    STATIC_EXPECT_TRUE(result);
}

// ---------------------------------------------------------------------------
// guarded_wrapper_constructor_resolver — direct tests
// ---------------------------------------------------------------------------

TEST(GuardedWrapperConstructorResolver, PeelsOneLayer)
{
    // left = W, right = WW& → inner = W& → strategy 1 → plain<int>&
    STATIC_EXPECT_TRUE((::std::is_same_v<W::executor_type &,
        decltype(::std::declval<fd::guarded_wrapper_constructor_resolver<W, WW &>>().resolve())>));
}

TEST(GuardedWrapperConstructorResolver, Value)
{
    constexpr auto result = [] {
        WW src{W{42}};
        fd::guarded_wrapper_constructor_resolver<W, WW &> r{src};
        return W::executor_type::value(r.resolve()) == 42;
    }();
    STATIC_EXPECT_TRUE(result);
}

TEST(GuardedWrapperConstructorResolver, DeepRecursion)
{
    // left = W, right = WWW& → peel two layers → plain<int>&
    STATIC_EXPECT_TRUE((::std::is_same_v<W::executor_type &,
        decltype(::std::declval<fd::guarded_wrapper_constructor_resolver<W, WWW &>>().resolve())>));
}
