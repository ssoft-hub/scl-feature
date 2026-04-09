#include <gtest_utils.h>

#include <scl/feature/type_traits/has_qualified_method.h>

// ---------------------------------------------------------------------------
// Helper: class with methods qualified in various ways
// ---------------------------------------------------------------------------

struct Qualified
{
    int lvalue_only() & { return {}; }
    int const_lvalue_only() const & { return {}; }
    int volatile_lvalue_only() volatile & { return {}; }
    int cv_lvalue_only() const volatile & { return {}; }

    int rvalue_only() && { return {}; }
    int const_rvalue_only() const && { return {}; }
    int volatile_rvalue_only() volatile && { return {}; }
    int cv_rvalue_only() const volatile && { return {}; }

    bool mutable_and_const() & { return {}; }
    int mutable_and_const() const & { return {}; }

    bool all_lvalue() & { return {}; }
    int all_lvalue() const & { return {}; }
    float all_lvalue() volatile & { return {}; }
    double all_lvalue() const volatile & { return {}; }

    bool all_rvalue() && { return {}; }
    int all_rvalue() const && { return {}; }
    float all_rvalue() volatile && { return {}; }
    double all_rvalue() const volatile && { return {}; }

    bool all_value() { return {}; }
    int all_value() const { return {}; }
    float all_value() volatile { return {}; }
    double all_value() const volatile { return {}; }

    bool with_args(int, double) & { return {}; }
    int with_args(int, double) const & { return {}; }
};

// ============================================================================
// Tests for SCL_HAS_QUALIFIED_METHOD
// ============================================================================

// --- lvalue_only: void lvalue_only() & ---

TEST(MethodQualifiers, MutableLValueOnly_MatchesMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(lvalue_only, Qualified &);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, MutableLValueOnly_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(lvalue_only, Qualified &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableLValueOnly_RejectsConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(lvalue_only, Qualified const &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableLValueOnly_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(lvalue_only, Qualified const &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableLValueOnly_RejectsVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(lvalue_only, Qualified volatile &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableLValueOnly_RejectsVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(lvalue_only, Qualified volatile &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableLValueOnly_RejectsCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(lvalue_only, Qualified const volatile &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableLValueOnly_RejectsCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(lvalue_only, Qualified const volatile &&);
    STATIC_EXPECT_FALSE(result);
}

// --- const_lvalue_only: void const_lvalue_only() const & ---

TEST(MethodQualifiers, ConstLValueOnly_RejectsMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_lvalue_only, Qualified &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstLValueOnly_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_lvalue_only, Qualified &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstLValueOnly_MatchesConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_lvalue_only, Qualified const &);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, ConstLValueOnly_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_lvalue_only, Qualified const &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstLValueOnly_RejectsVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_lvalue_only, Qualified volatile &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstLValueOnly_RejectsVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_lvalue_only, Qualified volatile &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstLValueOnly_RejectsCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_lvalue_only, Qualified const volatile &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstLValueOnly_RejectsCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_lvalue_only, Qualified const volatile &&);
    STATIC_EXPECT_FALSE(result);
}

// --- volatile_lvalue_only: void volatile_lvalue_only() volatile & ---

TEST(MethodQualifiers, VolatileLValueOnly_RejectsMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_lvalue_only, Qualified &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileLValueOnly_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_lvalue_only, Qualified &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileLValueOnly_RejectsConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_lvalue_only, Qualified const &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileLValueOnly_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_lvalue_only, Qualified const &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileLValueOnly_MatchesVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_lvalue_only, Qualified volatile &);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, VolatileLValueOnly_RejectsVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_lvalue_only, Qualified volatile &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileLValueOnly_RejectsCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_lvalue_only, Qualified const volatile &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileLValueOnly_RejectsCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_lvalue_only, Qualified const volatile &&);
    STATIC_EXPECT_FALSE(result);
}

// --- cv_lvalue_only: void cv_lvalue_only() const volatile & ---

TEST(MethodQualifiers, CVLValueOnly_RejectsMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_lvalue_only, Qualified &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVLValueOnly_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_lvalue_only, Qualified &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVLValueOnly_RejectsConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_lvalue_only, Qualified const &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVLValueOnly_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_lvalue_only, Qualified const &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVLValueOnly_RejectsVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_lvalue_only, Qualified volatile &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVLValueOnly_RejectsVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_lvalue_only, Qualified volatile &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVLValueOnly_MatchesCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_lvalue_only, Qualified const volatile &);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, CVLValueOnly_RejectsCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_lvalue_only, Qualified const volatile &&);
    STATIC_EXPECT_FALSE(result);
}

