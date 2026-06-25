#include <gtest_utils.h>

#include <scl/feature/reflection/operator.h>
#include <scl/feature/type_traits/executor.h>
#include <scl/utility/type_traits/forward_like.h>

#include <utility>

// ============================================================================
// FriendTarget — wrapped type with qualified operators for friend reflection
// ============================================================================

struct FriendTarget
{
    int value = 0;

    // Binary +: distinguishable return types per cv-ref (required by SCL_HAS_QUALIFIED_*)
    constexpr short operator+(int rhs) & { return static_cast<short>(value + rhs); }
    constexpr int operator+(int rhs) const & { return value + rhs + 50; }

    // Unary prefix - (const& only)
    constexpr int operator-() const & { return -value; }

    // Prefix ++ (mutable& only)
    constexpr long operator++() & { return ++value; }

    // Postfix ++(int) (mutable& only)
    constexpr short operator++(int) & { return static_cast<short>(value++); }
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
    SCL_REFLECT_FRIEND_BINARY_OPERATOR(-, op_neg)
    SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR(++, op_preinc)
    SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR(++, op_postinc)
};

// ============================================================================
// Executor override test types
// ============================================================================

struct OverridingFriendExecutor
{
    FriendTarget m_value;

    // Override for mutable lvalue: returns long (distinct from FriendTarget::operator+(int)& → short)
    static constexpr long operator_op_add(OverridingFriendExecutor & self, int v)
    {
        return static_cast<long>(self.m_value.value + v) * 100;
    }
    // No const& override → falls through to execute path → int (value + rhs + 50)

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

    constexpr int operator+(int rhs) const & noexcept { return value + rhs; }
    constexpr int operator-(int rhs) const & { return value - rhs; }
};

struct FriendNxExecutor
{
    FriendNxTarget m_value;

    // noexcept override for nx_add → override path propagates noexcept(true)
    static constexpr int operator_nx_add(FriendNxExecutor const & self, int v) noexcept
    {
        return self.m_value.value + v;
    }
    // No override for nx_sub → execute path; execute is not noexcept → noexcept(false)

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
// Tests — SCL_REFLECT_FRIEND_BINARY_OPERATOR
// ============================================================================

TEST(ReflectFriendOperator, BinaryAddMutableLvalue)
{
    FriendWrapped w{10};
    auto result = w + 5;
    EXPECT_EQ(result, static_cast<short>(15));
}

TEST(ReflectFriendOperator, BinaryAddConstLvalue)
{
    FriendWrapped const w{10};
    auto result = w + 5;
    EXPECT_EQ(result, 65); // 10 + 5 + 50
}

TEST(ReflectFriendOperator, BinaryAddConstLvalueConstexpr)
{
    constexpr FriendWrapped w{10};
    STATIC_EXPECT_EQ(w + 5, 65);
}

TEST(ReflectFriendOperator, PrefixNeg)
{
    FriendWrapped const w{7};
    EXPECT_EQ(-w, -7);
}

TEST(ReflectFriendOperator, PrefixNegConstexpr)
{
    constexpr FriendWrapped w{7};
    STATIC_EXPECT_EQ(-w, -7);
}

// ============================================================================
// Tests — SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR
// ============================================================================

TEST(ReflectFriendOperator, PrefixIncrement)
{
    FriendWrapped w{5};
    auto result = ++w;
    EXPECT_EQ(result, 6L);
    // Underlying value changed — verify via operator+
    EXPECT_EQ(w + 0, static_cast<short>(6));
}

TEST(ReflectFriendOperator, PrefixIncNotAvailableOnConst)
{
    STATIC_EXPECT_FALSE(can_prefix_inc_v<FriendWrapped const &>);
}

// ============================================================================
// Tests — SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR
// ============================================================================

TEST(ReflectFriendOperator, PostfixIncrement)
{
    FriendWrapped w{5};
    auto old_val = w++;
    EXPECT_EQ(old_val, static_cast<short>(5));
    EXPECT_EQ(w + 0, static_cast<short>(6));
}

TEST(ReflectFriendOperator, PostfixIncNotAvailableOnConst)
{
    STATIC_EXPECT_FALSE(can_postfix_inc_v<FriendWrapped const &>);
}

// ============================================================================
// Tests — ADL-only: member lookup must NOT work
// ============================================================================

TEST(ReflectFriendOperator, MemberLookupNotAvailable)
{
    // Friend (hidden-friend) operator: not a member function, so .operator+(...) fails.
    STATIC_EXPECT_FALSE((has_member_op_plus_v<FriendWrapped &, int>));
}

TEST(ReflectFriendOperator, AdlLookupAvailable)
{
    // ADL finds the hidden friend.
    STATIC_EXPECT_TRUE((can_friend_add_v<FriendWrapped &, int>));
}

TEST(ReflectFriendOperator, BinaryAddRvalueCallsConstLvalueOverload)
{
    // FriendTarget has operator+(int) const & — rvalue wrapper binds to const&
    STATIC_EXPECT_TRUE((can_friend_add_v<FriendWrapped &&, int>));
    STATIC_EXPECT_TRUE((::std::same_as<decltype(::std::declval<FriendWrapped &&>() + 5), int>));
}

// ============================================================================
// Tests — executor override for friend operator
// ============================================================================

TEST(ReflectFriendOperatorExecutorOverride, MutableLValueCallsOverride)
{
    OverridingFriendWrapped w{5};
    auto result = w + 3;

    static_assert(::std::same_as<decltype(result), long>,
        "mutable-lvalue operator+ must return long — the override return type");

    EXPECT_EQ(result, (5 + 3) * 100L);
}

TEST(ReflectFriendOperatorExecutorOverride, ConstLValueFallsToExecutePath)
{
    OverridingFriendWrapped const w{5};
    auto result = w + 3;

    static_assert(::std::same_as<decltype(result), int>,
        "const-lvalue operator+ must return int — execute path, FriendTarget::operator+(int) const&");

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
    // const& has no override → execute path → int
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
    // No executor override for nx_sub → execute path; execute is not noexcept
    FriendWrappedNx const w{1};
    STATIC_EXPECT_FALSE(noexcept(w - 1));
}
