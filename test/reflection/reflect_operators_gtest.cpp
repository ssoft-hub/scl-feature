#include <gtest_utils.h>

#include <scl/feature/reflection/reflect.h>
#include <scl/feature/type_traits/executor.h>
#include <scl/utility/type_traits/forward_like.h>

#include <utility>

// ============================================================================
// FullTarget — defines every operator used by reflect_operators
//
// Only one qualifier per operator:
//   const& for read-only ops (arithmetic, comparison, bitwise, logical, unary,
//           subscript, call, indirection, comma, ->*)
//   &       for mutation ops (prefix/postfix inc/dec, assignment, address-of, ->)
//
// SCL_HAS_QUALIFIED_METHOD detects a dedicated overload by comparing return
// types of calls through different qualifiers. Using a single qualifier per
// operator avoids the return-type-equality false-negative: if only const& is
// defined, can_lv_cv is false, so the macro returns true without needing
// distinct types. Same logic applies to mutable-only ops.
// ============================================================================

struct FullTarget
{
    int value = 0;

    // --- Binary arithmetic (const& only) ---
    constexpr int operator+(int rhs) const & noexcept { return value + rhs; }
    constexpr int operator-(int rhs) const & noexcept { return value - rhs; }
    constexpr int operator*(int rhs) const & noexcept { return value * rhs; }
    constexpr int operator/(int rhs) const & noexcept { return value / rhs; }
    constexpr int operator%(int rhs) const & noexcept { return value % rhs; }

    // --- Shift (const& only) ---
    constexpr int operator<<(int rhs) const & noexcept { return value << rhs; }
    constexpr int operator>>(int rhs) const & noexcept { return value >> rhs; }

    // --- Bitwise binary (const& only) ---
    constexpr int operator&(int rhs) const & noexcept { return value & rhs; }
    constexpr int operator|(int rhs) const & noexcept { return value | rhs; }
    constexpr int operator^(int rhs) const & noexcept { return value ^ rhs; }

    // --- Logical (const& only) ---
    constexpr bool operator&&(bool rhs) const & noexcept { return static_cast<bool>(value) && rhs; }
    constexpr bool operator||(bool rhs) const & noexcept { return static_cast<bool>(value) || rhs; }

    // --- Comparison (const& only) ---
    constexpr bool operator==(int rhs) const & noexcept { return value == rhs; }
    constexpr bool operator!=(int rhs) const & noexcept { return value != rhs; }
    constexpr bool operator<(int rhs) const & noexcept { return value < rhs; }
    constexpr bool operator<=(int rhs) const & noexcept { return value <= rhs; }
    constexpr bool operator>(int rhs) const & noexcept { return value > rhs; }
    constexpr bool operator>=(int rhs) const & noexcept { return value >= rhs; }

    // --- Unary prefix (const& only) ---
    constexpr int operator+() const & noexcept { return +value; }
    constexpr int operator-() const & noexcept { return -value; }
    constexpr int operator~() const & noexcept { return ~value; }
    constexpr bool operator!() const & noexcept { return !value; }

    // --- Indirection: returns held value (const& only) ---
    constexpr int operator*() const & noexcept { return value; }

    // --- Address-of: returns pointer to value field (& only) ---
    constexpr int * operator&() & noexcept { return &value; }

    // --- Arrow: pointer to self (& only — for member access via ->) ---
    constexpr FullTarget * operator->() & noexcept { return this; }

    // --- Arrow-to-pointer (const& only) ---
    constexpr int operator->*(int FullTarget::* pm) const & noexcept { return this->*pm; }

    // --- Prefix inc/dec (& only) ---
    constexpr int operator++() & noexcept { return ++value; }
    constexpr int operator--() & noexcept { return --value; }

    // --- Postfix inc/dec (& only) ---
    constexpr int operator++(int) & noexcept { return value++; }
    constexpr int operator--(int) & noexcept { return value--; }

    // --- Compound assignment (& only) ---
    constexpr int operator=(int rhs) & noexcept { return (value = rhs); }
    constexpr int operator+=(int rhs) & noexcept { return (value += rhs); }
    constexpr int operator-=(int rhs) & noexcept { return (value -= rhs); }
    constexpr int operator*=(int rhs) & noexcept { return (value *= rhs); }
    constexpr int operator/=(int rhs) & noexcept { return (value /= rhs); }
    constexpr int operator%=(int rhs) & noexcept { return (value %= rhs); }
    constexpr int operator<<=(int rhs) & noexcept { return (value <<= rhs); }
    constexpr int operator>>=(int rhs) & noexcept { return (value >>= rhs); }
    constexpr int operator&=(int rhs) & noexcept { return (value &= rhs); }
    constexpr int operator|=(int rhs) & noexcept { return (value |= rhs); }
    constexpr int operator^=(int rhs) & noexcept { return (value ^= rhs); }