// --- rvalue_only: void rvalue_only() & ---

TEST(MethodQualifiers, MutableRValueOnly_RejectsMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(rvalue_only, Qualified &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableRValueOnly_MatchesMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(rvalue_only, Qualified &&);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, MutableRValueOnly_RejectsConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(rvalue_only, Qualified const &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableRValueOnly_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(rvalue_only, Qualified const &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableRValueOnly_RejectsVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(rvalue_only, Qualified volatile &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableRValueOnly_RejectsVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(rvalue_only, Qualified volatile &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableRValueOnly_RejectsCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(rvalue_only, Qualified const volatile &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableRValueOnly_RejectsCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(rvalue_only, Qualified const volatile &&);
    STATIC_EXPECT_FALSE(result);
}

// --- const_rvalue_only: void const_rvalue_only() const & ---

TEST(MethodQualifiers, ConstRValueOnly_RejectsMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_rvalue_only, Qualified &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstRValueOnly_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_rvalue_only, Qualified &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstRValueOnly_RejectsConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_rvalue_only, Qualified const &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstRValueOnly_MatchesConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_rvalue_only, Qualified const &&);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, ConstRValueOnly_RejectsVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_rvalue_only, Qualified volatile &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstRValueOnly_RejectsVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_rvalue_only, Qualified volatile &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstRValueOnly_RejectsCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_rvalue_only, Qualified const volatile &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstRValueOnly_RejectsCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_rvalue_only, Qualified const volatile &&);
    STATIC_EXPECT_FALSE(result);
}

// --- volatile_rvalue_only: void volatile_rvalue_only() volatile & ---

TEST(MethodQualifiers, VolatileRValueOnly_RejectsMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_rvalue_only, Qualified &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileRValueOnly_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_rvalue_only, Qualified &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileRValueOnly_RejectsConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_rvalue_only, Qualified const &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileRValueOnly_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_rvalue_only, Qualified const &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileRValueOnly_RejectsVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_rvalue_only, Qualified volatile &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileRValueOnly_MatchesVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_rvalue_only, Qualified volatile &&);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, VolatileRValueOnly_RejectsCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_rvalue_only, Qualified const volatile &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileRValueOnly_RejectsCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_rvalue_only, Qualified const volatile &&);
    STATIC_EXPECT_FALSE(result);
}

// --- cv_rvalue_only: void cv_rvalue_only() const volatile & ---

TEST(MethodQualifiers, CVRValueOnly_RejectsMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_rvalue_only, Qualified &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVRValueOnly_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_rvalue_only, Qualified &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVRValueOnly_RejectsConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_rvalue_only, Qualified const &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVRValueOnly_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_rvalue_only, Qualified const &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVRValueOnly_RejectsVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_rvalue_only, Qualified volatile &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVRValueOnly_RejectsVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_rvalue_only, Qualified volatile &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVRValueOnly_RejectsCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_rvalue_only, Qualified const volatile &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVRValueOnly_MatchesCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_rvalue_only, Qualified const volatile &&);
    STATIC_EXPECT_TRUE(result);
}

// --- mutable_and_const: overloaded & and const & ---

TEST(MethodQualifiers, MutableAndConst_MatchesMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(mutable_and_const, Qualified &);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, MutableAndConst_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(mutable_and_const, Qualified &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableAndConst_MatchesConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(mutable_and_const, Qualified const &);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, MutableAndConst_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(mutable_and_const, Qualified const &&);
    STATIC_EXPECT_FALSE(result);
}

// --- all_lvalue: overloaded &, const&, volatile&, const volatile& ---

TEST(MethodQualifiers, AllLValue_MatchesMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_lvalue, Qualified &);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllLValue_MatchesMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_lvalue, Qualified &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, AllLValue_MatchesConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_lvalue, Qualified const &);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllLValue_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_lvalue, Qualified const &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, AllLValue_MatchesVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_lvalue, Qualified volatile &);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllLValue_RejectsVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_lvalue, Qualified volatile &&);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, AllLValue_MatchesCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_lvalue, Qualified const volatile &);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllLValue_MatchesCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_lvalue, Qualified const volatile &&);
    STATIC_EXPECT_FALSE(result);
}

