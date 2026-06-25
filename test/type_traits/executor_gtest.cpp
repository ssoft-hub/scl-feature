#include <gtest_utils.h>

#include <scl/feature/executor/inplace/plain.h>
#include <scl/feature/executor/inplace/uninitialized.h>
#include <scl/feature/type_traits/executor.h>
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

// Executor with noexcept guard and unguard.
struct NoexceptGuardExecutor
{
    template <typename Self>
    static constexpr void guard(Self &&) noexcept
    {}
    template <typename Self>
    static constexpr void unguard(Self &&) noexcept
    {}
};

// Executor with potentially-throwing guard and unguard.
struct ThrowingGuardExecutor
{
    template <typename Self>
    static void guard(Self &&) // not noexcept
    {}
    template <typename Self>
    static void unguard(Self &&) // not noexcept
    {}
};

// Executor with no guard / unguard at all (used by is_guard_noexcept tests).
struct NoGuardExecutor
{
    using value_type = int;

    template <typename Self>
    static constexpr int & value(Self && self) noexcept;
};

// ── Fixtures for has_value / has_execute exact-match tests ────────────────────

// Non-template value: accepts only lvalue ref.
struct LvalueValueExecutor
{
    int m_v{};
    static int & value(LvalueValueExecutor & self) noexcept { return self.m_v; }
};

// Non-template value: accepts only const lvalue ref.
struct ConstRefValueExecutor
{
    int m_v{};
    static int const & value(ConstRefValueExecutor const & self) noexcept { return self.m_v; }
};

// Non-template first parameter, template Func: execute accepts only lvalue ref executor.
struct LvalueExecuteExecutor
{
    template <typename Func>
    static void execute(LvalueExecuteExecutor &, Func && func)
    {
        func();
    }
};

// Non-template first parameter, template Func: execute accepts only const lvalue ref executor.
struct ConstRefExecuteExecutor
{
    template <typename Func>
    static void execute(ConstRefExecuteExecutor const &, Func && func)
    {
        func();
    }
};

// ── Fixtures for has_guard / has_unguard exact-match tests ────────────────────

// Non-template guard/unguard: accepts only lvalue ref.
struct LvalueGuardExecutor
{
    static void guard(LvalueGuardExecutor &) noexcept {}
    static void unguard(LvalueGuardExecutor &) noexcept {}
};

// Non-template guard/unguard: accepts only const lvalue ref.
struct ConstRefGuardExecutor
{
    static void guard(ConstRefGuardExecutor const &) noexcept {}
    static void unguard(ConstRefGuardExecutor const &) noexcept {}
};

// ── has_value_v ───────────────────────────────────────────────────────────────

TEST(HasValue, TemplateValue)
{
    // Template value(Self&&): exact match for all three primary value categories.
    STATIC_EXPECT_TRUE((has_value_v<MinimalExecutor<int>, MinimalExecutor<int> &>));
    STATIC_EXPECT_TRUE((has_value_v<MinimalExecutor<int>, MinimalExecutor<int> &&>));
    STATIC_EXPECT_TRUE((has_value_v<MinimalExecutor<int>, MinimalExecutor<int> const &>));

    STATIC_EXPECT_TRUE((has_value_v<inplace::plain<int>, inplace::plain<int> &>));
    STATIC_EXPECT_TRUE((has_value_v<inplace::plain<int>, inplace::plain<int> &&>));
    STATIC_EXPECT_TRUE((has_value_v<inplace::plain<int>, inplace::plain<int> const &>));
}

TEST(HasValue, NoValue)
{
    STATIC_EXPECT_FALSE((has_value_v<NoInterfaceExecutor, NoInterfaceExecutor &>));
    STATIC_EXPECT_FALSE((has_value_v<NoInterfaceExecutor, NoInterfaceExecutor &&>));
    STATIC_EXPECT_FALSE((has_value_v<NoInterfaceExecutor, NoInterfaceExecutor const &>));
}

TEST(HasValue, ExactLvalueRef)
{
    // Non-template value(T&): exact for T& only.
    STATIC_EXPECT_TRUE((has_value_v<LvalueValueExecutor, LvalueValueExecutor &>));
    STATIC_EXPECT_FALSE((has_value_v<LvalueValueExecutor, LvalueValueExecutor &&>));
    STATIC_EXPECT_FALSE((has_value_v<LvalueValueExecutor, LvalueValueExecutor const &>));
}

