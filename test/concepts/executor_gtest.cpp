#include <gtest_utils.h>

#include <scl/feature/concepts/executor.h>
#include <scl/feature/inplace/plain.h>
#include <scl/feature/inplace/uninitialized.h>
#include <scl/utility/type_traits/forward_like.h>

using namespace ::scl::feature;

// ── Executor fixtures ─────────────────────────────────────────────────────────

// Minimal executor: has execute() and value(), no guard/unguard.
template <typename T>
struct MinimalExecutor
{
    using value_type = T;

    T m_value{};

    template <typename Self, typename Func>
    static constexpr decltype(auto) execute(Self &&, Func && func)
        requires ::std::same_as<::std::remove_cvref_t<Self>, MinimalExecutor>
    {
        return ::std::forward<Func>(func)();
    }

    template <typename Self>
    static constexpr decltype(auto) value(Self && self)
        requires ::std::same_as<::std::remove_cvref_t<Self>, MinimalExecutor>
    {
        return ::scl::forward_like<Self>(self.m_value);
    }
};

// No execute(), no value() — does not satisfy executor interface.
struct NoInterfaceExecutor
{};

// Has value() but no execute() — does not satisfy executor interface.
struct NoExecuteExecutor
{
    template <typename Self>
    static constexpr int & value(Self &&) noexcept;
};

// ── concepts::executor ───────────────────────────────────────────────────────

TEST(ExecutorConcept, Satisfied)
{
    STATIC_EXPECT_TRUE(concepts::executor<MinimalExecutor<int>>);
    STATIC_EXPECT_TRUE(concepts::executor<inplace::plain<int>>);
    STATIC_EXPECT_TRUE(concepts::executor<inplace::uninitialized<int>>);
}

TEST(ExecutorConcept, CvRefStripped)
{
    STATIC_EXPECT_TRUE(concepts::executor<inplace::plain<int> const>);
    STATIC_EXPECT_TRUE(concepts::executor<inplace::plain<int> volatile>);
    STATIC_EXPECT_TRUE(concepts::executor<inplace::plain<int> const volatile>);
    STATIC_EXPECT_TRUE(concepts::executor<inplace::plain<int> &>);
    STATIC_EXPECT_TRUE(concepts::executor<inplace::plain<int> &&>);
    STATIC_EXPECT_TRUE(concepts::executor<inplace::plain<int> const &>);
}

TEST(ExecutorConcept, NotSatisfied)
{
    STATIC_EXPECT_FALSE(concepts::executor<NoInterfaceExecutor>);
    STATIC_EXPECT_FALSE(concepts::executor<NoExecuteExecutor>);
    STATIC_EXPECT_FALSE(concepts::executor<int>);
    STATIC_EXPECT_FALSE(concepts::executor<void>);
}
