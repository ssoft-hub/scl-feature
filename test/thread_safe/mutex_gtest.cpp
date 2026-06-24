#include <gtest/gtest.h>

#include <scl/feature/thread_safe/mutex.h>
#include <scl/feature/wrapper.h>
#include <scl/feature/wrapper_guard.h>

#include <mutex>
#include <thread>
#include <utility>

#define TEST_EXPECT_TRUE(X) \
    static_assert(X, #X);   \
    EXPECT_TRUE(X)

#define TEST_EXPECT_FALSE(X) \
    static_assert(!(X), #X); \
    EXPECT_FALSE(X)

using namespace ::scl;
using MutexInt = feature::thread_safe::mutex<int>;

// ---------------------------------------------------------------------------
// Demonstrate WHY explicit copy/move ctors are needed
// ---------------------------------------------------------------------------

TEST(ThreadSafeMutexTrait, StdMutexIsNotCopyConstructible)
{
    TEST_EXPECT_FALSE(::std::is_copy_constructible_v<::std::mutex>);
}

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

TEST(ThreadSafeMutexConstruct, DefaultInt)
{
    MutexInt e{0};
    EXPECT_EQ(MutexInt::value(e), 0);
}

TEST(ThreadSafeMutexConstruct, ForwardingCtor)
{
    MutexInt e{42};
    EXPECT_EQ(MutexInt::value(e), 42);
}

// ---------------------------------------------------------------------------
// Copy semantics — new mutex, value copied
// ---------------------------------------------------------------------------

TEST(ThreadSafeMutexCopy, CopyCtor)
{
    MutexInt const src{7};
    MutexInt dst{src};
    EXPECT_EQ(MutexInt::value(dst), 7);
}

TEST(ThreadSafeMutexCopy, CopyCtorNewLock)
{
    MutexInt const src{7};
    MutexInt dst{src};
    // Both must be independently guardable — guard one, then guard the other
    // from a second thread; they must not block each other.
    MutexInt::guard(dst);
    bool src_locked = false;
    ::std::thread t{[&] {
        MutexInt::guard(const_cast<MutexInt &>(src));
        src_locked = true;
        MutexInt::unguard(const_cast<MutexInt &>(src));
    }};
    t.join();
    MutexInt::unguard(dst);
    EXPECT_TRUE(src_locked);
}

TEST(ThreadSafeMutexCopy, CopyAssign)
{
    MutexInt const src{99};
    MutexInt dst{0};
    dst = src;
    EXPECT_EQ(MutexInt::value(dst), 99);
}

// ---------------------------------------------------------------------------
// Move semantics — new mutex, value moved
// ---------------------------------------------------------------------------

TEST(ThreadSafeMutexMove, MoveCtor)
{
    MutexInt src{55};
    MutexInt dst{::std::move(src)};
    EXPECT_EQ(MutexInt::value(dst), 55);
}

TEST(ThreadSafeMutexMove, MoveCtorNewLock)
{
    MutexInt src{55};
    MutexInt dst{::std::move(src)};
    // dst must own a fresh, unlocked mutex — guard/unguard must succeed.
    MutexInt::guard(dst);
    MutexInt::unguard(dst);
    // Verify the value was actually moved.
    EXPECT_EQ(MutexInt::value(dst), 55);
}

TEST(ThreadSafeMutexMove, MoveAssign)
{
    MutexInt src{77};
    MutexInt dst{0};
    dst = ::std::move(src);
    EXPECT_EQ(MutexInt::value(dst), 77);
}

// ---------------------------------------------------------------------------
// guard / unguard round-trip
// ---------------------------------------------------------------------------

TEST(ThreadSafeMutexGuard, GuardLocksUnguardUnlocks)
{
    MutexInt e{10};

    MutexInt::guard(e);
    // Lock is held — try_lock from the same thread would deadlock on a
    // non-recursive mutex; just verify value is accessible.
    EXPECT_EQ(MutexInt::value(e), 10);
    MutexInt::unguard(e);

    // After unguard the mutex must be free again — a second thread must be
    // able to guard it without blocking.
    bool locked_after_unguard = false;
    ::std::thread t{[&] {
        MutexInt::guard(e);
        locked_after_unguard = true;
        MutexInt::unguard(e);
    }};
    t.join();
    EXPECT_TRUE(locked_after_unguard);
}

// ---------------------------------------------------------------------------
// execute()
// ---------------------------------------------------------------------------

TEST(ThreadSafeMutexExecute, InvokesCallable)
{
    MutexInt e{5};
    auto const result = MutexInt::execute(e, [] { return 42; });
    EXPECT_EQ(result, 42);
}

// ---------------------------------------------------------------------------
// wrapper<int, thread_safe::mutex> integration
// ---------------------------------------------------------------------------

TEST(ThreadSafeMutexWrapper, WrapperGuardAccessesValue)
{
    wrapper<int, feature::thread_safe::mutex> w{100};
    {
        feature::wrapper_guard<decltype(w) &> g{w};
        EXPECT_EQ(g.value(), 100);
    }
}

TEST(ThreadSafeMutexWrapper, WrapperGuardMutatesValue)
{
    wrapper<int, feature::thread_safe::mutex> w{0};
    {
        feature::wrapper_guard<decltype(w) &> g{w};
        g.value() = 42;
    }
    {
        feature::wrapper_guard<decltype(w) &> g{w};
        EXPECT_EQ(g.value(), 42);
    }
}

TEST(ThreadSafeMutexWrapper, ConcurrentWriteDoesNotRace)
{
    wrapper<int, feature::thread_safe::mutex> w{0};

    auto increment = [&w]() {
        for (int i = 0; i < 1000; ++i)
        {
            feature::wrapper_guard<decltype(w) &> g{w};
            ++g.value();
        }
    };

    ::std::thread t1{increment};
    ::std::thread t2{increment};
    t1.join();
    t2.join();

    feature::wrapper_guard<decltype(w) &> g{w};
    EXPECT_EQ(g.value(), 2000);
}
