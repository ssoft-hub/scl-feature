#include <gtest_utils.h>

#include <scl/feature/wrapper.h>

// ============================================================================
// Tests for reflect<Wrapper, Executor, Target> specialization.
//
// Verifies that scl::wrapper<Target> correctly forwards each cv-ref-qualified
// call to the corresponding Target overload through a plain (inplace) executor.
// ============================================================================

// Target object with various qualified methods

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

template <typename Wrapper, typename Executor>
class scl::feature::reflect<Wrapper, Executor, Target>
{
public:
    SCL_REFLECT_TYPE(Wrapper, Executor)

    SCL_REFLECT_METHOD(get)
    SCL_REFLECT_METHOD(set)
};

// ============================================================================
// Custom wrapper — same reflect<Wrapper, Executor, Target> specialization,
// but accessed through a hand-rolled wrapper instead of scl::wrapper.
//
// Verifies that the reflection machinery is not coupled to scl::wrapper:
// any class that inherits from reflect<>, carries an executor, and provides
// an executor_trait specialization gets the same proxy methods.
// ============================================================================

namespace
{
    struct ReflectedTarget
        : ::scl::feature::reflect<ReflectedTarget, ::scl::feature::inplace::plain<Target>, Target>
    {
        using executor_type = ::scl::feature::inplace::plain<Target>;

        executor_type m_executor;

        SCL_REFLECT_TYPE(ReflectedTarget, executor_type)

        explicit constexpr ReflectedTarget(Target t)
            : m_executor{::std::move(t)}
        {}
    };
} // namespace

template <>
struct scl::feature::executor_trait<ReflectedTarget>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self) noexcept
    {
        return ::scl::forward_like<Self>(self.m_executor);
    }
};

// ============================================================================
// Tests for reflected get() via reflect<Wrapper, Executor, Target>
// ============================================================================

TEST(ReflectGetMethod, MutableLValueReturnsShort)
{
    using Reflected = scl::wrapper<Target>;

    Reflected r{Target{42}};
    auto result = r.get();

    static_assert(::std::same_as<decltype(result), short>,
        "reflected get() & must return short — Target::get() & return type");

    EXPECT_EQ(result, 42);
}

TEST(ReflectGetMethod, ConstLValueReturnsInt)
{
    using Reflected = scl::wrapper<Target>;

    Reflected const r{Target{42}};
    auto result = r.get();

    static_assert(::std::same_as<decltype(result), int>,
        "reflected get() const& must return int — Target::get() const& returns value + 50");

    EXPECT_EQ(result, 92);
}

TEST(ReflectGetMethod, RValueReturnsFloat)
{
    using Reflected = scl::wrapper<Target>;

    static_assert(::std::same_as<decltype(Reflected{Target{42}}.get()), float>,
        "reflected get() && must return float — Target::get() && returns value + 100");

    auto result = Reflected{Target{42}}.get();
    EXPECT_FLOAT_EQ(result, 142.0f);
}

TEST(ReflectGetMethod, ConstRValueReturnsDouble)
{
    using Reflected = scl::wrapper<Target>;

    auto result = ::std::move(static_cast<Reflected const &>(Reflected{Target{42}})).get();

    static_assert(::std::same_as<decltype(result), double>,
        "reflected get() const&& must return double — Target::get() const&& returns value + 200");

    EXPECT_DOUBLE_EQ(result, 242.0);
}

TEST(ReflectGetMethod, ConstexprConstLValue)
{
    using Reflected = scl::wrapper<Target>;

    constexpr Reflected r{Target{10}};
    STATIC_EXPECT_EQ(r.get(), 60);
}

TEST(ReflectGetMethod, ConstexprRValue)
{
    using Reflected = scl::wrapper<Target>;

    STATIC_EXPECT_EQ(Reflected{Target{10}}.get(), 110.0f);
}

// ============================================================================
// Tests for reflected set() via reflect<Wrapper, Executor, Target>
// ============================================================================

