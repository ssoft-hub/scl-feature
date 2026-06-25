#include <gtest_utils.h>

#include <scl/feature/reflection/property.h>
#include <scl/feature/type_traits/executor.h>
#include <scl/feature/wrapper_cast.h>
#include <scl/utility/type_traits/forward_like.h>

#include <type_traits>
#include <utility>

// ============================================================================
// Tests for SCL_REFLECT_PROPERTY
// ============================================================================

using namespace ::scl;
using namespace ::scl::feature;

// ── Fixtures ─────────────────────────────────────────────────────────────────

struct Point
{
    int x = 0;
    int y = 0;
};

struct PointExecutor
{
    Point m_value;

    template <typename Self, typename Func, typename... Args>
    static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args)
        noexcept(::std::is_nothrow_invocable_v<Func &&, Args &&...>)
        requires ::std::same_as<::std::remove_cvref_t<Self>, PointExecutor>
    {
        return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
    }

    template <typename Self>
    static constexpr decltype(auto) access(Self && self) noexcept
        requires ::std::same_as<::std::remove_cvref_t<Self>, PointExecutor>
    {
        return ::scl::forward_like<Self>(self.m_value);
    }
};

struct ReflectedPoint;

template <>
struct scl::feature::executor_trait<ReflectedPoint>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_impl);
    }
};

struct ReflectedPoint
{
    PointExecutor m_impl;

    SCL_REFLECT_TYPE(ReflectedPoint, PointExecutor);

    explicit ReflectedPoint(int x, int y)
        : m_impl{Point{x, y}}
    {}

    SCL_REFLECT_PROPERTY(x)
    SCL_REFLECT_PROPERTY(y)
};

// ── Read ──────────────────────────────────────────────────────────────────────

TEST(ReflectProperty, ReadXFromMutableLValue)
{
    ReflectedPoint p{3, 7};
    int val = wrapper_cast(p.x); // implicit conversion via operator int&()
    EXPECT_EQ(val, 3);
}

TEST(ReflectProperty, ReadYFromMutableLValue)
{
    ReflectedPoint p{3, 7};
    int val = wrapper_cast(p.y);
    EXPECT_EQ(val, 7);
}

TEST(ReflectProperty, ReadXFromConstLValue)
{
    ReflectedPoint const p{5, 9};
    int val = wrapper_cast(p.x); // operator int const&() const
    EXPECT_EQ(val, 5);
}

// ── Write ─────────────────────────────────────────────────────────────────────
//
// The reflected operator=(int) is not generated for fundamental types because
// the framework checks `value.operator=(x)` which fails for int.  Write access
// uses wrapper_cast(...).to<int &>() to obtain the stored reference and then
// assigns via built-in int& = int.

TEST(ReflectProperty, WriteXMutatesWrapped)
{
    ReflectedPoint p{0, 0};
    wrapper_cast(p.x).to<int &>() = 42;
    EXPECT_EQ(static_cast<int>(wrapper_cast(p.x)), 42);
    EXPECT_EQ(static_cast<int>(wrapper_cast(p.y)), 0); // y unaffected
}

TEST(ReflectProperty, WriteYMutatesWrapped)
{
    ReflectedPoint p{1, 2};
    wrapper_cast(p.y).to<int &>() = 99;
    EXPECT_EQ(static_cast<int>(wrapper_cast(p.x)), 1);
    EXPECT_EQ(static_cast<int>(wrapper_cast(p.y)), 99);
}

// ── Copy correctness ──────────────────────────────────────────────────────────

TEST(ReflectProperty, CopyPreservesIndependentValues)
{
    ReflectedPoint a{1, 2};
    ReflectedPoint b = a; // memberwise copy — executor offset must re-seat correctly

    EXPECT_EQ(static_cast<int>(wrapper_cast(a.x)), 1);
    EXPECT_EQ(static_cast<int>(wrapper_cast(b.x)), 1);

    wrapper_cast(a.x).to<int &>() = 99;
    EXPECT_EQ(static_cast<int>(wrapper_cast(a.x)), 99);
    EXPECT_EQ(static_cast<int>(wrapper_cast(b.x)), 1); // b must be independent
}

TEST(ReflectProperty, MovePreservesCorrectness)
{
    ReflectedPoint a{7, 8};
    ReflectedPoint b = ::std::move(a);
    EXPECT_EQ(static_cast<int>(wrapper_cast(b.x)), 7);
    EXPECT_EQ(static_cast<int>(wrapper_cast(b.y)), 8);
}

// ── Rvalue access ───────────────────────────────────────────────────────────
//
// Reading through an rvalue wrapper exercises the rvalue cv-ref branch of
// holder::access(): exec_refer becomes OuterExecutor&&, so outer_executor() must
// re-seat the offset and propagate the rvalue qualification correctly.

TEST(ReflectProperty, ReadXFromRValue)
{
    ReflectedPoint p{4, 6};
    int val = wrapper_cast(::std::move(p).x);
    EXPECT_EQ(val, 4);
}

// ── Class-type field ──────────────────────────────────────────────────────────
//
// The reflected field is a class type, not a fundamental.  Verifies the member-pointer
// projection works for non-fundamental fields, for both read and write through
// the obtained reference.

