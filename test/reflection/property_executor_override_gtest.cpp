#include <gtest_utils.h>

#include <scl/feature/reflection/property.h>
#include <scl/feature/type_traits/executor.h>
#include <scl/feature/wrapper_cast.h>
#include <scl/utility/type_traits/forward_like.h>

#include <type_traits>
#include <utility>

// ============================================================================
// Tests for SCL_REFLECT_PROPERTY executor override (property_<name>)
// ============================================================================

using namespace ::scl;
using namespace ::scl::feature;

// ── Fixtures ─────────────────────────────────────────────────────────────────

namespace
{
    struct Point
    {
        int x = 0;
        int y = 0;
    };
} // namespace

// ── Overriding executor ───────────────────────────────────────────────────────
//
// property_x(Executor &) redirects mutable-lvalue access to m_override_x, a
// separate storage field.  This makes the override detectable by value: reading
// m_override_x returns a different number than reading m_value.x.
//
// No property_x(const Executor &) override — const access falls through to the
// execute path and reads m_value.x.
// No property_y override — both qualifications use the execute path.

struct OverridingExecutor
{
    Point m_value;
    int m_override_x = 0;   ///< Separate storage used by the property_x override.
    int m_call_count_x = 0; ///< Incremented each time property_x(Executor&) is invoked.

    // Executor override: mutable lvalue only, NOT noexcept.
    static int & property_x(OverridingExecutor & self)
    {
        ++self.m_call_count_x;
        return self.m_override_x;
    }

    template <typename Self, typename Func, typename... Args>
    static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args)
        noexcept(::std::is_nothrow_invocable_v<Func &&, Args &&...>)
        requires ::std::same_as<::std::remove_cvref_t<Self>, OverridingExecutor>
    {
        return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
    }

    template <typename Self>
    static constexpr decltype(auto) value(Self && self) noexcept
        requires ::std::same_as<::std::remove_cvref_t<Self>, OverridingExecutor>
    {
        return ::scl::forward_like<Self>(self.m_value);
    }
};

struct OverridingReflectedPoint;

template <>
struct scl::feature::executor_trait<OverridingReflectedPoint>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_impl);
    }
};

struct OverridingReflectedPoint
{
    OverridingExecutor m_impl;

    SCL_REFLECT_TYPE(OverridingReflectedPoint, OverridingExecutor);

    explicit OverridingReflectedPoint(int vx, int vy, int ox = 0)
        : m_impl{Point{vx, vy}, ox}
    {}

    SCL_REFLECT_PROPERTY(x)
    SCL_REFLECT_PROPERTY(y)
};

// ── Tests — mutable lvalue: override is selected ──────────────────────────────

// property_x(Executor &) reads m_override_x, NOT m_value.x.
// Distinct initial values prove which path was taken.

TEST(ReflectPropertyExecutorOverride, MutableLValueXUsesOverride)
{
    // m_value.x = 10, m_override_x = 99
    OverridingReflectedPoint p{10, 20, 99};
    int val = wrapper_cast(p.x);
    EXPECT_EQ(val, 99); // override, not m_value.x (10)
}

TEST(ReflectPropertyExecutorOverride, MutableLValueYUsesExecutePath)
{
    // No property_y override — must read m_value.y via execute path.
    OverridingReflectedPoint p{10, 20, 99};
    int val = wrapper_cast(p.y);
    EXPECT_EQ(val, 20); // m_value.y, execute path
}

// ── Tests — const lvalue: no const& override → execute path ──────────────────

TEST(ReflectPropertyExecutorOverride, ConstLValueXUsesExecutePath)
{
    // No property_x(const Executor &) → reads m_value.x via execute path.
    OverridingReflectedPoint const p{10, 20, 99};
    int val = wrapper_cast(p.x);
    EXPECT_EQ(val, 10); // m_value.x, execute path
}

// ── Tests — write through override ───────────────────────────────────────────

TEST(ReflectPropertyExecutorOverride, WriteThroughOverrideUpdatesOverrideStorage)
{
    OverridingReflectedPoint p{10, 20, 0};
    wrapper_cast(p.x).to<int &>() = 42; // mutable-lvalue → override → m_override_x
    EXPECT_EQ(p.m_impl.m_override_x, 42);
    EXPECT_EQ(p.m_impl.m_value.x, 10); // m_value.x unaffected
}

TEST(ReflectPropertyExecutorOverride, WriteThroughExecutePathUpdatesWrappedValue)
{
    OverridingReflectedPoint p{10, 20, 0};
    wrapper_cast(p.y).to<int &>() = 55; // no override → execute path → m_value.y
    EXPECT_EQ(p.m_impl.m_value.y, 55);
}

// ── Tests — override is actually called ──────────────────────────────────────

