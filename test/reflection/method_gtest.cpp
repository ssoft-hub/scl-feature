#include <gtest_utils.h>

#include <scl/feature/reflection/method.h>
#include <scl/feature/type_traits/executor.h>
#include <scl/utility/type_traits/forward_like.h>

#include <utility>

// ============================================================================
// Tests for SCL_REFLECT_METHOD (full reflection through adapter)
// ============================================================================

// Target object with various qualified methods
struct Target
{
    int value = 0;

    template <typename T>
    constexpr T convert() const &
    {
        return static_cast<T>(value);
    }

    constexpr short get() & { return convert<short>(); }
    constexpr int get() const & { return convert<int>() + 50; }
    constexpr float get() && { return convert<float>() + 100; }
    constexpr double get() const && { return convert<double>() + 200; }

    constexpr void set(short v) & { value = v; }
};

struct TestExecutor
{
    Target m_value;

    template <typename Self, typename Func, typename... Args>
    static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args)
    {
        return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
    }

    template <typename Self>
    static constexpr decltype(auto) value(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_value);
    }
};

struct ReflectedTarget;

// Specialize executor_trait for ReflectedTarget (uses m_impl, not m_executor)
template <>
struct scl::feature::executor_trait<ReflectedTarget>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_impl);
    }
};

// Adapter that holds an executor and reflects Target methods
struct ReflectedTarget
{
    TestExecutor m_impl;

    SCL_REFLECT_TYPE(ReflectedTarget, m_impl);

    constexpr explicit ReflectedTarget(int v)
        : m_impl{v}
    {}

    SCL_REFLECT_METHOD(get)
    SCL_REFLECT_METHOD(set)
    SCL_REFLECT_METHOD(convert)
};

TEST(ReflectMethod, GetFromMutableLValue)
{
    ReflectedTarget r{42};
    EXPECT_EQ(r.get(), 42);
}

TEST(ReflectMethod, GetFromConstLValue)
{
    ReflectedTarget const r{42};
    EXPECT_EQ(r.get(), 92);
}

TEST(ReflectMethod, GetFromConstexprValue)
{
    constexpr ReflectedTarget r{42};
    STATIC_EXPECT_EQ(r.get(), 92);
}

TEST(ReflectMethod, GetFromRValue)
{
    // constexpr
    STATIC_EXPECT_EQ(ReflectedTarget{42}.get(), 142);
}

TEST(ReflectMethod, GetFromConstRValue)
{
    // constexpr
    STATIC_EXPECT_EQ(::std::move(static_cast<ReflectedTarget const &>(ReflectedTarget{42})).get(), 242);
}

TEST(ReflectMethod, SetThroughReflection)
{
    ReflectedTarget r{0};
    r.set(99);
    EXPECT_EQ(r.get(), 99);
}

template <typename T, typename... Args>
constexpr bool can_call_set_v = requires { ::std::declval<T>().set(::std::declval<Args>()...); };

TEST(ReflectMethod, SetNotAvailableOnConst)
{
    // set() is only available on mutable lvalue — const should not compile
    STATIC_EXPECT_FALSE((can_call_set_v<ReflectedTarget const &, short>));
}

TEST(ReflectMethod, SetNotAvailableOnRValue)
{
    STATIC_EXPECT_FALSE((can_call_set_v<ReflectedTarget &&, short>));
}

TEST(ReflectMethod, TemplateMethod)
{
    ReflectedTarget const r{42};
    EXPECT_DOUBLE_EQ(r.convert<double>(), 42.0);
}
