#include <gtest_utils.h>

#include <scl/feature/wrapper.h>
#include <scl/utility/type_traits/forward_like.h>

#include <functional>
#include <string>
#include <utility>

// ============================================================================
// Target (acceptance) suite for reflected FREE (non-member) operators on real
// `scl::wrapper`.  Encodes the behavioural contract:
//   FR3.forward   `w op x`   reflects `v op x`
//   FR3.reverse   `x op w`   reflects `x op v`
//   FR3.both      `w1 op w2` reflects `v1 op v2` (incl. cross-executor, nested)
//   FR5           member operator of the value wins over the free path
//   FR7           guard()/unguard() fire once per wrapped operand per level
//   FR10          no operator on the value -> ill-formed, SFINAE-friendly
// ============================================================================

using namespace ::scl;

namespace
{
    // -- guard-counting executors (plain-like, but count guard/unguard) --------
    int g_guard_a = 0;
    int g_unguard_a = 0;
    int g_guard_b = 0;
    int g_unguard_b = 0;

    void reset_counters() { g_guard_a = g_unguard_a = g_guard_b = g_unguard_b = 0; }

    template <typename Value>
    class counting_fa
    {
        using self_type = counting_fa<Value>;

    public:
        using value_type = Value;

        template <typename... Args>
        constexpr explicit counting_fa(Args &&... args)
            : m_value{::std::forward<Args>(args)...}
        {}

        counting_fa & operator=(counting_fa const &) = delete;
        counting_fa & operator=(counting_fa &&) = delete;

        template <typename Self>
        static void guard(Self &&) noexcept
        {
            ++g_guard_a;
        }
        template <typename Self>
        static void unguard(Self &&) noexcept
        {
            ++g_unguard_a;
        }
        template <typename Self, typename Func, typename... Args>
        static constexpr decltype(auto) execute(Self &&, Func && func, Args &&... args)
        {
            return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
        }
        template <typename Self>
        static constexpr decltype(auto) access(Self && self)
        {
            return ::scl::forward_like<Self>(self.m_value);
        }

    private:
        value_type m_value;
    };

    template <typename Value>
    class counting_fb
    {
        using self_type = counting_fb<Value>;

    public:
        using value_type = Value;

        template <typename... Args>
        constexpr explicit counting_fb(Args &&... args)
            : m_value{::std::forward<Args>(args)...}
        {}

        counting_fb & operator=(counting_fb const &) = delete;
        counting_fb & operator=(counting_fb &&) = delete;

        template <typename Self>
        static void guard(Self &&) noexcept
        {
            ++g_guard_b;
        }
        template <typename Self>
        static void unguard(Self &&) noexcept
        {
            ++g_unguard_b;
        }
        template <typename Self, typename Func, typename... Args>
        static constexpr decltype(auto) execute(Self &&, Func && func, Args &&... args)
        {
            return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
        }
        template <typename Self>
        static constexpr decltype(auto) access(Self && self)
        {
            return ::scl::forward_like<Self>(self.m_value);
        }

    private:
        value_type m_value;
    };

    // -- member-precedence target: has BOTH a member + and a free + -----------
    // Distinct return types prove which path ran: member -> short, free -> long.
    struct member_free_target
    {
        int value = 0;

        constexpr short operator+(int rhs) const { return static_cast<short>(value + rhs); }

        friend constexpr long operator+(member_free_target const & self, long rhs)
        {
            return static_cast<long>(self.value) + rhs + 1000;
        }
    };

    template <typename T, typename U>
    constexpr bool can_add_v = requires { ::std::declval<T>() + ::std::declval<U>(); };
} // namespace

// ============================================================================
// FR3.forward — `w op x` reflects the value's free / built-in operator (AC1)
// ============================================================================

TEST(ReflectFreeForward, IntPlusInt_ReflectsBuiltin)
{
    constexpr wrapper<int> w{10};
    STATIC_EXPECT_EQ(w + 5, 15);
}

TEST(ReflectFreeForward, IntLessThanInt_ReflectsBuiltin)
{
    constexpr wrapper<int> w{5};
    STATIC_EXPECT_TRUE(w < 6);
}

TEST(ReflectFreeForward, ConstWrapper_ForwardValid)
{
    constexpr wrapper<int> const w{10};
    STATIC_EXPECT_EQ(w * 3, 30);
}

TEST(ReflectFreeForward, ArithmeticBitwiseShift_AllValid)
{
    constexpr wrapper<int> w{12};
    STATIC_EXPECT_EQ(w + 3, 15);
    STATIC_EXPECT_EQ(w - 2, 10);
    STATIC_EXPECT_EQ(w * 2, 24);
    STATIC_EXPECT_EQ(w / 4, 3);
    STATIC_EXPECT_EQ(w % 5, 2);
    STATIC_EXPECT_EQ(w >> 2, 3);
    STATIC_EXPECT_EQ(w & 8, 8);
    STATIC_EXPECT_EQ(w | 1, 13);
}

