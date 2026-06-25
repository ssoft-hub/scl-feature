#include <gtest_utils.h>

#include <scl/feature/wrapper.h>
#include <scl/feature/wrapper_cast.h>

using namespace ::scl;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

struct guard_counters
{
    int guard_count = 0;
    int unguard_count = 0;
};

template <typename T>
struct counting_executor
{
    using value_type = T;

    explicit constexpr counting_executor(T v, guard_counters & counters)
        : m_value{v}
        , m_counters{counters}
    {}

    template <typename Self, typename Func, typename... Args>
    static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args)
    {
        return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
    }

    template <typename Self>
    static constexpr decltype(auto) access(Self && self)
        requires ::std::same_as<std::remove_cvref_t<Self>, counting_executor>
    {
        return ::scl::forward_like<Self>(self.m_value);
    }

    template <typename Self>
    static constexpr void guard(Self & self)
        requires ::std::same_as<std::remove_cvref_t<Self>, counting_executor>
    {
        ++self.m_counters.guard_count;
    }

    template <typename Self>
    static constexpr void unguard(Self & self)
        requires ::std::same_as<std::remove_cvref_t<Self>, counting_executor>
    {
        ++self.m_counters.unguard_count;
    }

    T m_value;
    guard_counters & m_counters;
};

using inner_w = wrapper<int, counting_executor>;
using outer_w = wrapper<inner_w, feature::inplace::plain>;

// ---------------------------------------------------------------------------
// Pass-through for non-wrapper types
// ---------------------------------------------------------------------------

TEST(WrapperCastPassThrough, ReturnsRefUnchanged)
{
    int x = 42;
    int & r = wrapper_cast(x);
    EXPECT_EQ(&r, &x);
}

TEST(WrapperCastPassThrough, ConstRef)
{
    int const x = 7;
    int const & r = wrapper_cast(x);
    EXPECT_EQ(&r, &x);
}

// ---------------------------------------------------------------------------
// wrapper_caster type traits
// ---------------------------------------------------------------------------

TEST(WrapperCasterTraits, NotCopyable)
{
    using w_type = wrapper<int, feature::inplace::plain>;
    using caster_type = wrapper_caster<w_type &>;
    STATIC_EXPECT_FALSE(::std::is_copy_constructible_v<caster_type>);
}

TEST(WrapperCasterTraits, NotMovable)
{
    using w_type = wrapper<int, feature::inplace::plain>;
    using caster_type = wrapper_caster<w_type &>;
    STATIC_EXPECT_FALSE(::std::is_move_constructible_v<caster_type>);
}

TEST(WrapperCasterTraits, LvalueNotConvertibleToValue)
{
    using w_type = wrapper<int, feature::inplace::plain>;
    using caster_type = wrapper_caster<w_type &>;
    // &&-qualified operators: lvalue caster cannot be implicitly converted
    STATIC_EXPECT_FALSE((::std::is_convertible_v<caster_type &, int &>));
    STATIC_EXPECT_FALSE((::std::is_convertible_v<caster_type &, w_type &>));
}

TEST(WrapperCasterTraits, RvalueConvertibleToValueAndWrapper)
{
    using w_type = wrapper<int, feature::inplace::plain>;
    using caster_type = wrapper_caster<w_type &>;
    STATIC_EXPECT_TRUE((::std::is_convertible_v<caster_type &&, int &>));
    STATIC_EXPECT_TRUE((::std::is_convertible_v<caster_type &&, w_type &>));
}

// ---------------------------------------------------------------------------
// Single-layer wrapper — plain executor (no guard/unguard)
// ---------------------------------------------------------------------------

TEST(WrapperCastWrapper, CastToInnerValue)
{
    wrapper<int, feature::inplace::plain> w{42};
    int val = wrapper_cast(w);
    EXPECT_EQ(val, 42);
}

TEST(WrapperCastWrapper, CastToWrapperRef)
{
    wrapper<int, feature::inplace::plain> w{99};
    using w_type = ::std::remove_reference_t<decltype(w)>;
    w_type & ref = wrapper_cast(w);
    EXPECT_EQ(&ref, &w);
}

TEST(WrapperCastWrapper, CastToConstInnerValue)
{
    wrapper<int, feature::inplace::plain> const w{7};
    int const val = ::scl::wrapper_cast(w);
    EXPECT_EQ(val, 7);
}

// ---------------------------------------------------------------------------
// Single-layer wrapper — counting executor: lazy guard behaviour
// ---------------------------------------------------------------------------

TEST(WrapperCastCounting, NoGuardAtConstruction)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    [[maybe_unused]]
    auto caster = wrapper_cast(w);
    EXPECT_EQ(ctrs.guard_count, 0);
}

TEST(WrapperCastCounting, GuardAcquiredAtCast)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    int val = wrapper_cast(w); // guard activated here
    EXPECT_EQ(ctrs.guard_count, 1);
    EXPECT_EQ(val, 42);
}

TEST(WrapperCastCounting, GuardReleasedWhenCasterDestroyed)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    {
        auto caster = wrapper_cast(w);
        [[maybe_unused]]
        int val = ::std::move(caster);
        EXPECT_EQ(ctrs.unguard_count, 0); // caster still alive
    }
    EXPECT_EQ(ctrs.unguard_count, 1);
}

