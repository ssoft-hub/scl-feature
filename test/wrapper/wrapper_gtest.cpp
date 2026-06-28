#include <gtest_utils.h>

#include <scl/feature/wrapper.h>
#include <scl/feature/wrapper_cast.h>
#include <scl/utility/type_traits/forward_like.h>

#include <concepts>
#include <functional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

using namespace ::scl;

namespace
{
    // Test executor whose operator_assign override stores three times the
    // right-hand operand, so the executor-override assignment path is
    // distinguishable from the value path by its effect.  Copy/move
    // construction stay available (the wrapper copies the executor); only
    // assignment is deleted, as the executor concept requires.
    template <typename Value>
    class tripling_assign_executor
    {
        using self_type = tripling_assign_executor<Value>;

    public:
        using value_type = Value;

        template <typename... Args>
        constexpr explicit tripling_assign_executor(Args &&... args)
            : m_value{::std::forward<Args>(args)...}
        {}

        tripling_assign_executor(tripling_assign_executor const &) = default;
        tripling_assign_executor(tripling_assign_executor &&) = default;
        tripling_assign_executor & operator=(tripling_assign_executor const &) = delete;
        tripling_assign_executor & operator=(tripling_assign_executor &&) = delete;
        ~tripling_assign_executor() = default;

        template <typename Self, typename Func, typename... Args>
        static constexpr decltype(auto) execute(Self && /*self*/, Func && func, Args &&... args)
            requires(::std::same_as<::std::remove_cvref_t<Self>, self_type> && ::std::invocable<Func, Args && ...>)
        {
            return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
        }

        template <typename Self>
        static constexpr decltype(auto) access(Self && self)
            requires ::std::same_as<::std::remove_cvref_t<Self>, self_type>
        {
            return ::scl::forward_like<Self>(self.m_value);
        }

        static constexpr value_type operator_assign(self_type & self, value_type rhs) noexcept
        {
            self.m_value = rhs * 3;
            return self.m_value;
        }

    private:
        value_type m_value;
    };

    // A copy-constructible but non-assignable value type: the wrapper must expose
    // no assignment for it (the dispatch is SFINAE-friendly, not a hard error).
    struct non_assignable
    {
        non_assignable() = default;
        non_assignable(non_assignable const &) = default;
        non_assignable(non_assignable &&) = default;
        non_assignable & operator=(non_assignable const &) = delete;
        non_assignable & operator=(non_assignable &&) = delete;
        ~non_assignable() = default;
    };
} // namespace

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

// Regression: `wrapper = plain_value` must assign the wrapped value, exactly as
// every other reflected operator accepts a plain right-hand operand.  Before the
// assignment dispatch was unified this did not compile (the self-type overloads
// hid the reflected operator= and accepted only wrappers).
TEST(WrapperAssign, PlainValueAssignFundamental)
{
    wrapper<int> w{0};
    w = 42;
    int const result = wrapper_cast(w);
    EXPECT_EQ(result, 42);
}

TEST(WrapperAssign, PlainValueAssignClassType)
{
    wrapper<::std::string> w{"old"};
    w = ::std::string{"new"};
    ::std::string const result = wrapper_cast(w);
    EXPECT_EQ(result, "new");
}

// A wrapper may be assigned from a wrapper holding a different value type on the
// same executor template; the source is read through its own executor.
TEST(WrapperAssign, OtherWrapperDifferentValueType)
{
    wrapper<int> src{5};
    wrapper<long> dst{0};
    dst = src;
    long const result = wrapper_cast(dst);
    EXPECT_EQ(result, 5L);
}

// The executor's operator_assign override receives the raw right-hand operand
// (the same convention as every other reflected operator's operator_<name>
// override) and short-circuits the value path.
TEST(WrapperAssign, ExecutorOperatorAssignOverrideReceivesOperand)
{
    wrapper<int, tripling_assign_executor> w{0};
    w = 5;
    int const result = wrapper_cast(w);
    EXPECT_EQ(result, 15); // 5 * 3, via operator_assign
}

TEST(WrapperAssign, AssignableFromPlainValue)
{
    STATIC_EXPECT_TRUE((::std::is_assignable_v<wrapper<int> &, int>));
}

TEST(WrapperAssign, AssignableFromOtherWrapper)
{
    STATIC_EXPECT_TRUE((::std::is_assignable_v<wrapper<long> &, wrapper<int>>));
}

TEST(WrapperAssign, AssignableFromSelf)
{
    STATIC_EXPECT_TRUE(::std::is_copy_assignable_v<wrapper<int>>);
    STATIC_EXPECT_TRUE(::std::is_move_assignable_v<wrapper<int>>);
}

TEST(WrapperAssign, NonAssignableValueTypeIsNotAssignable)
{
    STATIC_EXPECT_FALSE(::std::is_copy_assignable_v<wrapper<non_assignable>>);
    STATIC_EXPECT_FALSE((::std::is_assignable_v<wrapper<non_assignable> &, non_assignable>));
}