TEST(HasValue, ExactConstRef)
{
    // Non-template value(T const&): exact for T const& only.
    // T& and T&& are rejected even though const& would bind them — exact match required.
    STATIC_EXPECT_FALSE((has_value_v<ConstRefValueExecutor, ConstRefValueExecutor &>));
    STATIC_EXPECT_FALSE((has_value_v<ConstRefValueExecutor, ConstRefValueExecutor &&>));
    STATIC_EXPECT_TRUE((has_value_v<ConstRefValueExecutor, ConstRefValueExecutor const &>));
}

TEST(HasValue, HasValueOnly)
{
    // NoExecuteExecutor has value() but not execute() — has_value_v is independent.
    STATIC_EXPECT_TRUE((has_value_v<NoExecuteExecutor, NoExecuteExecutor &>));
    STATIC_EXPECT_FALSE((has_execute_v<NoExecuteExecutor, NoExecuteExecutor &>));
}

// ── has_execute_v ─────────────────────────────────────────────────────────────

TEST(HasExecute, TemplateExecute)
{
    // Template execute(Self&&, Func&&): exact match for all three primary value categories.
    STATIC_EXPECT_TRUE((has_execute_v<MinimalExecutor<int>, MinimalExecutor<int> &>));
    STATIC_EXPECT_TRUE((has_execute_v<MinimalExecutor<int>, MinimalExecutor<int> &&>));
    STATIC_EXPECT_TRUE((has_execute_v<MinimalExecutor<int>, MinimalExecutor<int> const &>));

    STATIC_EXPECT_TRUE((has_execute_v<inplace::plain<int>, inplace::plain<int> &>));
    STATIC_EXPECT_TRUE((has_execute_v<inplace::plain<int>, inplace::plain<int> &&>));
    STATIC_EXPECT_TRUE((has_execute_v<inplace::plain<int>, inplace::plain<int> const &>));
}

TEST(HasExecute, NoExecute)
{
    STATIC_EXPECT_FALSE((has_execute_v<NoInterfaceExecutor, NoInterfaceExecutor &>));
    STATIC_EXPECT_FALSE((has_execute_v<NoInterfaceExecutor, NoInterfaceExecutor &&>));
    STATIC_EXPECT_FALSE((has_execute_v<NoInterfaceExecutor, NoInterfaceExecutor const &>));
    STATIC_EXPECT_FALSE((has_execute_v<NoExecuteExecutor, NoExecuteExecutor &>));
}

TEST(HasExecute, ExactLvalueRef)
{
    // Non-template first param execute(T&, Func&&): exact for T& only.
    STATIC_EXPECT_TRUE((has_execute_v<LvalueExecuteExecutor, LvalueExecuteExecutor &>));
    STATIC_EXPECT_FALSE((has_execute_v<LvalueExecuteExecutor, LvalueExecuteExecutor &&>));
    STATIC_EXPECT_FALSE((has_execute_v<LvalueExecuteExecutor, LvalueExecuteExecutor const &>));
}

TEST(HasExecute, ExactConstRef)
{
    // Non-template first param execute(T const&, Func&&): exact for T const& only.
    STATIC_EXPECT_FALSE((has_execute_v<ConstRefExecuteExecutor, ConstRefExecuteExecutor &>));
    STATIC_EXPECT_FALSE((has_execute_v<ConstRefExecuteExecutor, ConstRefExecuteExecutor &&>));
    STATIC_EXPECT_TRUE((has_execute_v<ConstRefExecuteExecutor, ConstRefExecuteExecutor const &>));
}

// ── has_guard_v ───────────────────────────────────────────────────────────────

TEST(HasGuard, TemplateGuard)
{
    // Template guard(Self&&): exact match for all three primary value categories.
    STATIC_EXPECT_TRUE((has_guard_v<NoexceptGuardExecutor, NoexceptGuardExecutor &>));
    STATIC_EXPECT_TRUE((has_guard_v<NoexceptGuardExecutor, NoexceptGuardExecutor &&>));
    STATIC_EXPECT_TRUE((has_guard_v<NoexceptGuardExecutor, NoexceptGuardExecutor const &>));

    STATIC_EXPECT_TRUE((has_guard_v<ThrowingGuardExecutor, ThrowingGuardExecutor &>));
    STATIC_EXPECT_TRUE((has_guard_v<ThrowingGuardExecutor, ThrowingGuardExecutor &&>));
    STATIC_EXPECT_TRUE((has_guard_v<ThrowingGuardExecutor, ThrowingGuardExecutor const &>));
}