TEST(WrapperCastCounting, CastToWrapperRefNoGuard)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    using w_type = ::std::remove_reference_t<decltype(w)>;
    {
        [[maybe_unused]]
        w_type const & ref = wrapper_cast(w); // identity cast: no guard needed
        EXPECT_EQ(ctrs.guard_count, 0);
    }
    EXPECT_EQ(ctrs.unguard_count, 0);
}

// ---------------------------------------------------------------------------
// Nested wrapper: outer = plain, inner = counting_executor
// ---------------------------------------------------------------------------

TEST(WrapperCastNested, CastToOuterWrapperRefNoGuard)
{
    guard_counters inner_ctrs;
    outer_w w{inner_w{42, inner_ctrs}};
    outer_w & ref = wrapper_cast(w);
    EXPECT_EQ(&ref, &w);
    EXPECT_EQ(inner_ctrs.guard_count, 0);
}

TEST(WrapperCastNested, CastToInnerWrapperRefNoGuard)
{
    guard_counters inner_ctrs;
    outer_w w{inner_w{42, inner_ctrs}};
    [[maybe_unused]]
    inner_w const & ref = wrapper_cast(w);
    EXPECT_EQ(inner_ctrs.guard_count, 0); // inner identity: no guard
}

TEST(WrapperCastNested, CastToValueLocksInnerExecutor)
{
    guard_counters inner_ctrs;
    outer_w w{inner_w{42, inner_ctrs}};
    int val = wrapper_cast(w);
    EXPECT_EQ(inner_ctrs.guard_count, 1);
    EXPECT_EQ(val, 42);
}

TEST(WrapperCastNested, DestructorReleasesInnerGuard)
{
    guard_counters inner_ctrs;
    outer_w w{inner_w{42, inner_ctrs}};
    {
        auto caster = wrapper_cast(w);
        [[maybe_unused]]
        int val = ::std::move(caster);
        EXPECT_EQ(inner_ctrs.unguard_count, 0);
    }
    EXPECT_EQ(inner_ctrs.unguard_count, 1);
}

// ---------------------------------------------------------------------------
// wrapper_caster::to<T>() — explicit named cast
// ---------------------------------------------------------------------------

TEST(WrapperCasterTo, PassThroughToReturnsRef)
{
    int x = 42;
    int & r = wrapper_cast(x).to<int &>();
    EXPECT_EQ(&r, &x);
}

TEST(WrapperCasterTo, ToWrapperRefNoGuard)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    using w_type = ::std::remove_reference_t<decltype(w)>;
    w_type & ref = wrapper_cast(w).to<w_type &>();
    EXPECT_EQ(&ref, &w);
    EXPECT_EQ(ctrs.guard_count, 0);
    EXPECT_EQ(ctrs.unguard_count, 0);
}

TEST(WrapperCasterTo, ToInnerValueAcquiresGuard)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    int val = wrapper_cast(w).to<int &>();
    EXPECT_EQ(ctrs.guard_count, 1);
    EXPECT_EQ(val, 42);
}

TEST(WrapperCasterTo, ToReleasedOnCasterDestroy)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{99, ctrs};
    {
        auto caster = wrapper_cast(w);
        [[maybe_unused]]
        int & r = ::std::move(caster).to<int &>();
        EXPECT_EQ(ctrs.unguard_count, 0);
    }
    EXPECT_EQ(ctrs.unguard_count, 1);
}

TEST(WrapperCasterTo, WithoutCallNoGuard)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{0, ctrs};
    {
        [[maybe_unused]]
        auto caster = wrapper_cast(w);
        // .to<>() never called
    }
    EXPECT_EQ(ctrs.guard_count, 0);
    EXPECT_EQ(ctrs.unguard_count, 0);
}

TEST(WrapperCasterTo, LvalueNotInvocable)
{
    using w_type = wrapper<int, feature::inplace::plain>;
    using caster_type = wrapper_caster<w_type &>;
    STATIC_EXPECT_FALSE((::std::is_invocable_v<decltype(&caster_type::template to<int &>), caster_type &>));
}

TEST(WrapperCasterTo, NestedToOuterRefNoGuard)
{
    guard_counters inner_ctrs;
    outer_w w{inner_w{42, inner_ctrs}};
    outer_w & ref = wrapper_cast(w).to<outer_w &>();
    EXPECT_EQ(&ref, &w);
    EXPECT_EQ(inner_ctrs.guard_count, 0);
}

TEST(WrapperCasterTo, NestedToInnerWrapperRefNoGuard)
{
    guard_counters inner_ctrs;
    outer_w w{inner_w{42, inner_ctrs}};
    [[maybe_unused]]
    inner_w const & ref = wrapper_cast(w).to<inner_w &>();
    EXPECT_EQ(inner_ctrs.guard_count, 0);
}

TEST(WrapperCasterTo, NestedToValueLocksInnerExecutor)
{
    guard_counters inner_ctrs;
    outer_w w{inner_w{42, inner_ctrs}};
    int val = wrapper_cast(w).to<int &>();
    EXPECT_EQ(inner_ctrs.guard_count, 1);
    EXPECT_EQ(val, 42);
}

TEST(WrapperCasterTo, NestedDestructorReleasesInnerGuard)
{
    guard_counters inner_ctrs;
    outer_w w{inner_w{42, inner_ctrs}};
    {
        auto caster = wrapper_cast(w);
        [[maybe_unused]]
        int & r = ::std::move(caster).to<int &>();
        EXPECT_EQ(inner_ctrs.unguard_count, 0);
    }
    EXPECT_EQ(inner_ctrs.unguard_count, 1);
}
