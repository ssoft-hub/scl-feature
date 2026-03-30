#include <gtest_utils.h>

#include <scl/feature/inplace/uninitialized.h>
#include <scl/feature/type_traits/wrapper.h>
#include <scl/feature/wrapper.h>

#include <string>

using namespace ::scl;

// ── is_wrapper ───────────────────────────────────────────────────────────────

TEST(IsWrapper, NonWrapperType)
{
    STATIC_EXPECT_FALSE(feature::is_wrapper_v<int>);
    STATIC_EXPECT_FALSE(feature::is_wrapper_v<std::string>);
    STATIC_EXPECT_FALSE(feature::is_wrapper_v<void>);
}

TEST(IsWrapper, WrapperType)
{
    STATIC_EXPECT_TRUE((feature::is_wrapper_v<wrapper<int, feature::inplace::plain>>));
    STATIC_EXPECT_TRUE((feature::is_wrapper_v<wrapper<std::string, feature::inplace::plain>>));
}

TEST(IsWrapper, DefaultWrapper) { STATIC_EXPECT_TRUE(feature::is_wrapper_v<wrapper<int>>); }

TEST(IsWrapper, CvStripped)
{
    STATIC_EXPECT_TRUE(feature::is_wrapper_v<wrapper<int> const>);
    STATIC_EXPECT_TRUE(feature::is_wrapper_v<wrapper<int> volatile>);
    STATIC_EXPECT_TRUE(feature::is_wrapper_v<wrapper<int> const volatile>);
}

// ── is_compatible_with ───────────────────────────────────────────────────────

template <typename Type>
using Plain = feature::inplace::plain<Type>;
template <typename Type>
using Uninit = feature::inplace::uninitialized<Type>;

TEST(IsCompatibleWith, SameType)
{
    STATIC_EXPECT_TRUE((feature::is_compatible_with_v<wrapper<int, Plain>, wrapper<int, Plain>>));
    STATIC_EXPECT_TRUE((feature::is_compatible_with_v<int, int>));
}

TEST(IsCompatibleWith, IncompatibleValueTypes)
{
    // same executor, but int and double are neither same nor related by inheritance
    STATIC_EXPECT_FALSE((feature::is_compatible_with_v<wrapper<int, Plain>, wrapper<double, Plain>>));
    STATIC_EXPECT_FALSE((feature::is_compatible_with_v<wrapper<double, Plain>, wrapper<int, Plain>>));
    STATIC_EXPECT_FALSE((feature::is_compatible_with_v<int, double>));
}

TEST(IsCompatibleWith, DifferentTemplateParam)
{
    STATIC_EXPECT_FALSE((feature::is_compatible_with_v<wrapper<int, Plain>, wrapper<int, Uninit>>));
    STATIC_EXPECT_FALSE((feature::is_compatible_with_v<wrapper<int, Uninit>, wrapper<int, Plain>>));
}

TEST(IsCompatibleWith, NonWrapperVsWrapper)
{
    STATIC_EXPECT_FALSE((feature::is_compatible_with_v<int, wrapper<int, Plain>>));
    STATIC_EXPECT_FALSE((feature::is_compatible_with_v<wrapper<int, Plain>, int>));
}

TEST(IsCompatibleWith, Inheritance)
{
    struct Base
    {};
    struct Derived : Base
    {};

    STATIC_EXPECT_TRUE((feature::is_compatible_with_v<Base, Derived>));
    STATIC_EXPECT_FALSE((feature::is_compatible_with_v<Derived, Base>));
    STATIC_EXPECT_TRUE((feature::is_compatible_with_v<wrapper<Base, Plain>, wrapper<Derived, Plain>>));
    STATIC_EXPECT_FALSE((feature::is_compatible_with_v<wrapper<Derived, Plain>, wrapper<Base, Plain>>));
}

TEST(IsCompatibleWith, CvStripped)
{
    STATIC_EXPECT_TRUE((feature::is_compatible_with_v<wrapper<int, Plain> const, wrapper<int, Plain>>));
    STATIC_EXPECT_TRUE((feature::is_compatible_with_v<wrapper<int, Plain>, wrapper<int, Plain> const>));
}

// ── is_compatible_with_part_of / is_part_compatible_with ─────────────────────
//
// Note: wrapper<wrapper<int,Plain>,Plain> collapses to wrapper<int,Plain> via
// wrapper_fold duplicate-executor deduplication. Tests use *alternating* executors
// (Plain / Uninit / Plain …) to produce genuinely nested detail::wrapper types:
//
//   W   = detail::wrapper<int,        Plain>
//   WW  = detail::wrapper<W,          Uninit>   ← Uninit ≠ Plain → no collapse
//   WWW = detail::wrapper<WW,         Plain>    ← Plain  ≠ Uninit → no collapse

