#include <gtest_utils.h>

#include <scl/feature/wrapper.h>
#include <scl/feature/wrapper_cast.h>

#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace ::scl;

TEST(WrapperCreate, Int)
{
    [[maybe_unused]]
    constexpr wrapper<int> w{42};
}

TEST(WrapperCreate, IntExplicitTool)
{
    [[maybe_unused]]
    constexpr wrapper<int, feature::inplace::plain> w{42};
}

TEST(WrapperCreate, Double)
{
    [[maybe_unused]]
    constexpr wrapper<double> w{3.14};
}

TEST(WrapperCreate, String)
{
    [[maybe_unused]]
    wrapper<::std::string> w{"hello"};
}

TEST(WrapperCreate, StringView)
{
    [[maybe_unused]]
    constexpr wrapper<::std::string_view> w{"hello"};
}

TEST(WrapperCreate, Vector)
{
    [[maybe_unused]]
    wrapper<::std::vector<int>> w{1, 2, 3};
}

TEST(WrapperCreate, DefaultConstruct)
{
    [[maybe_unused]]
    constexpr wrapper<int> w{};
}

TEST(WrapperType, DuplicateToolCollapsed)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<wrapper<int, feature::inplace::plain, feature::inplace::plain>,
        wrapper<int, feature::inplace::plain>>));
}

TEST(WrapperType, NoToolDefaultsToPlain)
{
    STATIC_EXPECT_TRUE((::std::is_same_v<wrapper<int>, wrapper<int, feature::inplace::plain>>));
}

// ── Assignment ────────────────────────────────────────────────────────────────
// The executor has no assignment operator of its own; wrapper-to-wrapper
// assignment is performed value-semantically through the executor's execute().

TEST(WrapperAssign, SelfTypeCopyAssign)
{
    wrapper<int> a{42};
    wrapper<int> b{0};
    b = a;
    int const result = wrapper_cast(b);
    EXPECT_EQ(result, 42);
}

TEST(WrapperAssign, SelfTypeMoveAssign)
{
    wrapper<int> a{7};
    wrapper<int> b{0};
    b = ::std::move(a);
    int const result = wrapper_cast(b);
    EXPECT_EQ(result, 7);
}
