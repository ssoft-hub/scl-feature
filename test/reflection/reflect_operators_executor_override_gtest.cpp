#include <gtest_utils.h>

#include <scl/feature/reflection/reflect.h>
#include <scl/feature/type_traits/executor.h>
#include <scl/utility/type_traits/forward_like.h>

#include <utility>

// ============================================================================
// OvTarget — wrapped type used to verify which execution path is taken.
//
// operator+ has two overloads with distinct return types so that
// SCL_HAS_QUALIFIED_METHOD can discriminate qualifiers:
//   &       → short
//   const & → int
//
// All other operators have a single qualifier — the one that makes sense for
// the operation — so no return-type discrimination is required.
// ============================================================================

struct OvTarget
{
    int value = 0;

    // operator+ — two overloads: & returns short, const& returns int.
    // Needed so SCL_HAS_QUALIFIED_METHOD sees a dedicated & overload.
    constexpr short operator+(int rhs) & noexcept { return static_cast<short>(value + rhs); }
    constexpr int operator+(int rhs) const & noexcept { return value + rhs + 50; }

    // Unary minus — const& only.  No executor override in OvExecutor → execute path.
    constexpr int operator-() const & noexcept { return -value; }

    // Prefix / postfix ++ — & only.  OvExecutor overrides both.
    constexpr int operator++() & noexcept { return ++value; }
    constexpr int operator++(int) & noexcept { return value++; }

    // Assignment — & only.  OvExecutor overrides it.
    constexpr int operator=(int rhs) & noexcept
    {
        value = rhs;
        return value;
    }
};

// ============================================================================
// OvExecutor
//
// Provides operator_<name> overrides for a subset of operators.
// Return type is long in every override so tests can distinguish the override
// result (long) from the execute-path result (short / int) by return type alone.
//
// operator_plus:               const& override, noexcept  → value * 1000
// operator_prefix_increment:   & override,       noexcept  → value += 10
// operator_postfix_increment:  & override,       noexcept  → old value, += 10
// operator_assign:             & override,       noexcept  → value = rhs * 3
//
// No operator_unary_minus → -w falls through to the execute path.
// execute itself is NOT noexcept → execute-path operators are not noexcept.
// ============================================================================

struct OvExecutor
{
    OvTarget m_value;

    // --- operator_plus: const& only ---
    static constexpr long operator_plus(OvExecutor const & self, int) noexcept
    {
        return static_cast<long>(self.m_value.value) * 1000;
    }

    // --- operator_prefix_increment: & only ---
    static constexpr long operator_prefix_increment(OvExecutor & self) noexcept
    {
        self.m_value.value += 10;
        return self.m_value.value;
    }

    // --- operator_postfix_increment: & only ---
    static constexpr long operator_postfix_increment(OvExecutor & self) noexcept
    {
        int old = self.m_value.value;
        self.m_value.value += 10;
        return static_cast<long>(old);
    }

    // --- operator_assign: & only ---
    static constexpr long operator_assign(OvExecutor & self, int rhs) noexcept
    {
        self.m_value.value = rhs * 3;
        return self.m_value.value;
    }

    // execute is intentionally NOT noexcept — execute-path operators must not be noexcept.
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

// ============================================================================
// OvWrapper
// ============================================================================

struct OvWrapper;

template <>
struct scl::feature::executor_trait<OvWrapper>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_exec);
    }
};

struct OvWrapper : scl::feature::reflect_operators<OvWrapper, OvExecutor>
{
    using scl::feature::reflect_operators<OvWrapper, OvExecutor>::operator=;

    OvExecutor m_exec;

    constexpr explicit OvWrapper(int v)
        : m_exec{OvTarget{v}}
    {}
};

// ============================================================================
// Tests — operator_plus (const& override)
//
// const wrapper:   override selected  → returns long (value * 1000)
// mutable wrapper: no override (first-param type mismatch) → execute path
//                  → Target::operator+(int) & → returns short (value + rhs)
// ============================================================================

TEST(ReflectOpsExecutorOverride, PlusConstCallsOverride)
{
    OvWrapper const w{5};
    auto result = w + 3;

    static_assert(::std::same_as<decltype(result), long>,
        "const& operator+ must return long — the operator_plus override return type");

    EXPECT_EQ(result, 5000L); // 5 * 1000
}