// --- all_rvalue: overloaded &&, const&&, volatile&&, const volatile&& ---

TEST(MethodQualifiers, AllRValue_RejectsMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_rvalue, Qualified &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, AllRValue_MatchesMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_rvalue, Qualified &&);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllRValue_RejectsConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_rvalue, Qualified const &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, AllRValue_MatchesConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_rvalue, Qualified const &&);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllRValue_RejectsVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_rvalue, Qualified volatile &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, AllRValue_MatchesVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_rvalue, Qualified volatile &&);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllRValue_RejectsCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_rvalue, Qualified const volatile &);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, AllRValue_MatchesCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_rvalue, Qualified const volatile &&);
    STATIC_EXPECT_TRUE(result);
}

// --- all_value ---

TEST(MethodQualifiers, AllValue_MatchesMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_value, Qualified &);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllValue_MatchesMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_value, Qualified &&);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllValue_MatchesConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_value, Qualified const &);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllValue_MatchesConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_value, Qualified const &&);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllValue_MatchesVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_value, Qualified volatile &);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllValue_MatchesVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_value, Qualified volatile &&);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllValue_MatchesCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_value, Qualified const volatile &);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllValue_MatchesCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_value, Qualified const volatile &&);
    STATIC_EXPECT_TRUE(result);
}

// --- with_args: methods with parameters ---

TEST(MethodQualifiers, WithArgs_MatchesMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(with_args, Qualified &, int, double);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, WithArgs_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(with_args, Qualified &&, int, double);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, WithArgs_MatchesConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(with_args, Qualified const &, int, double);
    STATIC_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, WithArgs_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(with_args, Qualified const &&, int, double);
    STATIC_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, WithArgs_RejectsWrongArgs)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(with_args, Qualified &, float);
    STATIC_EXPECT_FALSE(result);
}

// --- non-reference type (neither lvalue nor rvalue) always returns false ---

TEST(MethodQualifiers, NonReference_ReturnsFalse)
{
    constexpr auto lvalue_result = SCL_HAS_QUALIFIED_METHOD(lvalue_only, Qualified);
    STATIC_EXPECT_FALSE(lvalue_result);
    constexpr auto rvalue_result = SCL_HAS_QUALIFIED_METHOD(rvalue_only, Qualified);
    STATIC_EXPECT_FALSE(rvalue_result);
}

// ============================================================================
// Tests for SCL_HAS_QUALIFIED_METHOD with explicit template arguments
// (SCL_FORWARD(template method<T>) syntax)
// ============================================================================

// Helper struct: template methods with various cv-ref qualifiers.
// Return types must differ across qualifiers to allow discrimination.

struct TemplateQualified
{
    // Only & overload
    template <typename T>
    bool temp_lvalue_only(T) &
    {
        return {};
    }

    // & (bool) and const& (int) overloads
    template <typename T>
    bool temp_mut_and_const(T) &
    {
        return {};
    }
    template <typename T>
    int temp_mut_and_const(T) const &
    {
        return {};
    }

    // All four lvalue qualifiers with distinct return types
    template <typename T>
    bool temp_all_lvalue(T) &
    {
        return {};
    }
    template <typename T>
    int temp_all_lvalue(T) const &
    {
        return {};
    }
    template <typename T>
    float temp_all_lvalue(T) volatile &
    {
        return {};
    }
    template <typename T>
    double temp_all_lvalue(T) const volatile &
    {
        return {};
    }

    // Only const&& overload
    template <typename T>
    bool temp_const_rvalue_only(T) const &&
    {
        return {};
    }

    // Two template parameters
    template <typename T, typename U>
    bool temp_multi(T, U) &
    {
        return {};
    }
    template <typename T, typename U>
    int temp_multi(T, U) const &
    {
        return {};
    }
};

// --- temp_lvalue_only: only & ---

TEST(TemplateMethodQualifiers, LValueOnly_MatchesMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_lvalue_only<float>),
        TemplateQualified &, float);
    STATIC_EXPECT_TRUE(result);
}

TEST(TemplateMethodQualifiers, LValueOnly_RejectsConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_lvalue_only<float>),
        TemplateQualified const &, float);
    STATIC_EXPECT_FALSE(result);
}

