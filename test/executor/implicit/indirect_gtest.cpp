#include <gtest_utils.h>

#include <scl/feature/concepts/executor.h>
#include <scl/feature/executor/implicit/indirect.h>
#include <scl/feature/type_traits/executor.h>
#include <scl/feature/wrapper.h>

#include <cstddef>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

using namespace ::scl;

using IntIndirect = feature::implicit::indirect<int>;

namespace
{
    /// Reflected value type: a mutating `set` (&) and a read-only `get` (const&).
    struct Box
    {
        int value = 0;

        void set(int v) & { value = v; }
        [[nodiscard]]
        int get() const &
        {
            return value;
        }
    };
} // namespace

template <typename Wrapper, typename Executor>
class scl::feature::reflect<Wrapper, Executor, Box>
{
public:
    SCL_REFLECT_TYPE(Wrapper, Executor)

    SCL_REFLECT_METHOD(set)
    SCL_REFLECT_METHOD(get)
};

namespace
{
    /// Address of the held value, read through a const executor — the no-clone path.
    template <typename Executor>
    auto const * address_of(Executor const & e)
    {
        return ::std::addressof(::std::remove_cvref_t<Executor>::access(e));
    }

    /// Process-wide allocation counters shared by every `counting_allocator<T>`.
    struct alloc_stats
    {
        inline static ::std::size_t allocations = 0;
        inline static ::std::size_t deallocations = 0;

        static void reset() noexcept
        {
            allocations = 0;
            deallocations = 0;
        }

        static ::std::size_t live() noexcept { return allocations - deallocations; }
    };

    /// Minimal stateless allocator that tallies (de)allocations into `alloc_stats`.
    template <typename T>
    struct counting_allocator
    {
        using value_type = T;

        counting_allocator() = default;
        template <typename U>
        explicit counting_allocator(counting_allocator<U> const & /*other*/) noexcept
        {}

        T * allocate(::std::size_t n)
        {
            ++alloc_stats::allocations;
            return ::std::allocator<T>{}.allocate(n);
        }

        void deallocate(T * p, ::std::size_t n) noexcept
        {
            ++alloc_stats::deallocations;
            ::std::allocator<T>{}.deallocate(p, n);
        }

        template <typename U>
        bool operator==(counting_allocator<U> const & /*other*/) const noexcept
        {
            return true;
        }
    };

    template <typename T>
    using counted_indirect = feature::implicit::indirect<T, counting_allocator<T>>;
} // namespace

// ============================================================================
// Executor interface
// ============================================================================

TEST(ImplicitIndirect, SatisfiesExecutorConcept)
{
    STATIC_EXPECT_TRUE(feature::concepts::executor<feature::implicit::indirect<int>>);
    STATIC_EXPECT_TRUE(feature::concepts::executor<feature::implicit::indirect<::std::string>>);
    // cv-ref qualifiers are stripped before the check.
    STATIC_EXPECT_TRUE(feature::concepts::executor<feature::implicit::indirect<int> const>);
}

TEST(ImplicitIndirect, NoexceptIsComputedFromConstness)
{
    // guard: const is no-op → noexcept; non-const may clone → throwing.
    STATIC_EXPECT_TRUE(noexcept(IntIndirect::guard(::std::declval<IntIndirect const &>())));
    STATIC_EXPECT_FALSE(noexcept(IntIndirect::guard(::std::declval<IntIndirect &>())));
    // access: pure accessor, unconditionally noexcept regardless of constness.
    STATIC_EXPECT_TRUE(noexcept(IntIndirect::access(::std::declval<IntIndirect const &>())));
    STATIC_EXPECT_TRUE(noexcept(IntIndirect::access(::std::declval<IntIndirect &>())));
}

TEST(ImplicitIndirect, ExecutorIsSinglePointerWide)
{
    // The Plan B payoff: an intrusive control block keeps the executor one pointer wide
    // (empty allocator folded away), unlike a two-pointer std::shared_ptr.
    STATIC_EXPECT_EQ(sizeof(feature::implicit::indirect<int>), sizeof(void *));
}

