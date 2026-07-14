#include <gtest_utils.h>

#include <scl/feature/concepts/executor.h>
#include <scl/feature/wrapper.h>
#include <scl/utility/type_traits/forward_like.h>

#include <concepts>
#include <functional>
#include <utility>

// ============================================================================
// Reflected equality `==` / `!=` must return `bool` (a C++20 rewritten equality candidate
// requires it). The wrapper's operator== always returns `bool`; the inner result only has to be
// static_cast-ible to it — otherwise the operator is absent (SFINAE), never a hard error.
//
// Across the three equality paths — free/execute (`w == x`), reverse friend (`x == w`), and the
// executor `operator_equal_to` override — for an inner result that is:
//   bool                present, returns `bool`
//   non-bool            absent
//   explicit-bool proxy present (via static_cast), returns `bool`
// Free and reverse paths also read each operand through the executor guard.
// ============================================================================

using namespace ::scl;

namespace
{
    struct not_bool
    {};

    struct explicit_bool
    {
        bool b;
        constexpr explicit operator bool() const noexcept { return b; }
    };

    struct eq_bool
    {
        int v;
        constexpr bool operator==(int rhs) const noexcept { return v == rhs; }
    };

    struct eq_not_bool
    {
        int v;
        constexpr not_bool operator==(int) const noexcept { return {}; }
    };

    struct eq_explicit_bool
    {
        int v;
        constexpr explicit_bool operator==(int rhs) const noexcept
        {
            return explicit_bool{v == rhs};
        }
    };

    struct no_eq
    {
        int v;
    };

    // Execute-only executor.
    template <typename Value>
    class plain_exec
    {
    public:
        using value_type = Value;

        template <typename... Args>
        constexpr explicit plain_exec(Args &&... args)
            : m_value{::std::forward<Args>(args)...}
        {}

        plain_exec & operator=(plain_exec const &) = delete;
        plain_exec & operator=(plain_exec &&) = delete;

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

    int g_guard_depth = 0;
    int g_guard = 0;
    int g_unguard = 0;
    bool g_accessed_unguarded = false;

    void reset_guard_counters()
    {
        g_guard_depth = g_guard = g_unguard = 0;
        g_accessed_unguarded = false;
    }

    // Executor that records guard/unguard fire and whether any access ran outside a guard.
    template <typename Value>
    class guarding_exec
    {
    public:
        using value_type = Value;

        template <typename... Args>
        constexpr explicit guarding_exec(Args &&... args)
            : m_value{::std::forward<Args>(args)...}
        {}

        guarding_exec & operator=(guarding_exec const &) = delete;
        guarding_exec & operator=(guarding_exec &&) = delete;

        template <typename Self>
        static void guard(Self &&) noexcept
        {
            ++g_guard;
            ++g_guard_depth;
        }
        template <typename Self>
        static void unguard(Self &&) noexcept
        {
            ++g_unguard;
            --g_guard_depth;
        }
        template <typename Self, typename Func, typename... Args>
        static decltype(auto) execute(Self &&, Func && func, Args &&... args)
        {
            return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
        }
        template <typename Self>
        static decltype(auto) access(Self && self)
        {
            if (g_guard_depth == 0)
                g_accessed_unguarded = true;
            return ::scl::forward_like<Self>(self.m_value);
        }

    private:
        value_type m_value;
    };

    // Executor whose operator_equal_to override returns bool.
    template <typename Value>
    class eq_override_bool_exec
    {
    public:
        using value_type = Value;

        template <typename... Args>
        constexpr explicit eq_override_bool_exec(Args &&... args)
            : m_value{::std::forward<Args>(args)...}
        {}

        eq_override_bool_exec & operator=(eq_override_bool_exec const &) = delete;
        eq_override_bool_exec & operator=(eq_override_bool_exec &&) = delete;