TEST(ReflectSetMethod, SetsThroughWrapper)
{
    using Reflected = scl::wrapper<Target>;

    Reflected r{Target{0}};
    r.set(short{99});

    EXPECT_EQ(r.get(), short{99});
}

TEST(ReflectSetMethod, SetWithWrapperArgument)
{
    using Reflected = scl::wrapper<Target>;

    Reflected r{Target{0}};
    r.set(::scl::wrapper<short>{short{77}});

    EXPECT_EQ(r.get(), short{77});
}

template <typename T, typename... Args>
constexpr bool can_call_set_v = requires { ::std::declval<T>().set(::std::declval<Args>()...); };

TEST(ReflectSetMethod, AvailableOnLValue)
{
    using Reflected = scl::wrapper<Target>;

    STATIC_EXPECT_TRUE((can_call_set_v<Reflected &, short>));
}

TEST(ReflectSetMethod, NotAvailableOnConst)
{
    using Reflected = scl::wrapper<Target>;

    STATIC_EXPECT_FALSE((can_call_set_v<Reflected const &, short>));
}

TEST(ReflectSetMethod, NotAvailableOnRValue)
{
    using Reflected = scl::wrapper<Target>;

    STATIC_EXPECT_FALSE((can_call_set_v<Reflected &&, short>));
}

// ============================================================================
// Custom wrapper — tests for reflected get()
// ============================================================================

TEST(CustomWrapperReflectGet, MutableLValueReturnsShort)
{
    ReflectedTarget r{Target{42}};
    auto result = r.get();

    static_assert(::std::same_as<decltype(result), short>,
        "reflected get() & must return short — Target::get() & return type");

    EXPECT_EQ(result, 42);
}

TEST(CustomWrapperReflectGet, ConstLValueReturnsInt)
{
    ReflectedTarget const r{Target{42}};
    auto result = r.get();

    static_assert(::std::same_as<decltype(result), int>,
        "reflected get() const& must return int — Target::get() const& returns value + 50");

    EXPECT_EQ(result, 92);
}

TEST(CustomWrapperReflectGet, RValueReturnsFloat)
{
    static_assert(::std::same_as<decltype(ReflectedTarget{Target{42}}.get()), float>,
        "reflected get() && must return float — Target::get() && returns value + 100");

    auto result = ReflectedTarget{Target{42}}.get();
    EXPECT_FLOAT_EQ(result, 142.0f);
}

TEST(CustomWrapperReflectGet, ConstRValueReturnsDouble)
{
    using R = ReflectedTarget;
    auto result = ::std::move(static_cast<R const &>(R{Target{42}})).get();

    static_assert(::std::same_as<decltype(result), double>,
        "reflected get() const&& must return double — Target::get() const&& returns value + 200");

    EXPECT_DOUBLE_EQ(result, 242.0);
}

TEST(CustomWrapperReflectGet, ConstexprConstLValue)
{
    constexpr ReflectedTarget r{Target{10}};
    STATIC_EXPECT_EQ(r.get(), 60);
}

TEST(CustomWrapperReflectGet, ConstexprRValue)
{
    STATIC_EXPECT_EQ(ReflectedTarget{Target{10}}.get(), 110.0f);
}

// ============================================================================
// Custom wrapper — tests for reflected set()
// ============================================================================

TEST(CustomWrapperReflectSet, SetsThroughWrapper)
{
    ReflectedTarget r{Target{0}};
    r.set(short{99});

    EXPECT_EQ(r.get(), short{99});
}

TEST(CustomWrapperReflectSet, AvailableOnLValue)
{
    STATIC_EXPECT_TRUE((can_call_set_v<ReflectedTarget &, short>));
}

TEST(CustomWrapperReflectSet, NotAvailableOnConst)
{
    STATIC_EXPECT_FALSE((can_call_set_v<ReflectedTarget const &, short>));
}

TEST(CustomWrapperReflectSet, NotAvailableOnRValue)
{
    STATIC_EXPECT_FALSE((can_call_set_v<ReflectedTarget &&, short>));
}