// ============================================================================
// Construction
// ============================================================================

TEST(ImplicitIndirect, ConstructFromValue)
{
    IntIndirect const e{42};
    EXPECT_EQ(IntIndirect::access(e), 42);
}

TEST(ImplicitIndirect, ConstructForwardsArgumentsToValue)
{
    feature::implicit::indirect<::std::string> const e{3, 'a'};
    EXPECT_EQ(feature::implicit::indirect<::std::string>::access(e), "aaa");
}

// ============================================================================
// Copy shares the allocation; reads never clone
// ============================================================================

TEST(ImplicitIndirect, CopyConstructionSharesSameObject)
{
    IntIndirect a{1};
    IntIndirect b{a}; // NOLINT(performance-unnecessary-copy-initialization) — intentional share

    EXPECT_EQ(address_of(a), address_of(b));
}

TEST(ImplicitIndirect, ConstAccessDoesNotClone)
{
    IntIndirect a{1};
    IntIndirect b{a};

    int const * const before = address_of(a);
    ::std::ignore = IntIndirect::access(::std::as_const(a)); // const read

    EXPECT_EQ(address_of(a), before);
    EXPECT_EQ(address_of(a), address_of(b)); // still shared
}

// ============================================================================
// Copy-on-write: guard() clones a shared value before mutable access
// ============================================================================

TEST(ImplicitIndirect, GuardOnSharedValueClonesAndLeavesOtherOwnerUntouched)
{
    IntIndirect a{1};
    IntIndirect b{a};

    IntIndirect::guard(a);      // shared, non-const → clone (copy-on-write step)
    IntIndirect::access(a) = 9; // now mutating a's own copy

    EXPECT_NE(address_of(a), address_of(b)); // diverged
    EXPECT_EQ(IntIndirect::access(::std::as_const(a)), 9);
    EXPECT_EQ(IntIndirect::access(::std::as_const(b)), 1); // other owner untouched
}

TEST(ImplicitIndirect, GuardOnSoleOwnerDoesNotClone)
{
    IntIndirect a{1};

    int const * const before = address_of(a);
    IntIndirect::access(a) = 7; // sole owner → no clone

    EXPECT_EQ(address_of(a), before);
    EXPECT_EQ(IntIndirect::access(::std::as_const(a)), 7);
}

// ============================================================================
// Move transfers ownership
// ============================================================================

TEST(ImplicitIndirect, MoveConstructionTransfersValue)
{
    IntIndirect a{5};
    IntIndirect b{::std::move(a)};

    EXPECT_EQ(IntIndirect::access(::std::as_const(b)), 5);
}

// ============================================================================
// Allocator: single allocation per value, clone allocates once, no leak
// ============================================================================

class ImplicitIndirectAllocator : public ::testing::Test
{
protected:
    using Counted = counted_indirect<int>;

    void SetUp() override { alloc_stats::reset(); }
};

TEST_F(ImplicitIndirectAllocator, ConstructAllocatesOneBlock)
{
    {
        Counted const a{1};
        EXPECT_EQ(alloc_stats::allocations, 1U);
    }
    EXPECT_EQ(alloc_stats::live(), 0U);
}

TEST_F(ImplicitIndirectAllocator, CopyDoesNotAllocate)
{
    Counted a{1};
    Counted b{a};

    EXPECT_EQ(alloc_stats::allocations, 1U);
    EXPECT_EQ(address_of(a), address_of(b));
}

TEST_F(ImplicitIndirectAllocator, GuardOnSharedValueAllocatesOneMoreBlock)
{
    Counted a{1};
    Counted b{a};

    Counted::guard(a);

    EXPECT_EQ(alloc_stats::allocations, 2U);
}

TEST_F(ImplicitIndirectAllocator, GuardOnSoleOwnerDoesNotAllocate)
{
    Counted a{1};

    Counted::guard(a);

    EXPECT_EQ(alloc_stats::allocations, 1U);
}

