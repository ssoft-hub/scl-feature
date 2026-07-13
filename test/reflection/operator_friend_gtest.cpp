#include <gtest_utils.h>

#include <scl/feature/reflection/operator.h>
#include <scl/feature/type_traits/executor.h>
#include <scl/utility/type_traits/forward_like.h>

#include <utility>

// ============================================================================
// FR12 — standalone friend declaration reflects the value's FREE operator.
//
// The wrapped types below define their operators as FREE (hidden-friend, i.e.
// non-member) functions — NO member operators.  The wrapper reflects them via
// SCL_REFLECT_FRIEND_* as hidden friends of the wrapper:
//   * `w op x` is found only by ADL — `w.operator op(...)` (member lookup) FAILS;
//   * the standalone declaration adds NO member path and does not obey member
//     precedence (there is no member to prefer).
//
// cv-ref discrimination uses the free operator's first (self) parameter:
//   operator+(FreeTarget &, int)       -> short
//   operator+(FreeTarget const &, int) -> int
// ============================================================================

// ============================================================================
// FriendTarget — FREE (hidden-friend) operators only, no members
// ============================================================================

struct FriendTarget
{
    int value = 0;

    // Binary +: FREE, distinguishable return types per cv-ref (via self param)
    friend constexpr short operator+(FriendTarget & self, int rhs)
    {
        return static_cast<short>(self.value + rhs);
    }
    friend constexpr int operator+(FriendTarget const & self, int rhs)
    {
        return self.value + rhs + 50;
    }

    // Unary prefix - (const& only), FREE
    friend constexpr int operator-(FriendTarget const & self) { return -self.value; }

    // Prefix ++ (mutable& only), FREE
    friend constexpr long operator++(FriendTarget & self) { return ++self.value; }

    // Postfix ++(int) (mutable& only), FREE
    friend constexpr short operator++(FriendTarget & self, int)
    {
        return static_cast<short>(self.value++);
    }
};

// ============================================================================
// Executor and wrapper — execute-path tests
// ============================================================================

struct FriendTargetExecutor
{
    FriendTarget m_value;

    template <typename Self, typename Func, typename... Args>
    static constexpr decltype(auto) execute(Self &&, Func && func, Args &&... args)
    {
        return ::std::forward<Func>(func)(::std::forward<Args>(args)...);
    }

    template <typename Self>
    static constexpr decltype(auto) access(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_value);
    }
};

struct FriendWrapped;

template <>
struct scl::feature::executor_trait<FriendWrapped>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_exec);
    }
};

struct FriendWrapped
{
    FriendTargetExecutor m_exec;

    SCL_REFLECT_TYPE(FriendWrapped, FriendTargetExecutor);

    constexpr explicit FriendWrapped(int v)
        : m_exec{v}
    {}

    SCL_REFLECT_FRIEND_BINARY_OPERATOR(+, op_add)
    SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR(-, op_neg)
    SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR(++, op_preinc)
    SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR(++, op_postinc)
};

// ============================================================================
// Executor override test types — override still wins over the free path (FR8)
// ============================================================================

struct OverridingFriendExecutor
{
    FriendTarget m_value;

    // Override for mutable lvalue: returns long (distinct from the free
    // operator+(FriendTarget&, int) -> short)
    static constexpr long operator_op_add(OverridingFriendExecutor & self, int v)
    {
        return static_cast<long>(self.m_value.value + v) * 100;
    }
    // No const& override → falls through to the free path → int (value + rhs + 50)

    template <typename Self, typename Func, typename... Args>
    static constexpr decltype(auto) execute(Self &&, Func && func, Args &&... args)
    {
        return ::std::forward<Func>(func)(::std::forward<Args>(args)...);
    }

    template <typename Self>
    static constexpr decltype(auto) access(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_value);
    }
};

struct OverridingFriendWrapped;

template <>
struct scl::feature::executor_trait<OverridingFriendWrapped>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_exec);
    }
};

struct OverridingFriendWrapped
{
    OverridingFriendExecutor m_exec;

    SCL_REFLECT_TYPE(OverridingFriendWrapped, OverridingFriendExecutor);

    constexpr explicit OverridingFriendWrapped(int v)
        : m_exec{FriendTarget{v}}
    {}

    SCL_REFLECT_FRIEND_BINARY_OPERATOR(+, op_add)
};

// ============================================================================
// noexcept propagation test types
// ============================================================================

struct FriendNxTarget
{
    int value = 0;

    // FREE noexcept / non-noexcept operators
    friend constexpr int operator+(FriendNxTarget const & self, int rhs) noexcept
    {
        return self.value + rhs;
    }
    friend constexpr int operator-(FriendNxTarget const & self, int rhs)
    {
        return self.value - rhs;
    }
};

