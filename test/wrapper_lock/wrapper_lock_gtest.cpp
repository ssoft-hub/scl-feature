#include <gtest_utils.h>

#include <scl/feature/wrapper.h>
#include <scl/feature/wrapper_lock.h>

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
    static constexpr decltype(auto) value(Self && self)
        requires std::same_as<std::remove_cvref_t<Self>, counting_executor>
    {
        return ::scl::forward_like<Self>(self.m_value);
    }

    template <typename Self>
    static constexpr void guard(Self & self)
        requires std::same_as<std::remove_cvref_t<Self>, counting_executor>
    {
        ++self.m_counters.guard_count;
    }

    template <typename Self>
    static constexpr void unguard(Self & self)
        requires std::same_as<std::remove_cvref_t<Self>, counting_executor>
    {
        ++self.m_counters.unguard_count;
    }

    T m_value;
    guard_counters & m_counters;
};

// ---------------------------------------------------------------------------
// Non-wrapper specialisation
// ---------------------------------------------------------------------------

TEST(WrapperLockValue, NotCopyable)
{
    STATIC_EXPECT_FALSE(::std::is_copy_constructible_v<wrapper_lock<int &>>);
}

TEST(WrapperLockValue, NotMovable)
{
    STATIC_EXPECT_FALSE(::std::is_move_constructible_v<wrapper_lock<int &>>);
}

TEST(WrapperLockValue, HoldsRef)
{
    int x = 42;
    wrapper_lock<int &> lk{x};
    EXPECT_EQ(&lk.value(), &x);
    EXPECT_EQ(lk.value(), 42);
}

TEST(WrapperLockValue, HoldsConstRef)
{
    int const x = 7;
    wrapper_lock<int const &> lk{x};
    EXPECT_EQ(&lk.value(), &x);
}

TEST(WrapperLockValue, LockUnlockNoOp)
{
    int x = 0;
    wrapper_lock<int &> lk{x};
    lk.lock();
    lk.unlock();
    EXPECT_EQ(x, 0);
}

// ---------------------------------------------------------------------------
// Wrapper specialisation — plain executor (no guard/unguard)
// ---------------------------------------------------------------------------

TEST(WrapperLockWrapper, NotCopyable)
{
    using w_type = wrapper<int, feature::inplace::plain>;
    STATIC_EXPECT_FALSE(::std::is_copy_constructible_v<wrapper_lock<w_type &>>);
}

TEST(WrapperLockWrapper, NotMovable)
{
    using w_type = wrapper<int, feature::inplace::plain>;
    STATIC_EXPECT_FALSE(::std::is_move_constructible_v<wrapper_lock<w_type &>>);
}

TEST(WrapperLockWrapper, WrapperValue)
{
    wrapper<int, feature::inplace::plain> w{99};
    wrapper_lock<decltype(w) &> lk{w};
    EXPECT_EQ(&lk.wrapper_value(), &w);
}

TEST(WrapperLockWrapper, ValueThroughPlainExecutor)
{
    wrapper<int, feature::inplace::plain> w{42};
    wrapper_lock<decltype(w) &> lk{w};
    lk.lock();
    EXPECT_EQ(lk.value(), 42);
}

TEST(WrapperLockWrapper, PlainExecutorLockUnlockNoGuard)
{
    // plain has no guard/unguard — lock/unlock must not crash or do anything observable
    wrapper<int, feature::inplace::plain> w{0};
    wrapper_lock<decltype(w) &> lk{w};
    lk.lock();
    lk.unlock();
    EXPECT_EQ(lk.value(), 0);
}

// ---------------------------------------------------------------------------
// Wrapper specialisation — counting executor (guard/unguard)
// ---------------------------------------------------------------------------

TEST(WrapperLockCounting, NoGuardAtConstruction)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    {
        [[maybe_unused]]
        wrapper_lock<decltype(w) &> lk{w};
    }
    EXPECT_EQ(ctrs.guard_count, 0);
    EXPECT_EQ(ctrs.unguard_count, 0);
}

TEST(WrapperLockCounting, LockCallsGuard)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    wrapper_lock<decltype(w) &> lk{w};
    lk.lock();
    EXPECT_EQ(ctrs.guard_count, 1);
    EXPECT_EQ(ctrs.unguard_count, 0);
}

TEST(WrapperLockCounting, UnlockCallsUnguard)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    wrapper_lock<decltype(w) &> lk{w};
    lk.lock();
    lk.unlock();
    EXPECT_EQ(ctrs.guard_count, 1);
    EXPECT_EQ(ctrs.unguard_count, 1);
}

TEST(WrapperLockCounting, DestructorCallsUnlock)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    {
        wrapper_lock<decltype(w) &> lk{w};
        lk.lock();
        EXPECT_EQ(ctrs.unguard_count, 0);
    }
    EXPECT_EQ(ctrs.unguard_count, 1);
}

TEST(WrapperLockCounting, DestructorWithoutLockNoUnguard)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    {
        [[maybe_unused]]
        wrapper_lock<decltype(w) &> lk{w};
        // lock() never called
    }
    EXPECT_EQ(ctrs.guard_count, 0);
    EXPECT_EQ(ctrs.unguard_count, 0);
}

TEST(WrapperLockCounting, LockIsIdempotent)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    wrapper_lock<decltype(w) &> lk{w};
    lk.lock();
    lk.lock(); // second call must be a no-op
    EXPECT_EQ(ctrs.guard_count, 1);
}

TEST(WrapperLockCounting, ValueAfterLock)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    wrapper_lock<decltype(w) &> lk{w};
    lk.lock();
    EXPECT_EQ(lk.value(), 42);
}

TEST(WrapperLockCounting, ConstWrapper)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> const w{7, ctrs};
    wrapper_lock<decltype(w) &> lk{w};
    lk.lock();
    EXPECT_EQ(lk.value(), 7);
}
