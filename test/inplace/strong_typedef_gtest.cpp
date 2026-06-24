#include <gtest/gtest.h>

#include <scl/feature/concepts/executor.h>
#include <scl/feature/inplace/strong_typedef.h>
#include <scl/feature/wrapper.h>

#include <type_traits>

#define TEST_EXPECT_TRUE(X) \
    static_assert(X, #X);   \
    EXPECT_TRUE(X);

#define TEST_EXPECT_FALSE(X) \
    static_assert(!(X), #X); \
    EXPECT_FALSE(X);

using namespace ::scl;

// Unique tag structs used throughout this TU
struct TagA
{};
struct TagB
{};
struct TagC
{};

using StrongA = feature::inplace::strong_typedef<TagA{}>;
using StrongB = feature::inplace::strong_typedef<TagB{}>;
using StrongC = feature::inplace::strong_typedef<TagC{}>;

// ---------------------------------------------------------------------------
// Concept satisfaction
// ---------------------------------------------------------------------------

TEST(StrongTypedefExecutor, SatisfiesExecutorConcept)
{
    TEST_EXPECT_TRUE(feature::concepts::executor<StrongA::type<int>>);
    TEST_EXPECT_TRUE(feature::concepts::executor<StrongA::type<double>>);
    TEST_EXPECT_TRUE(feature::concepts::executor<StrongB::type<int>>);
}

// ---------------------------------------------------------------------------
// Type identity
// ---------------------------------------------------------------------------

TEST(StrongTypedefType, SameTagSameValueIsSameType)
{
    TEST_EXPECT_TRUE((
        ::std::is_same_v<wrapper<int, StrongC::template type>, wrapper<int, StrongC::template type>>));
}

TEST(StrongTypedefType, DifferentTagsAreDistinctTypes)
{
    TEST_EXPECT_FALSE((
        ::std::is_same_v<wrapper<int, StrongA::template type>, wrapper<int, StrongB::template type>>));
}

TEST(StrongTypedefType, SameTagDifferentValueIsDistinctType)
{
    TEST_EXPECT_FALSE((::std::is_same_v<wrapper<int, StrongA::template type>,
        wrapper<double, StrongA::template type>>));
}

// ---------------------------------------------------------------------------
// Construction and value()
// ---------------------------------------------------------------------------

TEST(StrongTypedefWrapper, ConstructAndReadValue)
{
    wrapper<int, StrongA::template type> w{42};
    EXPECT_EQ(StrongA::type<int>::value(feature::detail::executor_access::get(w)), 42);
}

TEST(StrongTypedefWrapper, MakeStrongTypedefAlias)
{
    using UserId = feature::inplace::make_strong_typedef<TagA{}, int>;
    using OrderId = feature::inplace::make_strong_typedef<TagB{}, int>;

    TEST_EXPECT_FALSE((::std::is_same_v<UserId, OrderId>));

    UserId uid{1};
    OrderId oid{2};

    // Verify they hold the right values
    EXPECT_EQ(feature::inplace::strong_typedef<TagA{}>::type<int>::value(feature::detail::executor_access::get(uid)),
        1);
    EXPECT_EQ(feature::inplace::strong_typedef<TagB{}>::type<int>::value(feature::detail::executor_access::get(oid)),
        2);
}

// ---------------------------------------------------------------------------
// execute()
// ---------------------------------------------------------------------------

TEST(StrongTypedefWrapper, ExecuteInvokesCallable)
{
    wrapper<int, StrongA::template type> w{10};
    bool called = false;
    StrongA::type<int>::execute(feature::detail::executor_access::get(w), [&called]() {
        called = true;
    });
    EXPECT_TRUE(called);
}