TEST(ReflectFreeForward, NoOperatorAtAll_StaysInvalid)
{
    // FR10 — a type with no `+` at all: `w + x` ill-formed, SFINAE-friendly.
    struct no_ops
    {};
    STATIC_EXPECT_FALSE((can_add_v<wrapper<no_ops> &, int>));
}

// ============================================================================
// FR3.reverse — `x op w` reflects `x op v` (AC2)
// ============================================================================

TEST(ReflectFreeReverse, IntReverse_ReflectsBuiltin)
{
    constexpr wrapper<int> w{10};
    STATIC_EXPECT_EQ(5 + w, 15);
}

TEST(ReflectFreeReverse, IntReverseLessThan)
{
    constexpr wrapper<int> w{10};
    STATIC_EXPECT_TRUE(5 < w);
    STATIC_EXPECT_FALSE(20 < w);
}

// ============================================================================
// FR3.both — `w1 op w2` reflects `v1 op v2` (AC3, AC4, AC5)
// ============================================================================

TEST(ReflectFreeBoth, IntPlusInt_SameExecutor)
{
    constexpr wrapper<int> a{2};
    constexpr wrapper<int> b{3};
    STATIC_EXPECT_EQ(a + b, 5);
}

TEST(ReflectFreeBoth, IntLessThan_SameExecutor)
{
    constexpr wrapper<int> a{2};
    constexpr wrapper<int> b{3};
    STATIC_EXPECT_TRUE(a < b);
}

TEST(ReflectFreeBoth, CrossExecutor_Valid)
{
    wrapper<int, counting_fa> a{4};
    wrapper<int, counting_fb> b{6};
    EXPECT_EQ(a + b, 10);
}

TEST(ReflectFreeBoth, NestedWrapper_Recurses)
{
    wrapper<wrapper<int>> a{wrapper<int>{7}};
    wrapper<wrapper<int>> b{wrapper<int>{8}};
    EXPECT_EQ(a + b, 15);
}

// ============================================================================
// FR5 — member operator of the value wins over the free path (AC6)
// ============================================================================

TEST(ReflectFreePrecedence, MemberWinsOverFree)
{
    wrapper<member_free_target> w{member_free_target{10}};
    // member operator+(int) -> short is chosen for `w + 5`, not the free +.
    auto result = w + 5;
    static_assert(::std::same_as<decltype(result), short>,
        "member operator+ must win over the free operator+ (FR5)");
    EXPECT_EQ(result, static_cast<short>(15));
}

// ============================================================================
// FR7 — guard()/unguard() fire once per wrapped operand per level
// ============================================================================

TEST(ReflectFreeGuard, ForwardGuardsWrappedValueOnce)
{
    reset_counters();
    wrapper<int, counting_fa> w{10};
    EXPECT_EQ(w + 5, 15);
    EXPECT_EQ(g_guard_a, 1);
    EXPECT_EQ(g_unguard_a, 1);
}

TEST(ReflectFreeGuard, ReverseGuardsWrappedValueOnce)
{
    reset_counters();
    wrapper<int, counting_fa> w{10};
    EXPECT_EQ(5 + w, 15);
    EXPECT_EQ(g_guard_a, 1);
    EXPECT_EQ(g_unguard_a, 1);
}

TEST(ReflectFreeGuard, BothGuardsEachOperandOnce)
{
    reset_counters();
    wrapper<int, counting_fa> a{4};
    wrapper<int, counting_fb> b{6};
    EXPECT_EQ(a + b, 10);
    EXPECT_EQ(g_guard_a, 1);
    EXPECT_EQ(g_unguard_a, 1);
    EXPECT_EQ(g_guard_b, 1);
    EXPECT_EQ(g_unguard_b, 1);
}

TEST(ReflectFreeGuard, NestedGuardsOncePerLevel)
{
    // FR7 — one guard/unguard per nesting level.  `counting_fa` is the outer level;
    // the inner wrapper<int> uses the default (guardless) executor, so only the
    // outer counter moves — exactly once per operand.
    reset_counters();
    wrapper<wrapper<int>, counting_fa> a{wrapper<int>{7}};
    wrapper<wrapper<int>, counting_fa> b{wrapper<int>{8}};
    EXPECT_EQ(a + b, 15);
    EXPECT_EQ(g_guard_a, 2); // once per operand at the outer level
    EXPECT_EQ(g_unguard_a, 2);
}