    // --- Subscript (const& only) ---
    constexpr int operator[](int idx) const & noexcept { return value + idx; }

    // --- Call (const& only) ---
    constexpr int operator()() const & noexcept { return value; }
    constexpr int operator()(int x) const & noexcept { return value + x; }

    // --- Comma: returns rhs (const& only) ---
    constexpr int operator,(int rhs) const & noexcept { return rhs; }
};

// ============================================================================
// Executor and wrapper
// ============================================================================

struct FullExecutor
{
    FullTarget m_value;

    template <typename Self, typename Func, typename... Args>
    static constexpr decltype(auto) execute(Self &&, Func && func, Args &&... args)
    {
        return ::std::forward<Func>(func)(::std::forward<Args>(args)...);
    }

    template <typename Self>
    static constexpr decltype(auto) access(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_value);
    }
};

struct FullWrapper;

template <>
struct scl::feature::executor_trait<FullWrapper>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self)
    {
        return ::scl::forward_like<Self>(self.m_exec);
    }
};

struct FullWrapper : scl::feature::reflect_operators<FullWrapper, FullExecutor>
{
    using scl::feature::reflect_operators<FullWrapper, FullExecutor>::operator=;

    FullExecutor m_exec;

    constexpr explicit FullWrapper(int v)
        : m_exec{FullTarget{v}}
    {}
};

// Read held value via const call operator (no-arg overload of FullTarget::operator())
static int val(FullWrapper const & w) { return w(); }

// ============================================================================
// Tests — Binary Arithmetic (const wrapper)
// ============================================================================

TEST(ReflectAllOps, Plus)
{
    constexpr FullWrapper w{10};
    STATIC_EXPECT_EQ(w + 3, 13);
}
TEST(ReflectAllOps, Minus)
{
    constexpr FullWrapper w{10};
    STATIC_EXPECT_EQ(w - 3, 7);
}
TEST(ReflectAllOps, Multiply)
{
    constexpr FullWrapper w{6};
    STATIC_EXPECT_EQ(w * 4, 24);
}
TEST(ReflectAllOps, Divide)
{
    constexpr FullWrapper w{20};
    STATIC_EXPECT_EQ(w / 4, 5);
}
TEST(ReflectAllOps, Modulo)
{
    constexpr FullWrapper w{11};
    STATIC_EXPECT_EQ(w % 4, 3);
}

// ============================================================================
// Tests — Shift (const wrapper)
// ============================================================================

TEST(ReflectAllOps, LeftShift)
{
    constexpr FullWrapper w{3};
    STATIC_EXPECT_EQ(w << 2, 12);
}
TEST(ReflectAllOps, RightShift)
{
    constexpr FullWrapper w{12};
    STATIC_EXPECT_EQ(w >> 2, 3);
}

// ============================================================================
// Tests — Bitwise Binary (const wrapper)
// ============================================================================

TEST(ReflectAllOps, BitwiseAnd)
{
    constexpr FullWrapper w{0b1010};
    STATIC_EXPECT_EQ(w & 0b1100, 0b1000);
}
TEST(ReflectAllOps, BitwiseOr)
{
    constexpr FullWrapper w{0b1010};
    STATIC_EXPECT_EQ(w | 0b1100, 0b1110);
}
TEST(ReflectAllOps, BitwiseXor)
{
    constexpr FullWrapper w{0b1010};
    STATIC_EXPECT_EQ(w ^ 0b1100, 0b0110);
}

// ============================================================================
// Tests — Logical (const wrapper)
// ============================================================================

TEST(ReflectAllOps, LogicalAnd_True)
{
    constexpr FullWrapper w{1};
    STATIC_EXPECT_TRUE(w && true);
}
TEST(ReflectAllOps, LogicalAnd_False)
{
    constexpr FullWrapper w{0};
    STATIC_EXPECT_FALSE(w && true);
}
TEST(ReflectAllOps, LogicalOr_True)
{
    constexpr FullWrapper w{1};
    STATIC_EXPECT_TRUE(w || false);
}
TEST(ReflectAllOps, LogicalOr_False)
{
    constexpr FullWrapper w{0};
    STATIC_EXPECT_FALSE(w || false);
}

// ============================================================================
// Tests — Comparison (const wrapper)
// ============================================================================