TEST(ReflectPropertyExecutorOverride, MutableLValueCallsPropertyXExactlyOnce)
{
    OverridingReflectedPoint p{10, 20, 99};
    EXPECT_EQ(p.m_impl.m_call_count_x, 0);
    [[maybe_unused]]
    int val = wrapper_cast(p.x);
    EXPECT_EQ(p.m_impl.m_call_count_x, 1);
}

TEST(ReflectPropertyExecutorOverride, ConstLValueDoesNotCallPropertyXOverride)
{
    // No const& override → execute path, property_x(Executor&) never invoked.
    OverridingReflectedPoint const p{10, 20, 99};
    [[maybe_unused]]
    int val = wrapper_cast(p.x);
    EXPECT_EQ(p.m_impl.m_call_count_x, 0);
}

// ── Tests — override detection ────────────────────────────────────────────────

TEST(ReflectPropertyExecutorOverride, OverrideDetectedForMutableLValue)
{
    // has_override<ScLExec &> must be true for x, false for y and rvalue.
    using Tag_x = OverridingReflectedPoint::property_x_scl_tag;
    using Tag_y = OverridingReflectedPoint::property_y_scl_tag;

    STATIC_EXPECT_TRUE(Tag_x::has_override<OverridingExecutor &>);
    STATIC_EXPECT_FALSE(Tag_x::has_override<OverridingExecutor const &>);
    STATIC_EXPECT_FALSE(Tag_x::has_override<OverridingExecutor &&>);
    STATIC_EXPECT_FALSE(Tag_y::has_override<OverridingExecutor &>);
}

// ── noexcept propagation fixture ─────────────────────────────────────────────

namespace
{
    struct NoexceptOverrideExecutor
    {
        Point m_value;

        // Noexcept override for mutable lvalue only.
        static int & property_x(NoexceptOverrideExecutor & self) noexcept { return self.m_value.x; }

        template <typename Self, typename Func, typename... Args>
        static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args)
            requires ::std::same_as<::std::remove_cvref_t<Self>, NoexceptOverrideExecutor>
        {
            return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
        }

        template <typename Self>
        static constexpr decltype(auto) value(Self && self) noexcept
            requires ::std::same_as<::std::remove_cvref_t<Self>, NoexceptOverrideExecutor>
        {
            return ::scl::forward_like<Self>(self.m_value);
        }
    };
} // namespace

struct NoexceptReflectedPoint;

template <>
struct scl::feature::executor_trait<NoexceptReflectedPoint>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_impl);
    }
};

struct NoexceptReflectedPoint
{
    NoexceptOverrideExecutor m_impl;

    SCL_REFLECT_TYPE(NoexceptReflectedPoint, NoexceptOverrideExecutor);

    explicit NoexceptReflectedPoint(int x, int y)
        : m_impl{Point{x, y}}
    {}

    SCL_REFLECT_PROPERTY(x)
    SCL_REFLECT_PROPERTY(y)
};

// ── Tests — noexcept propagation ─────────────────────────────────────────────
//
// noexcept is tested at the holder::value() level, which is where the
// property_execution dispatch lives.  The wrapper_cast machinery sits on top
// and has its own (unconditional) noexcept on wrapper_caster construction, so
// testing through wrapper_cast would mask the underlying propagation.

TEST(ReflectPropertyExecutorOverride, NoexceptOverrideIsNoexcept)
{
    // property_x(NoexceptOverrideExecutor &) is noexcept → holder::value(Holder&) noexcept.
    NoexceptReflectedPoint p{1, 2};
    using Holder = ::std::remove_reference_t<decltype(p.x)>::executor_type;
    STATIC_EXPECT_TRUE(noexcept(Holder::value(::std::declval<Holder &>())));
}

TEST(ReflectPropertyExecutorOverride, NonNoexceptOverrideIsNotNoexcept)
{
    // property_x(OverridingExecutor &) is NOT noexcept → holder::value(Holder&) not noexcept.
    OverridingReflectedPoint p{0, 0};
    using Holder = ::std::remove_reference_t<decltype(p.x)>::executor_type;
    STATIC_EXPECT_FALSE(noexcept(Holder::value(::std::declval<Holder &>())));
}

TEST(ReflectPropertyExecutorOverride, ExecutePathIsNotNoexcept)
{
    // No const& override for x → execute path; execute is not noexcept.
    // Const propagates: const wrapper → Holder const& in value().
    NoexceptReflectedPoint const p{1, 2};
    using Holder = ::std::remove_reference_t<decltype(p.x)>::executor_type;
    STATIC_EXPECT_FALSE(noexcept(Holder::value(::std::declval<Holder const &>())));
}

TEST(ReflectPropertyExecutorOverride, NoOverrideYIsNotNoexcept)
{
    // No property_y override at all → execute path for both qualifications.
    NoexceptReflectedPoint p{1, 2};
    using Holder = ::std::remove_reference_t<decltype(p.y)>::executor_type;
    STATIC_EXPECT_FALSE(noexcept(Holder::value(::std::declval<Holder &>())));
}
