#include <counting_executor.h>
#include <gtest_utils.h>

#include <scl/feature/value_lock.h>
#include <scl/feature/wrapper.h>
#include <scl/feature/wrapper_lock.h>

using namespace ::scl;

// Alias: outer wrapper uses plain (no guard), inner uses counting_executor.
// This lets us verify which layer is locked without two separate counter sets.
using inner_w = wrapper<int, counting_executor>;
using outer_w = wrapper<inner_w, feature::inplace::plain>;

// ---------------------------------------------------------------------------
// Non-wrapper specialisation
// ---------------------------------------------------------------------------

TEST(ValueLockValue, NotCopyable)
{
    STATIC_EXPECT_FALSE(::std::is_copy_constructible_v<value_lock<int &>>);
}

TEST(ValueLockValue, NotMovable)
{
    STATIC_EXPECT_FALSE(::std::is_move_constructible_v<value_lock<int &>>);
}

TEST(ValueLockValue, ValueAs)
{
    int x = 42;
    value_lock<int &> vl{x};
    EXPECT_EQ(&vl.value_as<int &>(), &x);
    EXPECT_EQ(vl.value_as<int &>(), 42);
}

TEST(ValueLockValue, LockForNoOp)
{
    int x = 0;
    value_lock<int &> vl{x};
    vl.lock_for<int &>(); // must compile and do nothing observable
    EXPECT_EQ(x, 0);
}

// ---------------------------------------------------------------------------
// Single-layer wrapper
// ---------------------------------------------------------------------------

TEST(ValueLockWrapper, NotCopyable)
{
    STATIC_EXPECT_FALSE(
        (::std::is_copy_constructible_v<value_lock<wrapper<int, feature::inplace::plain> &>>));
}

TEST(ValueLockWrapper, NotMovable)
{
    STATIC_EXPECT_FALSE(
        (::std::is_move_constructible_v<value_lock<wrapper<int, feature::inplace::plain> &>>));
}

TEST(ValueLockWrapper, NoGuardAtConstruction)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    {
        [[maybe_unused]]
        value_lock<decltype(w) &> vl{w};
    }
    EXPECT_EQ(ctrs.guard_count, 0);
    EXPECT_EQ(ctrs.unguard_count, 0);
}

TEST(ValueLockWrapper, LockForWrapperRefIsIdentityNoGuard)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    value_lock<decltype(w) &> vl{w};
    vl.lock_for<decltype(w) &>(); // identity: target == WrapperRefer, no guard
    EXPECT_EQ(ctrs.guard_count, 0);
}

TEST(ValueLockWrapper, LockForInnerValueAcquiresGuard)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    value_lock<decltype(w) &> vl{w};
    vl.lock_for<int &>();
    EXPECT_EQ(ctrs.guard_count, 1);
}

TEST(ValueLockWrapper, ValueAsWrapperRef)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    value_lock<decltype(w) &> vl{w};
    auto & ref = vl.value_as<decltype(w) &>();
    EXPECT_EQ(&ref, &w);
}

TEST(ValueLockWrapper, ValueAsInnerValue)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    value_lock<decltype(w) &> vl{w};
    vl.lock_for<int &>();
    EXPECT_EQ(vl.value_as<int &>(), 42);
}

TEST(ValueLockWrapper, DestructorReleasesGuard)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    {
        value_lock<decltype(w) &> vl{w};
        vl.lock_for<int &>();
        EXPECT_EQ(ctrs.unguard_count, 0);
    }
    EXPECT_EQ(ctrs.unguard_count, 1);
}

TEST(ValueLockWrapper, DestructorWithoutLockNoUnguard)
{
    guard_counters ctrs;
    wrapper<int, counting_executor> w{42, ctrs};
    {
        value_lock<decltype(w) &> vl{w};
        // lock_for() never called
    }
    EXPECT_EQ(ctrs.guard_count, 0);
    EXPECT_EQ(ctrs.unguard_count, 0);
}

// ---------------------------------------------------------------------------
// Nested wrapper: outer = plain (no guard), inner = counting_executor
// ---------------------------------------------------------------------------

TEST(ValueLockNested, NoGuardAtConstruction)
{
    guard_counters inner_ctrs;
    outer_w w{inner_w{42, inner_ctrs}};
    {
        [[maybe_unused]]
        value_lock<outer_w &> vl{w};
    }
    EXPECT_EQ(inner_ctrs.guard_count, 0);
    EXPECT_EQ(inner_ctrs.unguard_count, 0);
}

TEST(ValueLockNested, LockForOuterWrapperRefNoGuard)
{
    guard_counters inner_ctrs;
    outer_w w{inner_w{42, inner_ctrs}};
    value_lock<outer_w &> vl{w};
    vl.lock_for<outer_w &>(); // identity at outer level — no guard acquired
    EXPECT_EQ(inner_ctrs.guard_count, 0);
}

TEST(ValueLockNested, LockForInnerWrapperRefNoGuard)
{
    guard_counters inner_ctrs;
    outer_w w{inner_w{42, inner_ctrs}};
    value_lock<outer_w &> vl{w};
    vl.lock_for<inner_w &>(); // identity at inner level — outer plain, inner no lock needed
    EXPECT_EQ(inner_ctrs.guard_count, 0);
}

TEST(ValueLockNested, LockForValueLocksInnerExecutor)
{
    guard_counters inner_ctrs;
    outer_w w{inner_w{42, inner_ctrs}};
    value_lock<outer_w &> vl{w};
    vl.lock_for<int &>(); // must lock the counting_executor inside inner_w
    EXPECT_EQ(inner_ctrs.guard_count, 1);
}

TEST(ValueLockNested, ValueAsOuterWrapperRef)
{
    guard_counters inner_ctrs;
    outer_w w{inner_w{42, inner_ctrs}};
    value_lock<outer_w &> vl{w};
    EXPECT_EQ(&vl.value_as<outer_w &>(), &w);
}

TEST(ValueLockNested, ValueAsInnerWrapperRef)
{
    guard_counters inner_ctrs;
    outer_w w{inner_w{42, inner_ctrs}};
    value_lock<outer_w &> vl{w};
    inner_w & inner_ref = vl.value_as<inner_w &>();
    // Verify the inner wrapper holds the right value via wrapper_lock
    wrapper_lock<inner_w &> inner_lk{inner_ref};
    inner_lk.lock();
    EXPECT_EQ(inner_lk.value(), 42);
}

TEST(ValueLockNested, ValueAsIntRef)
{
    guard_counters inner_ctrs;
    outer_w w{inner_w{42, inner_ctrs}};
    value_lock<outer_w &> vl{w};
    vl.lock_for<int &>();
    EXPECT_EQ(vl.value_as<int &>(), 42);
}

TEST(ValueLockNested, DestructorReleasesInnerGuard)
{
    guard_counters inner_ctrs;
    outer_w w{inner_w{42, inner_ctrs}};
    {
        value_lock<outer_w &> vl{w};
        vl.lock_for<int &>();
        EXPECT_EQ(inner_ctrs.unguard_count, 0);
    }
    EXPECT_EQ(inner_ctrs.unguard_count, 1);
}