TEST(ReflectAllOps, EqualTo_True)
{
    constexpr FullWrapper w{5};
    STATIC_EXPECT_TRUE(w == 5);
}
TEST(ReflectAllOps, EqualTo_False)
{
    constexpr FullWrapper w{5};
    STATIC_EXPECT_FALSE(w == 4);
}
TEST(ReflectAllOps, NotEqualTo_True)
{
    constexpr FullWrapper w{5};
    STATIC_EXPECT_TRUE(w != 4);
}
TEST(ReflectAllOps, Less_Eq)
{
    constexpr FullWrapper w{5};
    STATIC_EXPECT_TRUE(w < 6);
}
TEST(ReflectAllOps, LessEqual_Eq)
{
    constexpr FullWrapper w{5};
    STATIC_EXPECT_TRUE(w <= 5);
}
TEST(ReflectAllOps, LessEqual_Less)
{
    constexpr FullWrapper w{4};
    STATIC_EXPECT_TRUE(w <= 5);
}
TEST(ReflectAllOps, Greater_True)
{
    constexpr FullWrapper w{5};
    STATIC_EXPECT_TRUE(w > 4);
}
TEST(ReflectAllOps, Greater_False)
{
    constexpr FullWrapper w{5};
    STATIC_EXPECT_FALSE(w > 5);
}
TEST(ReflectAllOps, GreaterEqual_Eq)
{
    constexpr FullWrapper w{5};
    STATIC_EXPECT_TRUE(w >= 5);
}

// ============================================================================
// Tests — Unary Prefix (const wrapper)
// ============================================================================

TEST(ReflectAllOps, UnaryPlus)
{
    constexpr FullWrapper w{7};
    STATIC_EXPECT_EQ(+w, 7);
}
TEST(ReflectAllOps, UnaryMinus)
{
    constexpr FullWrapper w{7};
    STATIC_EXPECT_EQ(-w, -7);
}
TEST(ReflectAllOps, BitwiseNot)
{
    constexpr FullWrapper w{0};
    STATIC_EXPECT_EQ(~w, ~0);
}
TEST(ReflectAllOps, LogicalNot_Zero)
{
    constexpr FullWrapper w{0};
    STATIC_EXPECT_TRUE(!w);
}
TEST(ReflectAllOps, LogicalNot_Nonzero)
{
    constexpr FullWrapper w{1};
    STATIC_EXPECT_FALSE(!w);
}

// ============================================================================
// Tests — Indirection (const wrapper)
// ============================================================================

TEST(ReflectAllOps, Indirection)
{
    constexpr FullWrapper w{42};
    STATIC_EXPECT_EQ(*w, 42);
}

// ============================================================================
// Tests — Address-of (mutable wrapper)
// ============================================================================

TEST(ReflectAllOps, AddressOf)
{
    FullWrapper w{99};
    int * p = w.operator&();
    EXPECT_EQ(*p, 99);
}

// ============================================================================
// Tests — Arrow (mutable wrapper)
// ============================================================================

TEST(ReflectAllOps, Arrow)
{
    FullWrapper w{55};
    EXPECT_EQ(w->value, 55);
}

// ============================================================================
// Tests — Arrow-to-pointer (const wrapper)
// ============================================================================

TEST(ReflectAllOps, ArrowToPointer)
{
    constexpr FullWrapper w{7};
    constexpr int FullTarget::* pm = &FullTarget::value;
    STATIC_EXPECT_EQ(w->*pm, 7);
}

// ============================================================================
// Tests — Subscript (const wrapper)
// ============================================================================

TEST(ReflectAllOps, Subscript_ZeroIdx)
{
    constexpr FullWrapper w{10};
    STATIC_EXPECT_EQ(w[0], 10);
}
TEST(ReflectAllOps, Subscript_PositiveIdx)
{
    constexpr FullWrapper w{10};
    STATIC_EXPECT_EQ(w[3], 13);
}
TEST(ReflectAllOps, Subscript_NegativeIdx)
{
    constexpr FullWrapper w{10};
    STATIC_EXPECT_EQ(w[-2], 8);
}

// ============================================================================
// Tests — Call (const wrapper)
// ============================================================================

TEST(ReflectAllOps, CallNoArg)
{
    constexpr FullWrapper w{7};
    STATIC_EXPECT_EQ(w(), 7);
}
TEST(ReflectAllOps, CallOneArg)
{
    constexpr FullWrapper w{7};
    STATIC_EXPECT_EQ(w(3), 10);
}

// ============================================================================
// Tests — Comma (const wrapper)
// ============================================================================

TEST(ReflectAllOps, Comma)
{
    constexpr FullWrapper w{10};
    constexpr auto result = (w, 42);
    STATIC_EXPECT_EQ(result, 42);
}

// ============================================================================
// Tests — Prefix Increment / Decrement (mutable wrapper)
// ============================================================================

TEST(ReflectAllOps, PrefixIncrement)
{
    FullWrapper w{5};
    EXPECT_EQ(++w, 6);
    EXPECT_EQ(val(w), 6);
}

