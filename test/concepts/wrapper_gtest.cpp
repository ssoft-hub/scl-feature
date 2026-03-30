#include <gtest_utils.h>

#include <scl/feature/concepts/wrapper.h>
#include <scl/feature/inplace/uninitialized.h>
#include <scl/feature/wrapper.h>

#include <string>

using namespace ::scl;

template <typename Type>
using Plain = feature::inplace::plain<Type>;
template <typename Type>
using Uninit = feature::inplace::uninitialized<Type>;

TEST(WrapperTypeConcept, Satisfied)
{
    STATIC_EXPECT_TRUE(feature::concepts::wrapper<wrapper<int>>);
    STATIC_EXPECT_TRUE(feature::concepts::wrapper<wrapper<std::string>>);
    STATIC_EXPECT_TRUE((feature::concepts::wrapper<wrapper<int, feature::inplace::plain>>));
}

TEST(WrapperTypeConcept, NotSatisfied)
{
    STATIC_EXPECT_FALSE(feature::concepts::wrapper<int>);
    STATIC_EXPECT_FALSE(feature::concepts::wrapper<std::string>);
    STATIC_EXPECT_FALSE(feature::concepts::wrapper<void>);
}

// ── compatible_with ──────────────────────────────────────────────────────────

TEST(CompatibleWithConcept, Satisfied)
{
    STATIC_EXPECT_TRUE((feature::concepts::compatible_with<wrapper<int, Plain>, wrapper<int, Plain>>));
    STATIC_EXPECT_TRUE((feature::concepts::compatible_with<int, int>));
}

TEST(CompatibleWithConcept, NotSatisfied)
{
    STATIC_EXPECT_FALSE((feature::concepts::compatible_with<wrapper<int, Plain>, wrapper<double, Plain>>));
    STATIC_EXPECT_FALSE((feature::concepts::compatible_with<wrapper<int, Plain>, wrapper<int, Uninit>>));
    STATIC_EXPECT_FALSE((feature::concepts::compatible_with<int, wrapper<int, Plain>>));
}

// ── compatible_with_part_of ──────────────────────────────────────────────────

TEST(CompatibleWithPartOfConcept, Satisfied)
{
    using W = wrapper<int, Plain>;
    using WW = wrapper<W, Uninit>;

    STATIC_EXPECT_TRUE((feature::concepts::compatible_with_part_of<WW, W>));
}

TEST(CompatibleWithPartOfConcept, NotSatisfied)
{
    using W = wrapper<int, Plain>;
    using WW = wrapper<W, Uninit>;

    STATIC_EXPECT_FALSE((feature::concepts::compatible_with_part_of<W, WW>));
    STATIC_EXPECT_FALSE((feature::concepts::compatible_with_part_of<int, W>));
}

// ── part_compatible_with ─────────────────────────────────────────────────────

TEST(PartCompatibleWithConcept, Satisfied)
{
    using W = wrapper<int, Plain>;
    using WW = wrapper<W, Uninit>;

    STATIC_EXPECT_TRUE((feature::concepts::part_compatible_with<W, WW>));
}

TEST(PartCompatibleWithConcept, NotSatisfied)
{
    using W = wrapper<int, Plain>;
    using WW = wrapper<W, Uninit>;

    STATIC_EXPECT_FALSE((feature::concepts::part_compatible_with<WW, W>));
    STATIC_EXPECT_FALSE((feature::concepts::part_compatible_with<W, int>));
}