TEST(TemplateMethodQualifiers, LValueOnly_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_lvalue_only<float>),
        TemplateQualified &&, float);
    STATIC_EXPECT_FALSE(result);
}

TEST(TemplateMethodQualifiers, LValueOnly_RejectsWrongArgType)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_lvalue_only<float>),
        TemplateQualified &, int *);
    STATIC_EXPECT_FALSE(result);
}

// --- temp_mut_and_const: & and const& ---

TEST(TemplateMethodQualifiers, MutAndConst_MatchesMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_mut_and_const<double>),
        TemplateQualified &, double);
    STATIC_EXPECT_TRUE(result);
}

TEST(TemplateMethodQualifiers, MutAndConst_MatchesConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_mut_and_const<double>),
        TemplateQualified const &, double);
    STATIC_EXPECT_TRUE(result);
}

TEST(TemplateMethodQualifiers, MutAndConst_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_mut_and_const<double>),
        TemplateQualified &&, double);
    STATIC_EXPECT_FALSE(result);
}

TEST(TemplateMethodQualifiers, MutAndConst_RejectsVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_mut_and_const<double>),
        TemplateQualified volatile &, double);
    STATIC_EXPECT_FALSE(result);
}

// --- temp_all_lvalue: &, const&, volatile&, const volatile& ---

TEST(TemplateMethodQualifiers, AllLValue_MatchesMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_all_lvalue<int>),
        TemplateQualified &, int);
    STATIC_EXPECT_TRUE(result);
}

TEST(TemplateMethodQualifiers, AllLValue_MatchesConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_all_lvalue<int>),
        TemplateQualified const &, int);
    STATIC_EXPECT_TRUE(result);
}

TEST(TemplateMethodQualifiers, AllLValue_MatchesVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_all_lvalue<int>),
        TemplateQualified volatile &, int);
    STATIC_EXPECT_TRUE(result);
}

TEST(TemplateMethodQualifiers, AllLValue_MatchesCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_all_lvalue<int>),
        TemplateQualified const volatile &, int);
    STATIC_EXPECT_TRUE(result);
}

TEST(TemplateMethodQualifiers, AllLValue_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_all_lvalue<int>),
        TemplateQualified &&, int);
    STATIC_EXPECT_FALSE(result);
}

TEST(TemplateMethodQualifiers, AllLValue_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_all_lvalue<int>),
        TemplateQualified const &&, int);
    STATIC_EXPECT_FALSE(result);
}

// --- temp_const_rvalue_only: only const&& ---

TEST(TemplateMethodQualifiers, ConstRValueOnly_MatchesConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(
        SCL_FORWARD(template temp_const_rvalue_only<short>), TemplateQualified const &&, short);
    STATIC_EXPECT_TRUE(result);
}

TEST(TemplateMethodQualifiers, ConstRValueOnly_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(
        SCL_FORWARD(template temp_const_rvalue_only<short>), TemplateQualified &&, short);
    STATIC_EXPECT_FALSE(result);
}

TEST(TemplateMethodQualifiers, ConstRValueOnly_RejectsConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(
        SCL_FORWARD(template temp_const_rvalue_only<short>), TemplateQualified const &, short);
    STATIC_EXPECT_FALSE(result);
}

// --- temp_multi: two template parameters ---

TEST(TemplateMethodQualifiers, MultiParam_MatchesMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_multi<float, int>),
        TemplateQualified &, float, int);
    STATIC_EXPECT_TRUE(result);
}

TEST(TemplateMethodQualifiers, MultiParam_MatchesConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_multi<float, int>),
        TemplateQualified const &, float, int);
    STATIC_EXPECT_TRUE(result);
}

TEST(TemplateMethodQualifiers, MultiParam_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_multi<float, int>),
        TemplateQualified &&, float, int);
    STATIC_EXPECT_FALSE(result);
}

TEST(TemplateMethodQualifiers, MultiParam_RejectsWrongArgs)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_multi<float, int>),
        TemplateQualified &, double *, long *);
    STATIC_EXPECT_FALSE(result);
}

// --- non-reference type with template method always returns false ---

TEST(TemplateMethodQualifiers, NonReference_ReturnsFalse)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(template temp_lvalue_only<float>),
        TemplateQualified, float);
    STATIC_EXPECT_FALSE(result);
}
