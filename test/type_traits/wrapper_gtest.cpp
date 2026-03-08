#include <gtest/gtest.h>

#include <scl/feature/type_traits/wrapper.h>
#include <scl/feature/wrapper.h>

#include <string>

#define TEST_EXPECT_TRUE(X) \
    static_assert(X, #X);   \
    EXPECT_TRUE(X);

#define TEST_EXPECT_FALSE(X) \
    static_assert(!(X), #X); \
    EXPECT_FALSE(X);

using namespace ::scl;

// ── is_wrapper ───────────────────────────────────────────────────────────────

TEST(IsWrapper, NonWrapperType)
{
    TEST_EXPECT_FALSE(feature::is_wrapper_v<int>);
    TEST_EXPECT_FALSE(feature::is_wrapper_v<std::string>);
    TEST_EXPECT_FALSE(feature::is_wrapper_v<void>);
}

TEST(IsWrapper, WrapperType)
{
    TEST_EXPECT_TRUE((feature::is_wrapper_v<wrapper<int, feature::inplace::plain>>));
    TEST_EXPECT_TRUE((feature::is_wrapper_v<wrapper<std::string, feature::inplace::plain>>));
}

TEST(IsWrapper, DefaultWrapper) { static_assert(feature::is_wrapper_v<wrapper<int>>); }

TEST(IsWrapper, CvStripped)
{
    TEST_EXPECT_TRUE(feature::is_wrapper_v<wrapper<int> const>);
    TEST_EXPECT_TRUE(feature::is_wrapper_v<wrapper<int> volatile>);
    TEST_EXPECT_TRUE(feature::is_wrapper_v<wrapper<int> const volatile>);
}
