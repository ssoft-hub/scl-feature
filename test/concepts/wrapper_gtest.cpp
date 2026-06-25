#include <gtest_utils.h>

#include <scl/feature/concepts/wrapper.h>
#include <scl/feature/executor/inplace/uninitialized.h>
#include <scl/feature/wrapper.h>

#include <string>

using ::scl::wrapper;
using namespace ::scl::feature;

template <typename Type>
using Plain = inplace::plain<Type>;
template <typename Type>
using Uninit = inplace::uninitialized<Type>;

TEST(WrapperTypeConcept, Satisfied)
{
    STATIC_EXPECT_TRUE(concepts::wrapper<wrapper<int>>);
    STATIC_EXPECT_TRUE(concepts::wrapper<wrapper<std::string>>);
    STATIC_EXPECT_TRUE((concepts::wrapper<wrapper<int, inplace::plain>>));
}

TEST(WrapperTypeConcept, NotSatisfied)
{
    STATIC_EXPECT_FALSE(concepts::wrapper<int>);
    STATIC_EXPECT_FALSE(concepts::wrapper<std::string>);
    STATIC_EXPECT_FALSE(concepts::wrapper<void>);
}

// ── compatible_with ──────────────────────────────────────────────────────────

TEST(CompatibleWithConcept, Satisfied)
{
    STATIC_EXPECT_TRUE((concepts::compatible_with<wrapper<int, Plain>, wrapper<int, Plain>>));
    STATIC_EXPECT_TRUE((concepts::compatible_with<int, int>));
}

TEST(CompatibleWithConcept, NotSatisfied)
{
    STATIC_EXPECT_FALSE((concepts::compatible_with<wrapper<int, Plain>, wrapper<double, Plain>>));
    STATIC_EXPECT_FALSE((concepts::compatible_with<wrapper<int, Plain>, wrapper<int, Uninit>>));
    STATIC_EXPECT_FALSE((concepts::compatible_with<int, wrapper<int, Plain>>));
}

// ── compatible_with_part_of ──────────────────────────────────────────────────

TEST(CompatibleWithPartOfConcept, Satisfied)
{
    using W = wrapper<int, Plain>;
    using WW = wrapper<W, Uninit>;

    STATIC_EXPECT_TRUE((concepts::compatible_with_part_of<WW, W>));
}

TEST(CompatibleWithPartOfConcept, NotSatisfied)
{
    using W = wrapper<int, Plain>;
    using WW = wrapper<W, Uninit>;

    STATIC_EXPECT_FALSE((concepts::compatible_with_part_of<W, WW>));
    STATIC_EXPECT_FALSE((concepts::compatible_with_part_of<int, W>));
}

// ── part_compatible_with ─────────────────────────────────────────────────────

TEST(PartCompatibleWithConcept, Satisfied)
{
    using W = wrapper<int, Plain>;
    using WW = wrapper<W, Uninit>;

    STATIC_EXPECT_TRUE((concepts::part_compatible_with<W, WW>));
}

TEST(PartCompatibleWithConcept, NotSatisfied)
{
    using W = wrapper<int, Plain>;
    using WW = wrapper<W, Uninit>;

    STATIC_EXPECT_FALSE((concepts::part_compatible_with<WW, W>));
    STATIC_EXPECT_FALSE((concepts::part_compatible_with<W, int>));
}

// ── convertible_from ─────────────────────────────────────────────────────────

TEST(ConvertibleFromConcept, PlainValue)
{
    STATIC_EXPECT_TRUE((concepts::convertible_from<int &, int &>));
    STATIC_EXPECT_TRUE((concepts::convertible_from<int const &, int &>));
    STATIC_EXPECT_FALSE((concepts::convertible_from<double &, int &>));
}

TEST(ConvertibleFromConcept, SingleWrapper)
{
    using W = wrapper<int, Plain>;

    STATIC_EXPECT_TRUE((concepts::convertible_from<W &, W &>));
    STATIC_EXPECT_TRUE((concepts::convertible_from<int &, W &>));
    STATIC_EXPECT_FALSE((concepts::convertible_from<double &, W &>));
}

TEST(ConvertibleFromConcept, MultiLevelWrapper)
{
    using W = wrapper<int, Plain>;
    using WW = wrapper<W, Uninit>;

    STATIC_EXPECT_TRUE((concepts::convertible_from<WW &, WW &>));
    STATIC_EXPECT_TRUE((concepts::convertible_from<W &, WW &>));
    STATIC_EXPECT_TRUE((concepts::convertible_from<int &, WW &>));
    STATIC_EXPECT_FALSE((concepts::convertible_from<double &, WW &>));
}

TEST(ConvertibleFromConcept, NonReferenceTarget)
{
    using W = wrapper<int, Plain>;
    using WW = wrapper<W, Uninit>;

    // Value copy from plain reference
    STATIC_EXPECT_TRUE((concepts::convertible_from<int, int &>));
    STATIC_EXPECT_TRUE((concepts::convertible_from<double, int &>));
    STATIC_EXPECT_FALSE((concepts::convertible_from<std::string, int &>));

    // Value copy reachable through wrapper chain
    STATIC_EXPECT_TRUE((concepts::convertible_from<int, W &>));
    STATIC_EXPECT_TRUE((concepts::convertible_from<double, W &>));
    STATIC_EXPECT_TRUE((concepts::convertible_from<int, WW &>));
    STATIC_EXPECT_TRUE((concepts::convertible_from<W, WW &>));
}
