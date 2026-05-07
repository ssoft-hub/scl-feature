#include <gtest_utils.h>

#include <scl/feature/reflection/operator.h>
#include <scl/feature/type_traits/executor.h>
#include <scl/utility/type_traits/forward_like.h>

#include <utility>

// ============================================================================
// Target — wrapped type with various qualified operators
// ============================================================================

struct Target
{
    int value = 0;

    // Binary operator+ — distinguishable return types per cv-ref (required by SCL_HAS_QUALIFIED_*)
    constexpr short operator+(int rhs) & { return static_cast<short>(value + rhs); }
    constexpr int operator+(int rhs) const & { return value + rhs + 50; }

    // Prefix unary operator- (const & only, single overload)
    constexpr int operator-() const & { return -value; }

    // Prefix operator++ (mutable & only)
    constexpr long operator++() & { return ++value; }

    // Postfix operator++(int) (mutable & only)
    constexpr short operator++(int) & { return static_cast<short>(value++); }

    // operator= (must be member on wrapper)
    constexpr long operator=(int rhs) &
    {
        value = rhs;
        return value;
    }

    // operator[] — distinguishable return types per cv-ref
    constexpr short operator[](int idx) & { return static_cast<short>(value + idx); }
    constexpr int operator[](int idx) const & { return value + idx + 50; }

    // operator() — mutable (no args) and const (one arg), different signatures
    constexpr long operator()() & { return value; }
    constexpr int operator()(int x) const & { return value + x + 50; }
};

// ============================================================================
// Executor and wrapper — execute-path tests
// ============================================================================

struct TargetExecutor
{
    Target m_value;

    template <typename Self, typename Func, typename... Args>
    static constexpr decltype(auto) execute(Self &&, Func && func, Args &&... args)
    {
        return ::std::forward<Func>(func)(::std::forward<Args>(args)...);
    }

    template <typename Self>
    static constexpr decltype(auto) value(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_value);
    }
};

struct WrappedTarget;

template <>
struct scl::feature::executor_trait<WrappedTarget>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_exec);
    }
};

struct WrappedTarget
{
    TargetExecutor m_exec;

    SCL_REFLECT_TYPE(WrappedTarget, TargetExecutor);

    constexpr explicit WrappedTarget(int v)
        : m_exec{v}
    {}

    // Friend-inline: binary + (and unary + when called with no args — here unary variant
    // is absent on Target, so only the binary overloads survive the requires clause)
    SCL_REFLECT_BINARY_OPERATOR(+, op_add)

    // Friend-inline: unary prefix -
    SCL_REFLECT_BINARY_OPERATOR(-, op_neg)

    // Friend-inline: prefix ++
    SCL_REFLECT_PREFIX_UNARY_OPERATOR(++, op_preinc)

    // Friend-inline: postfix ++
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR(++, op_postinc)

    // Member: operator=
    SCL_REFLECT_BINARY_OPERATOR(=, op_assign)

    // Member: operator[]
    SCL_REFLECT_OPERATOR_WITH_ARGUMENTS([], op_subscript)

    // Member: operator()
    SCL_REFLECT_OPERATOR_WITH_ARGUMENTS((), op_call)
};

// ============================================================================
// Executor override test types
// ============================================================================

struct OverridingOpExecutor
{
    Target m_value;

    // Override for mutable lvalue: returns long (distinct from Target::operator+(int)& → short)
    static constexpr long operator_op_add(OverridingOpExecutor & self, int v)
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
    static constexpr decltype(auto) value(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_value);
    }
};

struct OverridingWrapped;

template <>
struct scl::feature::executor_trait<OverridingWrapped>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_exec);
    }
};

struct OverridingWrapped
{
    OverridingOpExecutor m_exec;

    SCL_REFLECT_TYPE(OverridingWrapped, OverridingOpExecutor);

    constexpr explicit OverridingWrapped(int v)
        : m_exec{Target{v}}
    {}

    SCL_REFLECT_BINARY_OPERATOR(+, op_add)
};

// ============================================================================
// noexcept propagation test types
// ============================================================================

