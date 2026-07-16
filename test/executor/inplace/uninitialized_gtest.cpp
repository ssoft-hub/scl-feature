#include <gtest_utils.h>

#include <scl/feature/executor/inplace/uninitialized.h>
#include <scl/feature/wrapper.h>

#include <string>
#include <type_traits>

using namespace ::scl;

TEST(UninitializedWrapper, Create)
{
    [[maybe_unused]]
    constexpr wrapper<int, feature::inplace::uninitialized> w_int{};
    [[maybe_unused]]
    constexpr wrapper<double, feature::inplace::uninitialized> w_double{};
    [[maybe_unused]]
    constexpr wrapper<::std::string, feature::inplace::uninitialized> w_string{};
}

// ============================================================================
// Assignment — the executor carries no assignment operator of its own; the
// wrapped value is assigned value-semantically through the wrapper.
// ============================================================================

using TrivialUninit = feature::inplace::uninitialized<int>;

template <typename Dst, typename Src>
constexpr bool can_copy_assign_v = requires(Dst & dst, Src const & src) { dst = src; };

TEST(UninitializedAssignment, ExecutorHasNoAssignment)
{
    // The executor itself is never assignable — the assignment of a wrapper's
    // value goes through the wrapper, not by assigning one executor to another.
    STATIC_EXPECT_FALSE((can_copy_assign_v<TrivialUninit, TrivialUninit>));
    STATIC_EXPECT_FALSE(::std::is_copy_assignable_v<TrivialUninit>);
    STATIC_EXPECT_FALSE(::std::is_move_assignable_v<TrivialUninit>);
}
