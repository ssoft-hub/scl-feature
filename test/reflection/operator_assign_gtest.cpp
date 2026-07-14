#include <gtest_utils.h>

#include <scl/feature/wrapper.h>
#include <scl/utility/type_traits/forward_like.h>

#include <functional>
#include <string>
#include <utility>

// ============================================================================
// Reflected assignment on real `scl::wrapper`:
//   `w = x`              reflects `v = x`
//   `w = w2` same type   uses the wrapper's own copy-assign (NOT reflection)
//   `w = w2` other value reflects `v = v2`
//   incompatible value   ill-formed, SFINAE-friendly
//   compound `w += x`    reflects `v += x`
//   guard()/unguard()    fire once per wrapper operand
//   executor `operator_assign` override takes priority over the reflected path
// ============================================================================

using namespace ::scl;

namespace
{
    int g_guard_a = 0;
    int g_unguard_a = 0;
    int g_guard_b = 0;
    int g_unguard_b = 0;

    void reset_counters() { g_guard_a = g_unguard_a = g_guard_b = g_unguard_b = 0; }

    template <typename Value>
    class counting_a
    {
        using self_type = counting_a<Value>;

    public:
        using value_type = Value;

        template <typename... Args>
        constexpr explicit counting_a(Args &&... args)
            : m_value{::std::forward<Args>(args)...}
        {}

        counting_a & operator=(counting_a const &) = delete;
        counting_a & operator=(counting_a &&) = delete;

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
    class counting_b
    {
        using self_type = counting_b<Value>;

    public:
        using value_type = Value;

        template <typename... Args>
        constexpr explicit counting_b(Args &&... args)
            : m_value{::std::forward<Args>(args)...}
        {}

        counting_b & operator=(counting_b const &) = delete;
        counting_b & operator=(counting_b &&) = delete;

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

    // -- executor with an operator_assign override. Marks value = rhs*3,
    //    returns long so the override path is distinguishable by return type. ---
    template <typename Value>
    class assign_override
    {
        using self_type = assign_override<Value>;

    public:
        using value_type = Value;

        template <typename... Args>
        constexpr explicit assign_override(Args &&... args)
            : m_value{::std::forward<Args>(args)...}
        {}

        assign_override & operator=(assign_override const &) = delete;
        assign_override & operator=(assign_override &&) = delete;