TEST(HasGuard, NoGuard)
{
    STATIC_EXPECT_FALSE((has_guard_v<NoGuardExecutor, NoGuardExecutor &>));
    STATIC_EXPECT_FALSE((has_guard_v<NoGuardExecutor, NoGuardExecutor &&>));
    STATIC_EXPECT_FALSE((has_guard_v<NoGuardExecutor, NoGuardExecutor const &>));
    STATIC_EXPECT_FALSE((has_guard_v<inplace::plain<int>, inplace::plain<int> &>));
}

TEST(HasGuard, ExactLvalueRef)
{
    // Non-template guard(T&): exact for T& only.
    STATIC_EXPECT_TRUE((has_guard_v<LvalueGuardExecutor, LvalueGuardExecutor &>));
    STATIC_EXPECT_FALSE((has_guard_v<LvalueGuardExecutor, LvalueGuardExecutor &&>));
    STATIC_EXPECT_FALSE((has_guard_v<LvalueGuardExecutor, LvalueGuardExecutor const &>));
}

TEST(HasGuard, ExactConstRef)
{
    // Non-template guard(T const&): exact for T const& only.
    // T&& is rejected even though const& binds rvalues — exact match required.
    STATIC_EXPECT_FALSE((has_guard_v<ConstRefGuardExecutor, ConstRefGuardExecutor &>));
    STATIC_EXPECT_FALSE((has_guard_v<ConstRefGuardExecutor, ConstRefGuardExecutor &&>));
    STATIC_EXPECT_TRUE((has_guard_v<ConstRefGuardExecutor, ConstRefGuardExecutor const &>));
}

// ── has_unguard_v ─────────────────────────────────────────────────────────────

TEST(HasUnguard, TemplateUnguard)
{
    STATIC_EXPECT_TRUE((has_unguard_v<NoexceptGuardExecutor, NoexceptGuardExecutor &>));
    STATIC_EXPECT_TRUE((has_unguard_v<NoexceptGuardExecutor, NoexceptGuardExecutor &&>));
    STATIC_EXPECT_TRUE((has_unguard_v<NoexceptGuardExecutor, NoexceptGuardExecutor const &>));

    STATIC_EXPECT_TRUE((has_unguard_v<ThrowingGuardExecutor, ThrowingGuardExecutor &>));
    STATIC_EXPECT_TRUE((has_unguard_v<ThrowingGuardExecutor, ThrowingGuardExecutor &&>));
    STATIC_EXPECT_TRUE((has_unguard_v<ThrowingGuardExecutor, ThrowingGuardExecutor const &>));
}

TEST(HasUnguard, NoUnguard)
{
    STATIC_EXPECT_FALSE((has_unguard_v<NoGuardExecutor, NoGuardExecutor &>));
    STATIC_EXPECT_FALSE((has_unguard_v<NoGuardExecutor, NoGuardExecutor &&>));
    STATIC_EXPECT_FALSE((has_unguard_v<NoGuardExecutor, NoGuardExecutor const &>));
    STATIC_EXPECT_FALSE((has_unguard_v<inplace::plain<int>, inplace::plain<int> &>));
}

TEST(HasUnguard, ExactLvalueRef)
{
    STATIC_EXPECT_TRUE((has_unguard_v<LvalueGuardExecutor, LvalueGuardExecutor &>));
    STATIC_EXPECT_FALSE((has_unguard_v<LvalueGuardExecutor, LvalueGuardExecutor &&>));
    STATIC_EXPECT_FALSE((has_unguard_v<LvalueGuardExecutor, LvalueGuardExecutor const &>));
}

TEST(HasUnguard, ExactConstRef)
{
    STATIC_EXPECT_FALSE((has_unguard_v<ConstRefGuardExecutor, ConstRefGuardExecutor &>));
    STATIC_EXPECT_FALSE((has_unguard_v<ConstRefGuardExecutor, ConstRefGuardExecutor &&>));
    STATIC_EXPECT_TRUE((has_unguard_v<ConstRefGuardExecutor, ConstRefGuardExecutor const &>));
}

// ── is_guard_noexcept ─────────────────────────────────────────────────────────

TEST(IsGuardNoexcept, NoGuardMethod)
{
    // No guard() → considered noexcept (vacuously true)
    STATIC_EXPECT_TRUE((is_guard_noexcept_v<NoGuardExecutor, NoGuardExecutor &>));
    STATIC_EXPECT_TRUE((is_guard_noexcept_v<NoGuardExecutor, NoGuardExecutor &&>));
    STATIC_EXPECT_TRUE((is_guard_noexcept_v<NoGuardExecutor, NoGuardExecutor const &>));
    STATIC_EXPECT_TRUE((is_guard_noexcept_v<inplace::plain<int>, inplace::plain<int> &>));
}

