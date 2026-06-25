#include <gtest_utils.h>

#include <scl/feature/executor/inplace/uninitialized.h>
#include <scl/feature/wrapper.h>

#include <string>

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
// Assignment — byte-copy of raw storage, constrained to trivially-copyable types
// ============================================================================

using TrivialUninit = feature::inplace::uninitialized<int>;

template <typename Dst, typename Src>
constexpr bool can_copy_assign_v = requires(Dst & dst, Src const & src) { dst = src; };

TEST(UninitializedAssignment, CopyAssign)
{
    TrivialUninit a;
    TrivialUninit b;
    TrivialUninit::value(a) = 42;
    b = a;
    EXPECT_EQ(TrivialUninit::value(b), 42);
}

TEST(UninitializedAssignment, MoveAssign)
{
    TrivialUninit a;
    TrivialUninit b;
    TrivialUninit::value(a) = 7;
    b = ::std::move(a);
    EXPECT_EQ(TrivialUninit::value(b), 7);
}

TEST(UninitializedAssignment, SelfAssignIsSafe)
{
    TrivialUninit a;
    TrivialUninit::value(a) = 99;
    a = a; // NOLINT(clang-diagnostic-self-assign-overloaded)
    EXPECT_EQ(TrivialUninit::value(a), 99);
}

TEST(UninitializedAssignment, ConstrainedToTriviallyCopyable)
{
    // int is trivially copyable → assignment available.
    STATIC_EXPECT_TRUE((can_copy_assign_v<TrivialUninit, TrivialUninit>));
    // std::string is not → no assignment operator is generated.
    using NonTrivialUninit = feature::inplace::uninitialized<::std::string>;
    STATIC_EXPECT_FALSE((can_copy_assign_v<NonTrivialUninit, NonTrivialUninit>));
}