struct NxTarget
{
    int value = 0;

    constexpr int operator+(int rhs) const & noexcept { return value + rhs; }
    constexpr int operator-(int rhs) const & { return value - rhs; }
};

struct NxExecutor
{
    NxTarget m_value;

    // noexcept executor override for nx_add → override path propagates noexcept(true)
    static constexpr int operator_nx_add(NxExecutor const & self, int v) noexcept
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
    static constexpr decltype(auto) value(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_value);
    }
};

struct WrappedNx;

template <>
struct scl::feature::executor_trait<WrappedNx>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_exec);
    }
};

struct WrappedNx
{
    NxExecutor m_exec;
    SCL_REFLECT_TYPE(WrappedNx, NxExecutor);
    constexpr explicit WrappedNx(int v)
        : m_exec{NxTarget{v}}
    {}

    SCL_REFLECT_BINARY_OPERATOR(+, nx_add)
    SCL_REFLECT_BINARY_OPERATOR(-, nx_sub)
};

// ============================================================================
// Helper: callability checks
// ============================================================================

template <typename T, typename... Args>
constexpr bool can_call_add_v =
    requires { ::std::declval<T>().operator+(::std::declval<Args>()...); };

template <typename T>
constexpr bool can_call_preinc_v = requires { ::std::declval<T>().operator++(); };

template <typename T>
constexpr bool can_call_postinc_v = requires { ::std::declval<T>().operator++(0); };

template <typename T, typename... Args>
constexpr bool can_call_subscript_v =
    requires { ::std::declval<T>().operator[](::std::declval<Args>()...); };

// ============================================================================
// Tests — SCL_REFLECT_OPERATOR (friend inline)
// ============================================================================

TEST(ReflectOperator, BinaryAddMutableLvalue)
{
    WrappedTarget w{10};
    auto result = w + 5;
    EXPECT_EQ(result, static_cast<short>(15));
}

TEST(ReflectOperator, BinaryAddConstLvalue)
{
    WrappedTarget const w{10};
    auto result = w + 5;
    EXPECT_EQ(result, 65); // 10 + 5 + 50
}

TEST(ReflectOperator, BinaryAddConstLvalueConstexpr)
{
    constexpr WrappedTarget w{10};
    STATIC_EXPECT_EQ(w + 5, 65);
}

TEST(ReflectOperator, PrefixNeg)
{
    WrappedTarget const w{7};
    EXPECT_EQ(-w, -7);
}

TEST(ReflectOperator, PrefixNegConstexpr)
{
    constexpr WrappedTarget w{7};
    STATIC_EXPECT_EQ(-w, -7);
}

TEST(ReflectOperator, PrefixIncrement)
{
    WrappedTarget w{5};
    auto result = ++w;
    EXPECT_EQ(result, 6L);
    // Underlying value changed — verify via operator+
    EXPECT_EQ(w + 0, static_cast<short>(6));
}

TEST(ReflectOperator, PostfixIncrement)
{
    WrappedTarget w{5};
    auto old_val = w++;
    EXPECT_EQ(old_val, static_cast<short>(5));
    // Underlying value incremented
    EXPECT_EQ(w + 0, static_cast<short>(6));
}

// ============================================================================
// Tests — negative: mutable-only operators not available on const
// ============================================================================

TEST(ReflectOperator, PrefixIncNotAvailableOnConst)
{
    STATIC_EXPECT_FALSE(can_call_preinc_v<WrappedTarget const &>);
}

TEST(ReflectOperator, PostfixIncNotAvailableOnConst)
{
    STATIC_EXPECT_FALSE(can_call_postinc_v<WrappedTarget const &>);
}

TEST(ReflectOperator, BinaryAddRvalueCallsConstLvalueOverload)
{
    // Target has operator+(int) const & — rvalue binds to const& so call is valid
    STATIC_EXPECT_TRUE((can_call_add_v<WrappedTarget &&, int>));
    STATIC_EXPECT_TRUE((::std::same_as<decltype(::std::declval<WrappedTarget &&>() + 5), int>));
}