struct FriendNxExecutor
{
    FriendNxTarget m_value;

    // noexcept override for nx_add → override path propagates noexcept(true)
    static constexpr int operator_nx_add(FriendNxExecutor const & self, int v) noexcept
    {
        return self.m_value.value + v;
    }
    // No override for nx_sub → free path; execute is not noexcept → noexcept(false)

    template <typename Self, typename Func, typename... Args>
    static constexpr decltype(auto) execute(Self &&, Func && func, Args &&... args)
    {
        return ::std::forward<Func>(func)(::std::forward<Args>(args)...);
    }

    template <typename Self>
    static constexpr decltype(auto) access(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_value);
    }
};

struct FriendWrappedNx;

template <>
struct scl::feature::executor_trait<FriendWrappedNx>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_exec);
    }
};

struct FriendWrappedNx
{
    FriendNxExecutor m_exec;
    SCL_REFLECT_TYPE(FriendWrappedNx, FriendNxExecutor);
    constexpr explicit FriendWrappedNx(int v)
        : m_exec{FriendNxTarget{v}}
    {}

    SCL_REFLECT_FRIEND_BINARY_OPERATOR(+, nx_add)
    SCL_REFLECT_FRIEND_BINARY_OPERATOR(-, nx_sub)
};

// ============================================================================
// Guard-counting target — the free-path operand must be reached under guard
// ============================================================================

namespace
{
    int g_friend_guard = 0;
    int g_friend_unguard = 0;

    struct GuardFriendExecutor
    {
        FriendTarget m_value;

        template <typename Self>
        static void guard(Self &&) noexcept
        {
            ++g_friend_guard;
        }
        template <typename Self>
        static void unguard(Self &&) noexcept
        {
            ++g_friend_unguard;
        }
        template <typename Self, typename Func, typename... Args>
        static constexpr decltype(auto) execute(Self &&, Func && func, Args &&... args)
        {
            return ::std::forward<Func>(func)(::std::forward<Args>(args)...);
        }
        template <typename Self>
        static constexpr decltype(auto) access(Self && self)
        {
            return ::scl::forward_like<Self>(self.m_value);
        }
    };
} // namespace

struct GuardFriendWrapped;

template <>
struct scl::feature::executor_trait<GuardFriendWrapped>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_exec);
    }
};

struct GuardFriendWrapped
{
    GuardFriendExecutor m_exec;
    SCL_REFLECT_TYPE(GuardFriendWrapped, GuardFriendExecutor);
    explicit GuardFriendWrapped(int v)
        : m_exec{FriendTarget{v}}
    {}

    SCL_REFLECT_FRIEND_BINARY_OPERATOR(+, op_add)
};

// ============================================================================
// Helper predicates
// ============================================================================

template <typename T, typename Arg>
constexpr bool can_friend_add_v = requires { ::std::declval<T>() + ::std::declval<Arg>(); };

template <typename T, typename Arg>
constexpr bool has_member_op_plus_v =
    requires { ::std::declval<T>().operator+(::std::declval<Arg>()); };

template <typename T>
constexpr bool can_prefix_inc_v = requires { ++::std::declval<T>(); };

template <typename T>
constexpr bool can_postfix_inc_v = requires { ::std::declval<T>()++; };

// ============================================================================
// Tests — SCL_REFLECT_FRIEND_BINARY_OPERATOR reflects the FREE operator
// ============================================================================

TEST(ReflectFriendOperator, FreeBinaryAddMutableLvalue)
{
    FriendWrapped w{10};
    auto result = w + 5;
    EXPECT_EQ(result, static_cast<short>(15)); // free operator+(FriendTarget&, int) -> short
}

TEST(ReflectFriendOperator, FreeBinaryAddConstLvalue)
{
    FriendWrapped const w{10};
    auto result = w + 5;
    EXPECT_EQ(result, 65); // free operator+(FriendTarget const&, int) -> int (10 + 5 + 50)
}

TEST(ReflectFriendOperator, FreeBinaryAddConstLvalueConstexpr)
{
    constexpr FriendWrapped w{10};
    STATIC_EXPECT_EQ(w + 5, 65);
}

TEST(ReflectFriendOperator, FreeUnaryNeg)
{
    FriendWrapped const w{7};
    EXPECT_EQ(-w, -7);
}

TEST(ReflectFriendOperator, FreeUnaryNegConstexpr)
{
    constexpr FriendWrapped w{7};
    STATIC_EXPECT_EQ(-w, -7);
}

