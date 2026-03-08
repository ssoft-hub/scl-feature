#include <gtest/gtest.h>

#include <scl/feature/concepts/wrapper.h>
#include <scl/feature/wrapper.h>

#include <string>

#define TEST_EXPECT_TRUE(X) \
    static_assert(X, #X);   \
    EXPECT_TRUE(X);

#define TEST_EXPECT_FALSE(X) \
    static_assert(!(X), #X); \
    EXPECT_FALSE(X);

using namespace ::scl;

TEST(WrapperTypeConcept, Satisfied)
{
    TEST_EXPECT_TRUE(feature::concepts::wrapper<wrapper<int>>);
    TEST_EXPECT_TRUE(feature::concepts::wrapper<wrapper<std::string>>);
    TEST_EXPECT_TRUE((feature::concepts::wrapper<wrapper<int, feature::inplace::plain>>));
}

TEST(WrapperTypeConcept, NotSatisfied)
{
    TEST_EXPECT_FALSE(feature::concepts::wrapper<int>);
    TEST_EXPECT_FALSE(feature::concepts::wrapper<std::string>);
    TEST_EXPECT_FALSE(feature::concepts::wrapper<void>);
}