        static constexpr bool operator_equal_to(eq_override_bool_exec &, int rhs) noexcept
        {
            return rhs == 42;
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

    // Executor whose operator_equal_to override returns a non-bool type.
    template <typename Value>
    class eq_override_not_bool_exec
    {
    public:
        using value_type = Value;

        template <typename... Args>
        constexpr explicit eq_override_not_bool_exec(Args &&... args)
            : m_value{::std::forward<Args>(args)...}
        {}

        eq_override_not_bool_exec & operator=(eq_override_not_bool_exec const &) = delete;
        eq_override_not_bool_exec & operator=(eq_override_not_bool_exec &&) = delete;

        static constexpr not_bool operator_equal_to(eq_override_not_bool_exec &, int) noexcept
        {
            return {};
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

    static_assert(feature::concepts::executor<plain_exec<int>>);
    static_assert(feature::concepts::executor<guarding_exec<int>>);
    static_assert(feature::concepts::executor<eq_override_bool_exec<int>>);
    static_assert(feature::concepts::executor<eq_override_not_bool_exec<int>>);

    // Probe as a variable template so `==` lives in a dependent context: a non-dependent invalid
    // `==` in a plain requires-expression is a hard error, not `false`. Left is an lvalue and Right
    // an rvalue, matching the real `w == 42` and the override's exact fn-pointer match.
    template <typename Left, typename Right>
    constexpr bool eq_ok = requires(Left & left) { left == ::std::declval<Right>(); };
} // namespace

// ============================================================================
// Free/execute path (w == x)
// ============================================================================

TEST(EqualityBool, BoolResult_Present_ReturnsBool)
{
    constexpr wrapper<eq_bool, plain_exec> w{5};
    STATIC_EXPECT_TRUE((eq_ok<wrapper<eq_bool, plain_exec>, int>));
    STATIC_EXPECT_TRUE((::std::same_as<decltype(w == 5), bool>));
    STATIC_EXPECT_TRUE(w == 5);
    STATIC_EXPECT_FALSE(w == 4);
}

TEST(EqualityBool, NonBoolResult_Absent)
{
    STATIC_EXPECT_FALSE((eq_ok<wrapper<eq_not_bool, plain_exec>, int>));
    STATIC_EXPECT_FALSE((eq_ok<int, wrapper<eq_not_bool, plain_exec>>));
}

TEST(EqualityBool, ExplicitBoolResult_Present_ReturnsBool)
{
    constexpr wrapper<eq_explicit_bool, plain_exec> w{7};
    STATIC_EXPECT_TRUE((eq_ok<wrapper<eq_explicit_bool, plain_exec>, int>));
    STATIC_EXPECT_TRUE((::std::same_as<decltype(w == 7), bool>));
    STATIC_EXPECT_TRUE(w == 7);
    STATIC_EXPECT_FALSE(w == 6);
}

TEST(EqualityBool, FreePath_ReadsOperandUnderGuard)
{
    reset_guard_counters();
    wrapper<eq_bool, guarding_exec> w{5};

    bool const equal = (w == 5);

    EXPECT_TRUE(equal);
    EXPECT_EQ(g_guard, 1);
    EXPECT_EQ(g_unguard, 1);
    EXPECT_FALSE(g_accessed_unguarded);
}

// ============================================================================
// Reverse-friend path (x == w)
// ============================================================================

TEST(EqualityBool, Reverse_BoolResult_ReturnsBool)
{
    constexpr wrapper<eq_bool, plain_exec> w{5};
    STATIC_EXPECT_TRUE((eq_ok<int, wrapper<eq_bool, plain_exec>>));
    STATIC_EXPECT_TRUE((::std::same_as<decltype(5 == w), bool>));
    STATIC_EXPECT_TRUE(5 == w);
    STATIC_EXPECT_FALSE(4 == w);
}

TEST(EqualityBool, ReversePath_ReadsOperandUnderGuard)
{
    reset_guard_counters();
    wrapper<eq_bool, guarding_exec> w{5};

    bool const equal = (5 == w);

    EXPECT_TRUE(equal);
    EXPECT_EQ(g_guard, 1);
    EXPECT_EQ(g_unguard, 1);
    EXPECT_FALSE(g_accessed_unguarded);
}

// ============================================================================
// Executor-override path (Executor::operator_equal_to)
// ============================================================================

TEST(EqualityBool, Override_BoolResult_ReturnsBool)
{
    wrapper<no_eq, eq_override_bool_exec> w{0};
    STATIC_EXPECT_TRUE((eq_ok<wrapper<no_eq, eq_override_bool_exec>, int>));
    STATIC_EXPECT_TRUE((::std::same_as<decltype(w == 42), bool>));
    EXPECT_TRUE(w == 42);
    EXPECT_FALSE(w == 1);
}

TEST(EqualityBool, Override_NonBoolResult_Absent)
{
    STATIC_EXPECT_FALSE((eq_ok<wrapper<no_eq, eq_override_not_bool_exec>, int>));
}