// ── is_compatible_with_part_of ───────────────────────────────────────────────
// Semantics: Expected (a wrapper) recursively contains a value compatible with Test.

TEST(IsCompatibleWithPartOf, DirectInnerValue)
{
    using W = wrapper<int, Plain>;
    using WW = wrapper<W, Uninit>; // WW's inner value IS W

    STATIC_EXPECT_TRUE((feature::is_compatible_with_part_of_v<WW, W>));
    STATIC_EXPECT_FALSE((feature::is_compatible_with_part_of_v<W, WW>)); // W does not contain WW
}

TEST(IsCompatibleWithPartOf, MultiLevel)
{
    using W = wrapper<int, Plain>;
    using WW = wrapper<W, Uninit>;
    using WWW = wrapper<WW, Plain>;

    STATIC_EXPECT_TRUE((feature::is_compatible_with_part_of_v<WWW, WW>)); // one level deep
    STATIC_EXPECT_TRUE((feature::is_compatible_with_part_of_v<WWW, W>));  // two levels deep
}

TEST(IsCompatibleWithPartOf, SameWrapper)
{
    // A wrapper does not "contain" itself (inner value ≠ the whole)
    STATIC_EXPECT_FALSE((feature::is_compatible_with_part_of_v<wrapper<int, Plain>, wrapper<int, Plain>>));
}

TEST(IsCompatibleWithPartOf, NonWrapper)
{
    using W = wrapper<int, Plain>;
    STATIC_EXPECT_FALSE((feature::is_compatible_with_part_of_v<int, W>)); // Expected not a wrapper
    STATIC_EXPECT_FALSE((feature::is_compatible_with_part_of_v<W, int>)); // Test not a wrapper
    STATIC_EXPECT_FALSE((feature::is_compatible_with_part_of_v<int, int>));
}

TEST(IsCompatibleWithPartOf, CvStripped)
{
    using W = wrapper<int, Plain>;
    using WW = wrapper<W, Uninit>;

    STATIC_EXPECT_TRUE((feature::is_compatible_with_part_of_v<WW const, W>));
    STATIC_EXPECT_TRUE((feature::is_compatible_with_part_of_v<WW, W const>));
}

// ── is_part_compatible_with ───────────────────────────────────────────────────
// Semantics: Test (a wrapper) recursively contains a value compatible with Expected.

TEST(IsPartCompatibleWith, DirectInnerValue)
{
    using W = wrapper<int, Plain>;
    using WW = wrapper<W, Uninit>; // WW's inner value IS W

    STATIC_EXPECT_TRUE((feature::is_part_compatible_with_v<W, WW>));
    STATIC_EXPECT_FALSE((feature::is_part_compatible_with_v<WW, W>)); // W does not contain WW
}

TEST(IsPartCompatibleWith, MultiLevel)
{
    using W = wrapper<int, Plain>;
    using WW = wrapper<W, Uninit>;
    using WWW = wrapper<WW, Plain>;

    STATIC_EXPECT_TRUE((feature::is_part_compatible_with_v<WW, WWW>)); // one level deep
    STATIC_EXPECT_TRUE((feature::is_part_compatible_with_v<W, WWW>));  // two levels deep
}

TEST(IsPartCompatibleWith, SameWrapper)
{
    // A wrapper does not "contain" itself (inner value ≠ the whole)
    STATIC_EXPECT_FALSE((feature::is_part_compatible_with_v<wrapper<int, Plain>, wrapper<int, Plain>>));
}

TEST(IsPartCompatibleWith, NonWrapper)
{
    using W = wrapper<int, Plain>;
    STATIC_EXPECT_FALSE((feature::is_part_compatible_with_v<int, W>)); // Expected not a wrapper
    STATIC_EXPECT_FALSE((feature::is_part_compatible_with_v<W, int>)); // Test not a wrapper
    STATIC_EXPECT_FALSE((feature::is_part_compatible_with_v<int, int>));
}

TEST(IsPartCompatibleWith, CvStripped)
{
    using W = wrapper<int, Plain>;
    using WW = wrapper<W, Uninit>;

    STATIC_EXPECT_TRUE((feature::is_part_compatible_with_v<W const, WW>));
    STATIC_EXPECT_TRUE((feature::is_part_compatible_with_v<W, WW const>));
}