namespace
{
    struct Inner
    {
        int v = 0;
    };

    struct Bag
    {
        Inner inner;
    };

    struct BagExecutor
    {
        Bag m_value;

        template <typename Self, typename Func, typename... Args>
        static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args)
            noexcept(::std::is_nothrow_invocable_v<Func &&, Args &&...>)
            requires ::std::same_as<::std::remove_cvref_t<Self>, BagExecutor>
        {
            return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
        }

        template <typename Self>
        static constexpr decltype(auto) access(Self && self) noexcept
            requires ::std::same_as<::std::remove_cvref_t<Self>, BagExecutor>
        {
            return ::scl::forward_like<Self>(self.m_value);
        }
    };
} // namespace

struct ReflectedBag;

template <>
struct scl::feature::executor_trait<ReflectedBag>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_impl);
    }
};

struct ReflectedBag
{
    BagExecutor m_impl;

    SCL_REFLECT_TYPE(ReflectedBag, BagExecutor);

    explicit ReflectedBag(int v)
        : m_impl{Bag{Inner{v}}}
    {}

    SCL_REFLECT_PROPERTY(inner)
};

TEST(ReflectProperty, ReadClassTypeFieldReturnsWrappedReference)
{
    ReflectedBag b{11};
    EXPECT_EQ(wrapper_cast(b.inner).to<Inner &>().v, 11);
}

TEST(ReflectProperty, WriteClassTypeFieldMutatesWrapped)
{
    ReflectedBag b{0};
    wrapper_cast(b.inner).to<Inner &>().v = 77;
    EXPECT_EQ(b.m_impl.m_value.inner.v, 77);
}

// ── Executor concept: execute / guard forwarding ──────────────────────────────
//
// holder is itself an executor.  These checks pin the executor-concept surface
// of the generated property holder without reaching into its private state:
//   * execute() is well-formed (forwards arbitrary work to the outer executor);
//   * guard()/unguard() are present iff the outer executor provides them.

namespace
{
    using PlainHolder = ::std::remove_reference_t<decltype(::std::declval<ReflectedPoint &>().x)>::executor_type;

    struct GuardingExecutor
    {
        Point m_value;

        template <typename Self>
        static void guard(Self && self) noexcept
            requires ::std::same_as<::std::remove_cvref_t<Self>, GuardingExecutor>
        {}

        template <typename Self>
        static void unguard(Self && self) noexcept
            requires ::std::same_as<::std::remove_cvref_t<Self>, GuardingExecutor>
        {}

        template <typename Self, typename Func, typename... Args>
        static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args)
            requires ::std::same_as<::std::remove_cvref_t<Self>, GuardingExecutor>
        {
            return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
        }

        template <typename Self>
        static constexpr decltype(auto) access(Self && self) noexcept
            requires ::std::same_as<::std::remove_cvref_t<Self>, GuardingExecutor>
        {
            return ::scl::forward_like<Self>(self.m_value);
        }
    };
} // namespace

struct ReflectedGuarded;

template <>
struct scl::feature::executor_trait<ReflectedGuarded>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_impl);
    }
};

struct ReflectedGuarded
{
    GuardingExecutor m_impl;

    SCL_REFLECT_TYPE(ReflectedGuarded, GuardingExecutor);

    explicit ReflectedGuarded(int x, int y)
        : m_impl{Point{x, y}}
    {}

    SCL_REFLECT_PROPERTY(x)
};

namespace
{
    using GuardedHolder = ::std::remove_reference_t<decltype(::std::declval<ReflectedGuarded &>().x)>::executor_type;

    // Named functor, not a lambda: a lambda inside a requires-expression nested in
    // the STATIC_EXPECT_* macro triggers an internal compiler error on GCC 13.
    struct returns_int
    {
        int operator()() const noexcept { return 0; }
    };

    constexpr bool plain_holder_execute_well_formed =
        requires(PlainHolder & h) { PlainHolder::execute(h, returns_int{}); };

    // Guard presence is detected with the library's own trait (function-pointer
    // cast).  A hand-rolled `requires { Holder::guard(h); }` would instead make
    // GCC hard-error on the constrained-out overload rather than yield false.
    constexpr bool guarded_holder_has_guard = has_guard_v<GuardedHolder, GuardedHolder &>;
    constexpr bool guarded_holder_has_unguard = has_unguard_v<GuardedHolder, GuardedHolder &>;
    constexpr bool plain_holder_has_guard = has_guard_v<PlainHolder, PlainHolder &>;
    constexpr bool plain_holder_has_unguard = has_unguard_v<PlainHolder, PlainHolder &>;
} // namespace

TEST(ReflectProperty, HolderExecuteForwardsToOuterExecutor)
{
    STATIC_EXPECT_TRUE(plain_holder_execute_well_formed);
}

TEST(ReflectProperty, GuardPresentWhenOuterExecutorProvidesIt)
{
    STATIC_EXPECT_TRUE(guarded_holder_has_guard);
    STATIC_EXPECT_TRUE(guarded_holder_has_unguard);
}

TEST(ReflectProperty, GuardAbsentWhenOuterExecutorLacksIt)
{
    STATIC_EXPECT_FALSE(plain_holder_has_guard);
    STATIC_EXPECT_FALSE(plain_holder_has_unguard);
}