// ============================================================================
// Tests — SCL_REFLECT_BINARY_OPERATOR (member function)
// ============================================================================

TEST(ReflectMemberOperator, AssignmentChangesValue)
{
    WrappedTarget w{0};
    auto result = (w = 42);
    EXPECT_EQ(result, 42L);
    EXPECT_EQ(w + 0, static_cast<short>(42));
}

TEST(ReflectMemberOperator, SubscriptMutableLvalue)
{
    WrappedTarget w{10};
    EXPECT_EQ(w[3], static_cast<short>(13));
}

TEST(ReflectMemberOperator, SubscriptConstLvalue)
{
    WrappedTarget const w{10};
    EXPECT_EQ(w[3], 63); // 10 + 3 + 50
}

TEST(ReflectMemberOperator, SubscriptConstLvalueConstexpr)
{
    constexpr WrappedTarget w{10};
    STATIC_EXPECT_EQ(w[3], 63);
}

TEST(ReflectMemberOperator, SubscriptRvalueCallsConstLvalueOverload)
{
    // Target has operator[](int) const & — rvalue binds to const& so call is valid
    STATIC_EXPECT_TRUE((can_call_subscript_v<WrappedTarget &&, int>));
    STATIC_EXPECT_TRUE((::std::same_as<decltype(::std::declval<WrappedTarget &&>()[0]), int>));
}

TEST(ReflectMemberOperator, CallOperatorMutableLvalue)
{
    WrappedTarget w{7};
    EXPECT_EQ(w(), 7L);
}

TEST(ReflectMemberOperator, CallOperatorConstLvalue)
{
    WrappedTarget const w{7};
    EXPECT_EQ(w(3), 60); // 7 + 3 + 50
}

TEST(ReflectMemberOperator, CallOperatorConstLvalueConstexpr)
{
    constexpr WrappedTarget w{7};
    STATIC_EXPECT_EQ(w(3), 60);
}

// ============================================================================
// Tests — executor override for friend operator
// ============================================================================

TEST(ReflectOperatorExecutorOverride, MutableLValueCallsOverride)
{
    OverridingWrapped w{5};
    auto result = w + 3;

    static_assert(::std::same_as<decltype(result), long>,
        "mutable-lvalue operator+ must return long — the override return type");

    EXPECT_EQ(result, (5 + 3) * 100L);
}

TEST(ReflectOperatorExecutorOverride, ConstLValueFallsToExecutePath)
{
    OverridingWrapped const w{5};
    auto result = w + 3;

    static_assert(::std::same_as<decltype(result), int>,
        "const-lvalue operator+ must return int — execute path, Target::operator+(int) const&");

    EXPECT_EQ(result, 58); // 5 + 3 + 50
}

TEST(ReflectOperatorExecutorOverride, ReturnTypeIsOverrideNotTarget)
{
    OverridingWrapped w{0};
    STATIC_EXPECT_FALSE((::std::same_as<decltype(w + 1), short>));
    STATIC_EXPECT_TRUE((::std::same_as<decltype(w + 1), long>));
}

TEST(ReflectOperatorExecutorOverride, ConstAndRValueUnaffectedByMutableOverride)
{
    OverridingWrapped const cw{0};
    // const& has no override → execute path → int
    STATIC_EXPECT_TRUE((::std::same_as<decltype(cw + 1), int>));
}

// ============================================================================
// Tests — noexcept propagation
// ============================================================================

TEST(ReflectOperator, NoexceptPropagatesFromExecutorOverrideNoexceptOp)
{
    // NxExecutor::operator_nx_add(NxExecutor const &, int) is noexcept
    // → override path is selected → reflected operator+ is noexcept
    WrappedNx const w{1};
    STATIC_EXPECT_TRUE(noexcept(w + 1));
}

TEST(ReflectOperator, NoexceptNotPropagatedWhenNoExecutorOverride)
{
    // No executor override for nx_sub → execute path
    // NxExecutor::execute is not noexcept → reflected operator- is not noexcept
    WrappedNx const w{1};
    STATIC_EXPECT_FALSE(noexcept(w - 1));
}