        static constexpr long operator_assign(self_type & self, int rhs) noexcept
        {
            self.m_value = rhs * 3;
            return self.m_value;
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

    // Like assign_override, but operator_assign takes the value by lvalue reference so it matches
    // the value category a same-type wrapper source resolves to (its held lvalue).
    template <typename Value>
    class assign_override_ref
    {
        using self_type = assign_override_ref<Value>;

    public:
        using value_type = Value;

        template <typename... Args>
        constexpr explicit assign_override_ref(Args &&... args)
            : m_value{::std::forward<Args>(args)...}
        {}

        assign_override_ref & operator=(assign_override_ref const &) = delete;
        assign_override_ref & operator=(assign_override_ref &&) = delete;

        static constexpr long operator_assign(self_type & self, value_type & rhs) noexcept
        {
            self.m_value = rhs * 3;
            return self.m_value;
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

    template <typename Lhs, typename Rhs>
    constexpr bool can_assign_v = requires { ::std::declval<Lhs>() = ::std::declval<Rhs>(); };

    // A value type whose copy-assignment binds `const &` (the implicit one): its volatile-qualified
    // assignment is ill-formed, which the self-assignment overloads must treat as SFINAE, not a
    // hard error, when the wrapper is instantiated for all eight cv-ref source qualifications.
    struct classy
    {
        int v;
        constexpr bool operator==(int rhs) const noexcept { return v == rhs; }
    };

    // A value type that is not assignable at all.
    struct non_assignable
    {
        int v;
        non_assignable & operator=(non_assignable const &) = delete;
        non_assignable & operator=(non_assignable &&) = delete;
    };
} // namespace

// ============================================================================
// `w = x` reflects `v = x`
// ============================================================================

TEST(ReflectAssign, PlainValue_Assigns)
{
    wrapper<int> w{1};
    w = 5;
    EXPECT_EQ(w, 5);
}

TEST(ReflectAssign, PlainValue_Compiles)
{
    // A plain value on the RHS must be assignable to the wrapper.
    STATIC_EXPECT_TRUE((can_assign_v<wrapper<int> &, int>));
}

// ============================================================================
// compound `w += x` reflects `v += x`
// ============================================================================

TEST(ReflectAssign, Compound_PlusAssign)
{
    wrapper<int> w{10};
    w += 5;
    EXPECT_EQ(w, 15);
}

TEST(ReflectAssign, Compound_AllArithmeticBitwise)
{
    STATIC_EXPECT_TRUE((can_assign_v<wrapper<int> &, int>));
    STATIC_EXPECT_TRUE(requires { ::std::declval<wrapper<int> &>() += 1; });
    STATIC_EXPECT_TRUE(requires { ::std::declval<wrapper<int> &>() -= 1; });
    STATIC_EXPECT_TRUE(requires { ::std::declval<wrapper<int> &>() *= 1; });
    STATIC_EXPECT_TRUE(requires { ::std::declval<wrapper<int> &>() /= 1; });
    STATIC_EXPECT_TRUE(requires { ::std::declval<wrapper<int> &>() |= 1; });
    STATIC_EXPECT_TRUE(requires { ::std::declval<wrapper<int> &>() &= 1; });
    STATIC_EXPECT_TRUE(requires { ::std::declval<wrapper<int> &>() <<= 1; });
}

// ============================================================================
// same wrapper type uses the wrapper's own copy-assign, not reflection
// ============================================================================

TEST(ReflectAssign, SameType_UsesWrapperCopyAssign)
{
    wrapper<int> a{1};
    wrapper<int> b{2};
    a = b; // wrapper copy-assign, no ambiguity with a reflected value-assign
    EXPECT_EQ(a, 2);
}

// ============================================================================
// `w = w2` different value type reflects `v = v2`
// ============================================================================

TEST(ReflectAssign, DifferentValue_Assigns)
{
    wrapper<long> a{0};
    wrapper<int> b{7};
    a = b; // long = int, reflected value-assign
    EXPECT_EQ(a, 7L);
}

TEST(ReflectAssign, DifferentValue_Compiles)
{
    STATIC_EXPECT_TRUE((can_assign_v<wrapper<long> &, wrapper<int> &>));
}

// ============================================================================
// incompatible value assignment is ill-formed (SFINAE-friendly)
// ============================================================================

TEST(ReflectAssign, IncompatibleValue_StaysInvalid)
{
    STATIC_EXPECT_FALSE((can_assign_v<wrapper<int> &, int *>));
    STATIC_EXPECT_FALSE((can_assign_v<wrapper<int> &, wrapper<std::string> &>));
}

// A class value whose assignment binds `const &` instantiates without a hard error on the
// volatile-qualified self-assignment overloads, and same-type assignment still works.
TEST(ReflectAssign, ClassValue_SameTypeAssigns)
{
    wrapper<classy> a{classy{1}};
    wrapper<classy> b{classy{2}};
    a = b;
    EXPECT_EQ(a, 2);
}

// A non-assignable value leaves assignment SFINAE-absent (the wrapper still instantiates).
TEST(ReflectAssign, NonAssignableValue_StaysInvalid)
{
    STATIC_EXPECT_FALSE((can_assign_v<wrapper<non_assignable> &, wrapper<non_assignable> &>));
    STATIC_EXPECT_FALSE((can_assign_v<wrapper<non_assignable> &, non_assignable>));
}

// ============================================================================
// executor override `operator_assign` takes priority
// ============================================================================

TEST(ReflectAssign, ExecutorOverride_TakesPriority)
{
    wrapper<int, assign_override> w{0};
    auto result = (w = 7);
    static_assert(::std::same_as<decltype(result), long>,
        "operator_assign override must return long — the override path");
    EXPECT_EQ(result, 21L); // 7 * 3
    EXPECT_EQ(w, 21);
}

// The override is honoured on the self-type path too: a same-type source is read to its
// value and passed to operator_assign, matching the operand path.
TEST(ReflectAssign, ExecutorOverride_SameTypeSource)
{
    wrapper<int, assign_override_ref> a{0};
    wrapper<int, assign_override_ref> b{7};
    auto result = (a = b);
    static_assert(::std::same_as<decltype(result), long>,
        "operator_assign override must return long — the override path");
    EXPECT_EQ(result, 21L); // 7 * 3
    EXPECT_EQ(a, 21);
}

// ============================================================================
// guard()/unguard() fire once per wrapper operand
//
// Counters are snapshotted immediately after the assignment: a later reflected read of the
// wrapper would guard it again and pollute the count.
// ============================================================================

TEST(ReflectAssign, PlainValueAssign_GuardsTargetOnce)
{
    reset_counters();
    wrapper<int, counting_a> w{0};
    w = 5; // target guarded once, plain source not a wrapper
    int const guard = g_guard_a;
    int const unguard = g_unguard_a;
    EXPECT_EQ(w, 5);
    EXPECT_EQ(guard, 1);
    EXPECT_EQ(unguard, 1);
}

TEST(ReflectAssign, DifferentValueAssign_GuardsEachOperandOnce)
{
    reset_counters();
    wrapper<long, counting_a> a{0};
    wrapper<int, counting_b> b{7};
    a = b; // reflected value-assign: both operands read under guard
    int const guard_a = g_guard_a;
    int const unguard_a = g_unguard_a;
    EXPECT_EQ(a, 7L);
    EXPECT_EQ(guard_a, 1);
    EXPECT_EQ(unguard_a, 1);
    EXPECT_EQ(g_guard_b, 1);
    EXPECT_EQ(g_unguard_b, 1);
}

// Same wrapper type: the explicit self-type overload delegates to the same
// free-path dispatch, guarding the source here and the target inside the
// delegated write — one guard/unguard per operand (same counter, so two each).
TEST(ReflectAssign, SameTypeAssign_GuardsEachOperandOnce)
{
    reset_counters();
    wrapper<int, counting_a> a{1};
    wrapper<int, counting_a> b{9};
    a = b;
    int const guard = g_guard_a;
    int const unguard = g_unguard_a;
    EXPECT_EQ(a, 9);
    EXPECT_EQ(guard, 2);
    EXPECT_EQ(unguard, 2);
}