TEST(IsGuardNoexcept, NoexceptGuard)
{
    STATIC_EXPECT_TRUE((is_guard_noexcept_v<NoexceptGuardExecutor, NoexceptGuardExecutor &>));
    STATIC_EXPECT_TRUE((is_guard_noexcept_v<NoexceptGuardExecutor, NoexceptGuardExecutor &&>));
    STATIC_EXPECT_TRUE((is_guard_noexcept_v<NoexceptGuardExecutor, NoexceptGuardExecutor const &>));
}

TEST(IsGuardNoexcept, ThrowingGuard)
{
    STATIC_EXPECT_FALSE((is_guard_noexcept_v<ThrowingGuardExecutor, ThrowingGuardExecutor &>));
    STATIC_EXPECT_FALSE((is_guard_noexcept_v<ThrowingGuardExecutor, ThrowingGuardExecutor &&>));
    STATIC_EXPECT_FALSE((is_guard_noexcept_v<ThrowingGuardExecutor, ThrowingGuardExecutor const &>));
}

// ── is_unguard_noexcept ───────────────────────────────────────────────────────

TEST(IsUnguardNoexcept, NoUnguardMethod)
{
    // No unguard() → considered noexcept (vacuously true)
    STATIC_EXPECT_TRUE((is_unguard_noexcept_v<NoGuardExecutor, NoGuardExecutor &>));
    STATIC_EXPECT_TRUE((is_unguard_noexcept_v<NoGuardExecutor, NoGuardExecutor &&>));
    STATIC_EXPECT_TRUE((is_unguard_noexcept_v<NoGuardExecutor, NoGuardExecutor const &>));
    STATIC_EXPECT_TRUE((is_unguard_noexcept_v<inplace::plain<int>, inplace::plain<int> &>));
}

TEST(IsUnguardNoexcept, NoexceptUnguard)
{
    STATIC_EXPECT_TRUE((is_unguard_noexcept_v<NoexceptGuardExecutor, NoexceptGuardExecutor &>));
    STATIC_EXPECT_TRUE((is_unguard_noexcept_v<NoexceptGuardExecutor, NoexceptGuardExecutor &&>));
    STATIC_EXPECT_TRUE((is_unguard_noexcept_v<NoexceptGuardExecutor, NoexceptGuardExecutor const &>));
}

TEST(IsUnguardNoexcept, ThrowingUnguard)
{
    STATIC_EXPECT_FALSE((is_unguard_noexcept_v<ThrowingGuardExecutor, ThrowingGuardExecutor &>));
    STATIC_EXPECT_FALSE((is_unguard_noexcept_v<ThrowingGuardExecutor, ThrowingGuardExecutor &&>));
    STATIC_EXPECT_FALSE((is_unguard_noexcept_v<ThrowingGuardExecutor, ThrowingGuardExecutor const &>));
}

// ── is_executor_v ─────────────────────────────────────────────────────────────

TEST(IsExecutor, InplaceExecutors)
{
    STATIC_EXPECT_TRUE(is_executor_v<inplace::plain<int>>);
    STATIC_EXPECT_TRUE(is_executor_v<inplace::uninitialized<int>>);
    STATIC_EXPECT_TRUE(is_executor_v<MinimalExecutor<int>>);
}

TEST(IsExecutor, CvStripped)
{
    STATIC_EXPECT_TRUE(is_executor_v<inplace::plain<int> const>);
    STATIC_EXPECT_TRUE(is_executor_v<inplace::plain<int> volatile>);
    STATIC_EXPECT_TRUE(is_executor_v<inplace::plain<int> const volatile>);
}

TEST(IsExecutor, RefStripped)
{
    STATIC_EXPECT_TRUE(is_executor_v<inplace::plain<int> &>);
    STATIC_EXPECT_TRUE(is_executor_v<inplace::plain<int> &&>);
    STATIC_EXPECT_TRUE(is_executor_v<inplace::plain<int> const &>);
}

TEST(IsExecutor, GuardUnguardAloneNotEnough)
{
    // guard/unguard are optional — their presence alone does not satisfy the interface.
    STATIC_EXPECT_FALSE(is_executor_v<NoexceptGuardExecutor>);
    STATIC_EXPECT_FALSE(is_executor_v<ThrowingGuardExecutor>);
}

TEST(IsExecutor, NotExecutor)
{
    STATIC_EXPECT_FALSE(is_executor_v<NoInterfaceExecutor>);
    STATIC_EXPECT_FALSE(is_executor_v<NoExecuteExecutor>);
    STATIC_EXPECT_FALSE(is_executor_v<int>);
    STATIC_EXPECT_FALSE(is_executor_v<void>);
}