TEST(ReflectAllOps, PrefixDecrement)
{
    FullWrapper w{5};
    EXPECT_EQ(--w, 4);
    EXPECT_EQ(val(w), 4);
}

// ============================================================================
// Tests — Postfix Increment / Decrement (mutable wrapper)
// ============================================================================

TEST(ReflectAllOps, PostfixIncrement)
{
    FullWrapper w{5};
    EXPECT_EQ(w++, 5);
    EXPECT_EQ(val(w), 6);
}

TEST(ReflectAllOps, PostfixDecrement)
{
    FullWrapper w{5};
    EXPECT_EQ(w--, 5);
    EXPECT_EQ(val(w), 4);
}

// ============================================================================
// Tests — Assignment Operators (mutable wrapper)
// ============================================================================

TEST(ReflectAllOps, Assign)
{
    FullWrapper w{0};
    EXPECT_EQ(w = 42, 42);
    EXPECT_EQ(val(w), 42);
}

TEST(ReflectAllOps, PlusAssign)
{
    FullWrapper w{10};
    EXPECT_EQ(w += 5, 15);
    EXPECT_EQ(val(w), 15);
}

TEST(ReflectAllOps, MinusAssign)
{
    FullWrapper w{10};
    EXPECT_EQ(w -= 3, 7);
    EXPECT_EQ(val(w), 7);
}

TEST(ReflectAllOps, MultiplyAssign)
{
    FullWrapper w{4};
    EXPECT_EQ(w *= 3, 12);
    EXPECT_EQ(val(w), 12);
}

TEST(ReflectAllOps, DivideAssign)
{
    FullWrapper w{12};
    EXPECT_EQ(w /= 4, 3);
    EXPECT_EQ(val(w), 3);
}

TEST(ReflectAllOps, ModuloAssign)
{
    FullWrapper w{11};
    EXPECT_EQ(w %= 4, 3);
    EXPECT_EQ(val(w), 3);
}

TEST(ReflectAllOps, LeftShiftAssign)
{
    FullWrapper w{3};
    EXPECT_EQ(w <<= 2, 12);
    EXPECT_EQ(val(w), 12);
}

TEST(ReflectAllOps, RightShiftAssign)
{
    FullWrapper w{12};
    EXPECT_EQ(w >>= 2, 3);
    EXPECT_EQ(val(w), 3);
}

TEST(ReflectAllOps, BitwiseAndAssign)
{
    FullWrapper w{0b1010};
    EXPECT_EQ(w &= 0b1100, 0b1000);
    EXPECT_EQ(val(w), 0b1000);
}

TEST(ReflectAllOps, BitwiseOrAssign)
{
    FullWrapper w{0b1010};
    EXPECT_EQ(w |= 0b1100, 0b1110);
    EXPECT_EQ(val(w), 0b1110);
}

TEST(ReflectAllOps, BitwiseXorAssign)
{
    FullWrapper w{0b1010};
    EXPECT_EQ(w ^= 0b1100, 0b0110);
    EXPECT_EQ(val(w), 0b0110);
}

// ============================================================================
// Callability predicates — required because MSVC partially validates the body
// of inline requires{} expressions when they appear inside runtime macros
// (e.g. EXPECT_FALSE). Pre-computing in variable templates avoids this.
// ============================================================================

template <typename T>
constexpr bool can_prefix_inc_v = requires { ++::std::declval<T>(); };

template <typename T>
constexpr bool can_postfix_inc_v = requires { ::std::declval<T>()++; };

template <typename T>
constexpr bool can_assign_int_v = requires { ::std::declval<T>() = 5; };

template <typename T>
constexpr bool can_address_of_v = requires { ::std::declval<T>().operator&(); };

template <typename T>
constexpr bool can_arrow_v = requires { ::std::declval<T>().operator->(); };

// ============================================================================
// Tests — Negative: mutation operators not callable on const wrapper
// ============================================================================

TEST(ReflectAllOps, PrefixIncNotAvailableOnConst)
{
    STATIC_EXPECT_FALSE(can_prefix_inc_v<FullWrapper const &>);
}

TEST(ReflectAllOps, PostfixIncNotAvailableOnConst)
{
    STATIC_EXPECT_FALSE(can_postfix_inc_v<FullWrapper const &>);
}

TEST(ReflectAllOps, AssignNotAvailableOnConst)
{
    STATIC_EXPECT_FALSE(can_assign_int_v<FullWrapper const &>);
}

TEST(ReflectAllOps, AddressOfNotAvailableOnConst)
{
    STATIC_EXPECT_FALSE(can_address_of_v<FullWrapper const &>);
}

TEST(ReflectAllOps, ArrowNotAvailableOnConst)
{
    STATIC_EXPECT_FALSE(can_arrow_v<FullWrapper const &>);
}
