#include <gtest_utils.h>

#include <scl/feature/reflection/method.h>
#include <scl/feature/type_traits/executor.h>
#include <scl/feature/wrapper.h>
#include <scl/utility/type_traits/forward_like.h>

#include <type_traits>
#include <utility>

// ============================================================================
// Shared target type
// ============================================================================

namespace
{
    /// Target with distinct return types per qualifier (required by SCL_REFLECT_METHOD).
    struct Target
    {
        int value = 0;

        constexpr short get() & { return static_cast<short>(value); }
        constexpr int get() const & { return value + 50; }
        constexpr float get() && { return static_cast<float>(value) + 100.0f; }
        constexpr double get() const && { return static_cast<double>(value) + 200.0; }

        constexpr void set(short v) & { value = v; }
    };
} // namespace

// ============================================================================
// Fixture: execute return-type test
//
// An executor whose execute() wraps every non-void result in Box<T>.
// The reflected method must return Box<T>, not T.
// ============================================================================

template <typename T>
struct Box
{
    T data{};

    constexpr bool operator==(Box const &) const noexcept = default;
};

template <>
struct Box<void>
{
    constexpr bool operator==(Box const &) const noexcept = default;
};

struct BoxingExecutor
{
    Target m_value;

    template <typename Self, typename Func, typename... Args>
    static constexpr auto execute(Self &&, Func && func, Args &&... args)
    {
        if constexpr (::std::is_void_v<::std::invoke_result_t<Func, Args...>>)
        {
            ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
            return Box<void>{};
        }
        else
        {
            return Box{::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...)};
        }
    }

    template <typename Self>
    static constexpr decltype(auto) value(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_value);
    }
};

struct BoxingReflected;

template <>
struct scl::feature::executor_trait<BoxingReflected>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_exec);
    }
};

struct BoxingReflected
{
    BoxingExecutor m_exec;

    SCL_REFLECT_TYPE(BoxingReflected, m_exec);

    constexpr explicit BoxingReflected(int v)
        : m_exec{v}
    {}

    SCL_REFLECT_METHOD(get)
    SCL_REFLECT_METHOD(set)
};

// ============================================================================
// Fixture: wrapper-as-argument test
//
// A plain pass-through executor. The reflected set() must accept
// scl::wrapper<short> wherever a raw short is expected via wrapper_cast.
// ============================================================================

struct PlainReflectedExecutor
{
    Target m_value;

    template <typename Self, typename Func, typename... Args>
    static constexpr auto execute(Self &&, Func && func, Args &&... args)
    {
        return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
    }

    template <typename Self>
    static constexpr decltype(auto) value(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_value);
    }
};

struct PlainReflected;

template <>
struct scl::feature::executor_trait<PlainReflected>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_exec);
    }
};

struct PlainReflected
{
    PlainReflectedExecutor m_exec;

    SCL_REFLECT_TYPE(PlainReflected, m_exec);

    constexpr explicit PlainReflected(int v)
        : m_exec{v}
    {}

    SCL_REFLECT_METHOD(get)
    SCL_REFLECT_METHOD(set)
};

// ============================================================================
// Helper: call-site concept check
// ============================================================================

template <typename T, typename... Args>
constexpr bool can_call_set_v = requires { ::std::declval<T>().set(::std::declval<Args>()...); };

// ============================================================================
// Tests — reflected method returns execute()'s result, not the wrapped type
// ============================================================================

// execute() of BoxingExecutor wraps the result in Box<T>.
// The reflected get() & must return Box<short> (not short).

TEST(ReflectMethodExecuteResult, MutableLValueReturnsBoxedType)
{
    BoxingReflected r{42};
    auto result = r.get();

    static_assert(::std::same_as<decltype(result), Box<short>>,
        "reflected get() & must return Box<short> — the result of execute(), not short");

    EXPECT_EQ(result.data, 42);
}

TEST(ReflectMethodExecuteResult, ConstLValueReturnsBoxedType)
{
    BoxingReflected const r{42};
    auto result = r.get();

    static_assert(::std::same_as<decltype(result), Box<int>>,
        "reflected get() const& must return Box<int> — the result of execute(), not int");

    EXPECT_EQ(result.data, 92);
}