TEST(ReflectFriendOperator, FreePrefixIncrement)
{
    FriendWrapped w{5};
    auto result = ++w;
    EXPECT_EQ(result, 6L);
    EXPECT_EQ(w + 0, static_cast<short>(6));
}

TEST(ReflectFriendOperator, FreePrefixIncNotAvailableOnConst)
{
    STATIC_EXPECT_FALSE(can_prefix_inc_v<FriendWrapped const &>);
}

TEST(ReflectFriendOperator, FreePostfixIncrement)
{
    FriendWrapped w{5};
    auto old_val = w++;
    EXPECT_EQ(old_val, static_cast<short>(5));
    EXPECT_EQ(w + 0, static_cast<short>(6));
}

TEST(ReflectFriendOperator, FreePostfixIncNotAvailableOnConst)
{
    STATIC_EXPECT_FALSE(can_postfix_inc_v<FriendWrapped const &>);
}

// ============================================================================
// Tests — FR12: ADL-only, member lookup must NOT work
// ============================================================================

TEST(ReflectFriendOperator, MemberLookupNotAvailable)
{
    // Standalone friend: a hidden-friend, not a member — `.operator+(...)` fails.
    STATIC_EXPECT_FALSE((has_member_op_plus_v<FriendWrapped &, int>));
}

TEST(ReflectFriendOperator, AdlLookupAvailable)
{
    // ADL finds the hidden friend.
    STATIC_EXPECT_TRUE((can_friend_add_v<FriendWrapped &, int>));
}

TEST(ReflectFriendOperator, RvalueBindsToConstLvalueFreeOverload)
{
    // The free operator+(FriendTarget const&, int) accepts an rvalue wrapper's value.
    STATIC_EXPECT_TRUE((can_friend_add_v<FriendWrapped &&, int>));
    STATIC_EXPECT_TRUE((::std::same_as<decltype(::std::declval<FriendWrapped &&>() + 5), int>));
}

// ============================================================================
// Tests — executor override still wins over the free path (FR8)
// ============================================================================

TEST(ReflectFriendOperatorExecutorOverride, MutableLValueCallsOverride)
{
    OverridingFriendWrapped w{5};
    auto result = w + 3;

    static_assert(::std::same_as<decltype(result), long>,
        "mutable-lvalue operator+ must return long — the override return type");

    EXPECT_EQ(result, (5 + 3) * 100L);
}

TEST(ReflectFriendOperatorExecutorOverride, ConstLValueFallsToFreePath)
{
    OverridingFriendWrapped const w{5};
    auto result = w + 3;

    static_assert(::std::same_as<decltype(result), int>,
        "const-lvalue operator+ must return int — free path, operator+(FriendTarget const&, int)");

    EXPECT_EQ(result, 58); // 5 + 3 + 50
}

TEST(ReflectFriendOperatorExecutorOverride, ReturnTypeIsOverrideNotTarget)
{
    OverridingFriendWrapped w{0};
    STATIC_EXPECT_FALSE((::std::same_as<decltype(w + 1), short>));
    STATIC_EXPECT_TRUE((::std::same_as<decltype(w + 1), long>));
}

TEST(ReflectFriendOperatorExecutorOverride, ConstUnaffectedByMutableOverride)
{
    OverridingFriendWrapped const cw{0};
    // const& has no override → free path → int
    STATIC_EXPECT_TRUE((::std::same_as<decltype(cw + 1), int>));
}

// ============================================================================
// Tests — noexcept propagation
// ============================================================================

TEST(ReflectFriendOperator, NoexceptPropagatesFromExecutorOverride)
{
    // FriendNxExecutor::operator_nx_add(const&, int) is noexcept → override path selected
    FriendWrappedNx const w{1};
    STATIC_EXPECT_TRUE(noexcept(w + 1));
}

TEST(ReflectFriendOperator, NoexceptNotPropagatedWhenNoExecutorOverride)
{
    // No executor override for nx_sub → free path; execute is not noexcept
    FriendWrappedNx const w{1};
    STATIC_EXPECT_FALSE(noexcept(w - 1));
}

// ============================================================================
// Tests — FR7: the free-path operand is reached under guard
// ============================================================================

TEST(ReflectFriendOperator, FreePathGuardsWrappedValueOnce)
{
    g_friend_guard = 0;
    g_friend_unguard = 0;

    GuardFriendWrapped w{10};
    auto result = w + 5; // free operator+ over the guarded value

    EXPECT_EQ(result, static_cast<short>(15));
    EXPECT_EQ(g_friend_guard, 1);
    EXPECT_EQ(g_friend_unguard, 1);
}