TEST_F(ImplicitIndirectAllocator, SharedThenDetachedReleasesEveryBlock)
{
    {
        Counted a{1};
        Counted b{a};
        Counted::guard(a); // a detaches into its own block
    }
    EXPECT_EQ(alloc_stats::allocations, 2U);
    EXPECT_EQ(alloc_stats::live(), 0U);
}

TEST_F(ImplicitIndirectAllocator, CustomAllocatorRoutesWrapperAllocations)
{
    // One-parameter alias binds the allocator, fitting wrapper's template-template slot
    // without relying on defaulted-argument matching.
    wrapper<Box, counted_indirect> w{Box{5}};

    EXPECT_EQ(alloc_stats::allocations, 1U);
    EXPECT_EQ(w.get(), 5);
}

// ============================================================================
// Counter type is configurable — a non-atomic counter still drives copy-on-write
// ============================================================================

TEST(ImplicitIndirect, NonAtomicCounterClonesCorrectly)
{
    using NonAtomic = feature::implicit::indirect<int, ::std::allocator<int>, ::std::size_t>;

    STATIC_EXPECT_TRUE(feature::concepts::executor<NonAtomic>);

    NonAtomic a{1};
    NonAtomic b{a};

    NonAtomic::guard(a);
    NonAtomic::access(a) = 9;

    EXPECT_EQ(NonAtomic::access(::std::as_const(a)), 9);
    EXPECT_EQ(NonAtomic::access(::std::as_const(b)), 1);
}

// ============================================================================
// End-to-end through scl::wrapper — the real execute()/access() dispatch path.
// This is the regression guard: a mutating call on a shared wrapper must clone,
// so the original copy is unaffected.
// ============================================================================

TEST(ImplicitIndirectWrapper, MutatingCallOnCopyLeavesOriginalUnchanged)
{
    using W = wrapper<Box, feature::implicit::indirect>;

    W a{Box{1}};
    W b = a; // shares the allocation

    b.set(2); // non-const method call → execute path → guard clones b

    EXPECT_EQ(a.get(), 1); // original untouched (would be 2 if CoW failed)
    EXPECT_EQ(b.get(), 2);
}

TEST(ImplicitIndirectWrapper, ConstWrapperReadCompilesAndDoesNotClone)
{
    using W = wrapper<Box, feature::implicit::indirect>;

    W const a{Box{7}};
    W const b = a; // shares

    // Const reads only — must compile (no const detach instantiation) and not diverge.
    EXPECT_EQ(a.get(), 7);
    EXPECT_EQ(b.get(), 7);
}

// ============================================================================
// operator_assign: wrapper assignment shares and detaches correctly
// ============================================================================

namespace
{
    /// Address of the held value inside a wrapper, accessed through a const executor.
    template <typename W>
    auto const * wrapper_value_address(W const & w)
    {
        using WT = ::std::remove_cvref_t<W>;
        using ET = typename WT::executor_type;
        return ::std::addressof(ET::access(::scl::feature::executor_trait<WT>::executor(w)));
    }
} // namespace

TEST(ImplicitIndirectWrapper, CopyAssignmentSharesAndDetachesCorrectly)
{
    using W = wrapper<Box, feature::implicit::indirect>;

    W a{Box{1}};
    W b{Box{2}};
    b = a;

    EXPECT_EQ(wrapper_value_address(a), wrapper_value_address(b)); // shared after assignment

    b.set(9); // non-const call → execute path → guard clones b

    EXPECT_NE(wrapper_value_address(a), wrapper_value_address(b)); // diverged after CoW
    EXPECT_EQ(a.get(), 1);
    EXPECT_EQ(b.get(), 9);
}

TEST(ImplicitIndirectWrapper, MoveAssignmentTransfersOwnership)
{
    using W = wrapper<Box, feature::implicit::indirect>;

    W a{Box{5}};
    W b{Box{0}};
    b = ::std::move(a);

    EXPECT_EQ(b.get(), 5);
}