TEST(ReflectMethodExecuteResult, RValueReturnsBoxedType)
{
    // constexpr check: rvalue overload returns Box<float>
    static_assert(::std::same_as<decltype(BoxingReflected{42}.get()), Box<float>>,
        "reflected get() && must return Box<float> — the result of execute(), not float");

    auto result = BoxingReflected{42}.get();
    EXPECT_FLOAT_EQ(result.data, 142.0f);
}

TEST(ReflectMethodExecuteResult, ReturnTypeIsNotOriginalWrappedType)
{
    BoxingReflected r{0};

    STATIC_EXPECT_FALSE((::std::same_as<decltype(r.get()), short>));
    STATIC_EXPECT_TRUE((::std::same_as<decltype(r.get()), Box<short>>));
}

// execute() of BoxingExecutor returns Box<void> for void-returning methods.
// The reflected set() must return Box<void>, not void.

TEST(ReflectMethodExecuteResult, SetReturnsBoxVoid)
{
    BoxingReflected r{0};

    STATIC_EXPECT_TRUE((::std::same_as<decltype(r.set(short{0})), Box<void>>));
}

TEST(ReflectMethodExecuteResult, SetWithRawShortGoesToExecute)
{
    BoxingReflected r{0};
    auto result = r.set(short{42});

    static_assert(::std::same_as<decltype(result), Box<void>>,
        "reflected set() & must return Box<void> — the result of execute()");

    auto got = r.get();
    EXPECT_EQ(got.data, 42);
}

TEST(ReflectMethodExecuteResult, SetWithWrapperRValueGoesToExecute)
{
    BoxingReflected r{0};
    r.set(::scl::wrapper<short>{short{77}});
    EXPECT_EQ(r.get().data, 77);
}

TEST(ReflectMethodExecuteResult, SetWithWrapperLValueGoesToExecute)
{
    BoxingReflected r{0};
    ::scl::wrapper<short> w{short{55}};
    r.set(w);
    EXPECT_EQ(r.get().data, 55);
}

TEST(ReflectMethodExecuteResult, SetWithConstWrapperLValueGoesToExecute)
{
    BoxingReflected r{0};
    ::scl::wrapper<short> const cw{short{33}};
    r.set(cw);
    EXPECT_EQ(r.get().data, 33);
}

// ============================================================================
// Tests — reflected interface accepts scl::wrapper<T> in place of raw T
// ============================================================================

// MSVC (non-Clang frontend) triggers fatal error C1001 (internal compiler error)
// when evaluating can_call_set_v inside a requires-expression that involves the
// reflected set() signature.
#if !defined(_MSC_VER) || defined(__clang__)

TEST(ReflectMethodWrapperArg, IsCallableWithWrapperRValue)
{
    STATIC_EXPECT_TRUE((can_call_set_v<PlainReflected &, ::scl::wrapper<short>>));
}

TEST(ReflectMethodWrapperArg, IsCallableWithWrapperLValue)
{
    STATIC_EXPECT_TRUE((can_call_set_v<PlainReflected &, ::scl::wrapper<short> &>));
}

TEST(ReflectMethodWrapperArg, IsCallableWithConstWrapperLValue)
{
    STATIC_EXPECT_TRUE((can_call_set_v<PlainReflected &, ::scl::wrapper<short> const &>));
}

#endif // !_MSC_VER

TEST(ReflectMethodWrapperArg, SetWithWrapperRValueSetsValue)
{
    PlainReflected r{0};
    r.set(::scl::wrapper<short>{short{99}});
    EXPECT_EQ(r.get(), 99);
}

TEST(ReflectMethodWrapperArg, SetWithWrapperLValueSetsValue)
{
    PlainReflected r{0};
    ::scl::wrapper<short> w{short{77}};
    r.set(w);
    EXPECT_EQ(r.get(), 77);
}

TEST(ReflectMethodWrapperArg, SetWithConstWrapperLValueSetsValue)
{
    PlainReflected r{0};
    ::scl::wrapper<short> const cw{short{55}};
    r.set(cw);
    EXPECT_EQ(r.get(), 55);
}

TEST(ReflectMethodWrapperArg, RawShortStillAccepted)
{
    // Passing a raw short must keep working alongside the wrapper path.
    // STATIC_EXPECT_TRUE((can_call_set_v<PlainReflected &, short>));

    PlainReflected r{0};
    r.set(short{33});
    EXPECT_EQ(r.get(), 33);
}