TEST(ReflectOpsExecutorOverride, PlusMutableUsesExecutePath)
{
    OvWrapper w{5};
    auto result = w + 3;

    static_assert(::std::same_as<decltype(result), short>,
        "mutable& operator+ must return short — execute path, OvTarget::operator+(int) &");

    EXPECT_EQ(result, static_cast<short>(8)); // 5 + 3
}

TEST(ReflectOpsExecutorOverride, PlusReturnTypesDistinct)
{
    OvWrapper w{0};
    OvWrapper const cw{0};

    // Override (const) → long; execute path (mutable) → short
    STATIC_EXPECT_TRUE((::std::same_as<decltype(cw + 1), long>));
    STATIC_EXPECT_TRUE((::std::same_as<decltype(w + 1), short>));
    STATIC_EXPECT_FALSE((::std::same_as<decltype(cw + 1), decltype(w + 1)>));
}

// ============================================================================
// Tests — operator_prefix_increment (& override)
//
// The override adds 10; the native Target::operator++() & adds 1.
// Return type is long from the override (vs int from execute path).
// ============================================================================

TEST(ReflectOpsExecutorOverride, PrefixIncrementCallsOverride)
{
    OvWrapper w{5};
    auto result = ++w;

    static_assert(::std::same_as<decltype(result), long>,
        "prefix operator++ must return long — the operator_prefix_increment override return type");

    EXPECT_EQ(result, 15L);                // += 10 → 15
    EXPECT_EQ(w.m_exec.m_value.value, 15); // side effect: += 10, not += 1
}

// ============================================================================
// Tests — operator_postfix_increment (& override)
//
// The override returns the old value as long and adds 10.
// The native Target::operator++(int) & returns old value as int and adds 1.
// ============================================================================

TEST(ReflectOpsExecutorOverride, PostfixIncrementCallsOverride)
{
    OvWrapper w{5};
    auto result = w++;

    static_assert(::std::same_as<decltype(result), long>,
        "postfix operator++ must return long — the operator_postfix_increment override return type");

    EXPECT_EQ(result, 5L);                 // old value
    EXPECT_EQ(w.m_exec.m_value.value, 15); // side effect: += 10, not += 1
}

// ============================================================================
// Tests — operator_assign (& override)
//
// The override sets value to rhs * 3 and returns long.
// The native Target::operator=(int) & sets value = rhs and returns int.
// ============================================================================

TEST(ReflectOpsExecutorOverride, AssignCallsOverride)
{
    OvWrapper w{0};
    auto result = (w = 7);

    static_assert(::std::same_as<decltype(result), long>,
        "operator= must return long — the operator_assign override return type");

    EXPECT_EQ(result, 21L);                // 7 * 3 = 21
    EXPECT_EQ(w.m_exec.m_value.value, 21); // set to rhs * 3, not rhs
}

// ============================================================================
// Tests — unary minus: no override → execute path
//
// OvExecutor has no operator_unary_minus, so -w goes through execute.
// OvTarget::operator-() const & returns int.
// execute is not noexcept → -w must not be noexcept.
// ============================================================================

TEST(ReflectOpsExecutorOverride, UnaryMinusUsesExecutePath)
{
    OvWrapper const w{7};
    auto result = -w;

    static_assert(::std::same_as<decltype(result), int>,
        "operator- must return int — execute path, OvTarget::operator-() const &");

    EXPECT_EQ(result, -7);
}

// ============================================================================
// Tests — noexcept propagation
// ============================================================================

TEST(ReflectOpsExecutorOverride, OverrideNoexceptPropagatedToConst)
{
    // operator_plus(OvExecutor const&, int) is noexcept
    OvWrapper const w{0};
    STATIC_EXPECT_TRUE(noexcept(w + 0));
}

TEST(ReflectOpsExecutorOverride, ExecutePathNotNoexcept)
{
    // No operator_unary_minus override → execute path; execute is not noexcept
    OvWrapper const w{0};
    STATIC_EXPECT_FALSE(noexcept(-w));
}

TEST(ReflectOpsExecutorOverride, PrefixIncrementOverrideNoexcept)
{
    OvWrapper w{0};
    STATIC_EXPECT_TRUE(noexcept(++w));
}

TEST(ReflectOpsExecutorOverride, PostfixIncrementOverrideNoexcept)
{
    OvWrapper w{0};
    STATIC_EXPECT_TRUE(noexcept(w++));
}

TEST(ReflectOpsExecutorOverride, AssignOverrideNoexcept)
{
    OvWrapper w{0};
    STATIC_EXPECT_TRUE(noexcept(w = 0));
}
