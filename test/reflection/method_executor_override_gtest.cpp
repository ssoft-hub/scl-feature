#include <gtest_utils.h>

#include <scl/feature/reflection/method.h>
#include <scl/feature/type_traits/executor.h>
#include <scl/utility/type_traits/forward_like.h>

#include <type_traits>
#include <utility>

// ============================================================================
// Shared target type
// ============================================================================

namespace
{
    /// Target with distinct return types per qualifier (required by SCL_REFLECT_METHOD).
    struct Target
    {
        int value = 0;

        constexpr short get() & { return static_cast<short>(value); }
        constexpr int get() const & { return value + 50; }
        constexpr float get() && { return static_cast<float>(value) + 100.0f; }
        constexpr double get() const && { return static_cast<double>(value) + 200.0; }

        constexpr void set(short v) & { value = v; }
    };
} // namespace

// ============================================================================
// Fixture: executor with method_get override only for mutable lvalue
//
// method_get(Executor &) returns long(value * 2) — distinct from Target::get() &
// which returns short(value).  Absence of method_get(const &) and method_get(&&)
// ensures those qualifiers fall through to the execute path.
// ============================================================================

struct OverridingExecutor
{
    Target m_value;

    // Executor override: called instead of execute() for mutable-lvalue wrapper.
    // Returns long to distinguish from Target::get() & which returns short.
    static constexpr long method_get(OverridingExecutor & self)
    {
        return static_cast<long>(self.m_value.value) * 2;
    }

    // Executor override for set(): multiplies by 3 to distinguish from normal set().
    static constexpr void method_set(OverridingExecutor & self, short v)
    {
        self.m_value.value = static_cast<int>(v) * 3;
    }

    template <typename Self, typename Func, typename... Args>
    static constexpr auto execute(Self &&, Func && func, Args &&... args)
    {
        return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
    }

    template <typename Self>
    static constexpr decltype(auto) access(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_value);
    }
};

struct OverridingReflected;

template <>
struct scl::feature::executor_trait<OverridingReflected>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_exec);
    }
};

struct OverridingReflected
{
    OverridingExecutor m_exec;

    SCL_REFLECT_TYPE(OverridingReflected, OverridingExecutor);

    constexpr explicit OverridingReflected(int v)
        : m_exec{Target{v}}
    {}

    SCL_REFLECT_METHOD(get)
    SCL_REFLECT_METHOD(set)
};

// ============================================================================
// Tests — mutable lvalue: executor override is selected
// ============================================================================

// method_get(OverridingExecutor &) has return type long, not short.
// The override must be selected for mutable-lvalue wrappers.

TEST(ReflectMethodExecutorOverride, MutableLValueCallsOverride)
{
    OverridingReflected r{5};
    auto result = r.get();

    static_assert(::std::same_as<decltype(result), long>,
        "reflected get() & must return long — the return type of method_get(Executor &)");

    EXPECT_EQ(result, 10L); // 5 * 2
}

TEST(ReflectMethodExecutorOverride, MutableLValueSetCallsOverride)
{
    OverridingReflected r{0};
    r.set(short{7});

    // method_set multiplies by 3, not the normal assignment
    EXPECT_EQ(r.m_exec.m_value.value, 21); // 7 * 3
}

// ============================================================================
// Tests — const lvalue: no const& override → execute path (returns int)
// ============================================================================

TEST(ReflectMethodExecutorOverride, ConstLValueFallsToExecutePath)
{
    OverridingReflected const r{5};
    auto result = r.get();

    static_assert(::std::same_as<decltype(result), int>,
        "reflected get() const& must return int — execute path, Target::get() const&");

    EXPECT_EQ(result, 55); // value + 50
}

// ============================================================================
// Tests — rvalue: no && override → execute path (returns float)
// ============================================================================

TEST(ReflectMethodExecutorOverride, RValueFallsToExecutePath)
{
    static_assert(::std::same_as<decltype(OverridingReflected{5}.get()), float>,
        "reflected get() && must return float — execute path, Target::get() &&");

    auto result = OverridingReflected{5}.get();
    EXPECT_FLOAT_EQ(result, 105.0f); // value + 100
}

// ============================================================================
// Tests — type checks
// ============================================================================

TEST(ReflectMethodExecutorOverride, ReturnTypeIsOverrideNotTargetType)
{
    OverridingReflected r{0};

    // Mutable lvalue: override returns long, not short (Target::get() &)
    STATIC_EXPECT_FALSE((::std::same_as<decltype(r.get()), short>));
    STATIC_EXPECT_TRUE((::std::same_as<decltype(r.get()), long>));
}

TEST(ReflectMethodExecutorOverride, ConstAndRValueUnaffectedByMutableOverride)
{
    OverridingReflected const cr{0};

    // const& has no override → execute path → int
    STATIC_EXPECT_TRUE((::std::same_as<decltype(cr.get()), int>));

    // && has no override → execute path → float
    STATIC_EXPECT_TRUE((::std::same_as<decltype(OverridingReflected{0}.get()), float>));
}

// ============================================================================
// Fixture: noexcept propagation
//
// NoexceptReflected has a noexcept method_get override for mutable lvalue.
// NoThrowExecutor::execute is not noexcept, so const& (execute path) is not.
// ============================================================================

namespace
{
    struct NoexceptOverrideExecutor
    {
        Target m_value;

        // Noexcept override for mutable lvalue only.
        static constexpr long method_get(NoexceptOverrideExecutor & self) noexcept
        {
            return static_cast<long>(self.m_value.value);
        }

        template <typename Self, typename Func, typename... Args>
        static constexpr auto execute(Self &&, Func && func, Args &&... args)
        {
            return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
        }

        template <typename Self>
        static constexpr decltype(auto) access(Self && self)
        {
            return ::scl::forward_like<Self>(self.m_value);
        }
    };
} // namespace

struct NoexceptReflected;

template <>
struct scl::feature::executor_trait<NoexceptReflected>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_exec);
    }
};

struct NoexceptReflected
{
    NoexceptOverrideExecutor m_exec;

    SCL_REFLECT_TYPE(NoexceptReflected, NoexceptOverrideExecutor);

    constexpr explicit NoexceptReflected(int v)
        : m_exec{Target{v}}
    {}

    SCL_REFLECT_METHOD(get)
};

// ============================================================================
// Tests — noexcept propagation
// ============================================================================

TEST(ReflectMethodExecutorOverride, NoexceptOverrideIsNoexcept)
{
    // method_get(NoexceptOverrideExecutor&) is noexcept — reflected overload
    // must propagate this.
    NoexceptReflected r{0};
    STATIC_EXPECT_TRUE(noexcept(r.get()));
}

TEST(ReflectMethodExecutorOverride, NonNoexceptOverrideIsNotNoexcept)
{
    // method_get(OverridingExecutor&) is not noexcept — reflected overload
    // must not be noexcept either.
    OverridingReflected r{0};
    STATIC_EXPECT_FALSE(noexcept(r.get()));
}

TEST(ReflectMethodExecutorOverride, ExecutePathIsNotNoexcept)
{
    // const& has no override → execute path; execute is not noexcept.
    NoexceptReflected const r{0};
    STATIC_EXPECT_FALSE(noexcept(r.get()));
}
