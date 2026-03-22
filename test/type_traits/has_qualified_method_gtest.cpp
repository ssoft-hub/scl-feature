#include <gtest/gtest.h>

#include <scl/feature/type_traits/has_qualified_method.h>

#define TEST_EXPECT_TRUE(X) \
    static_assert((X), #X); \
    EXPECT_TRUE(X);

#define TEST_EXPECT_FALSE(X) \
    static_assert(!(X), #X); \
    EXPECT_FALSE(X);

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
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, MutableLValueOnly_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(lvalue_only, Qualified &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableLValueOnly_RejectsConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(lvalue_only, Qualified const &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableLValueOnly_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(lvalue_only, Qualified const &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableLValueOnly_RejectsVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(lvalue_only, Qualified volatile &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableLValueOnly_RejectsVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(lvalue_only, Qualified volatile &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableLValueOnly_RejectsCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(lvalue_only, Qualified const volatile &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableLValueOnly_RejectsCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(lvalue_only, Qualified const volatile &&);
    TEST_EXPECT_FALSE(result);
}

// --- const_lvalue_only: void const_lvalue_only() const & ---

TEST(MethodQualifiers, ConstLValueOnly_RejectsMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_lvalue_only, Qualified &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstLValueOnly_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_lvalue_only, Qualified &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstLValueOnly_MatchesConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_lvalue_only, Qualified const &);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, ConstLValueOnly_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_lvalue_only, Qualified const &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstLValueOnly_RejectsVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_lvalue_only, Qualified volatile &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstLValueOnly_RejectsVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_lvalue_only, Qualified volatile &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstLValueOnly_RejectsCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_lvalue_only, Qualified const volatile &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstLValueOnly_RejectsCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_lvalue_only, Qualified const volatile &&);
    TEST_EXPECT_FALSE(result);
}

// --- volatile_lvalue_only: void volatile_lvalue_only() volatile & ---

TEST(MethodQualifiers, VolatileLValueOnly_RejectsMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_lvalue_only, Qualified &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileLValueOnly_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_lvalue_only, Qualified &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileLValueOnly_RejectsConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_lvalue_only, Qualified const &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileLValueOnly_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_lvalue_only, Qualified const &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileLValueOnly_MatchesVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_lvalue_only, Qualified volatile &);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, VolatileLValueOnly_RejectsVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_lvalue_only, Qualified volatile &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileLValueOnly_RejectsCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_lvalue_only, Qualified const volatile &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileLValueOnly_RejectsCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_lvalue_only, Qualified const volatile &&);
    TEST_EXPECT_FALSE(result);
}

// --- cv_lvalue_only: void cv_lvalue_only() const volatile & ---

TEST(MethodQualifiers, CVLValueOnly_RejectsMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_lvalue_only, Qualified &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVLValueOnly_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_lvalue_only, Qualified &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVLValueOnly_RejectsConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_lvalue_only, Qualified const &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVLValueOnly_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_lvalue_only, Qualified const &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVLValueOnly_RejectsVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_lvalue_only, Qualified volatile &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVLValueOnly_RejectsVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_lvalue_only, Qualified volatile &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVLValueOnly_MatchesCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_lvalue_only, Qualified const volatile &);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, CVLValueOnly_RejectsCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_lvalue_only, Qualified const volatile &&);
    TEST_EXPECT_FALSE(result);
}

// --- rvalue_only: void rvalue_only() & ---

TEST(MethodQualifiers, MutableRValueOnly_RejectsMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(rvalue_only, Qualified &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableRValueOnly_MatchesMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(rvalue_only, Qualified &&);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, MutableRValueOnly_RejectsConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(rvalue_only, Qualified const &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableRValueOnly_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(rvalue_only, Qualified const &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableRValueOnly_RejectsVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(rvalue_only, Qualified volatile &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableRValueOnly_RejectsVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(rvalue_only, Qualified volatile &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableRValueOnly_RejectsCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(rvalue_only, Qualified const volatile &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableRValueOnly_RejectsCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(rvalue_only, Qualified const volatile &&);
    TEST_EXPECT_FALSE(result);
}

// --- const_rvalue_only: void const_rvalue_only() const & ---

TEST(MethodQualifiers, ConstRValueOnly_RejectsMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_rvalue_only, Qualified &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstRValueOnly_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_rvalue_only, Qualified &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstRValueOnly_RejectsConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_rvalue_only, Qualified const &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstRValueOnly_MatchesConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_rvalue_only, Qualified const &&);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, ConstRValueOnly_RejectsVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_rvalue_only, Qualified volatile &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstRValueOnly_RejectsVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_rvalue_only, Qualified volatile &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstRValueOnly_RejectsCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_rvalue_only, Qualified const volatile &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, ConstRValueOnly_RejectsCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(const_rvalue_only, Qualified const volatile &&);
    TEST_EXPECT_FALSE(result);
}

// --- volatile_rvalue_only: void volatile_rvalue_only() volatile & ---

TEST(MethodQualifiers, VolatileRValueOnly_RejectsMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_rvalue_only, Qualified &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileRValueOnly_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_rvalue_only, Qualified &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileRValueOnly_RejectsConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_rvalue_only, Qualified const &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileRValueOnly_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_rvalue_only, Qualified const &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileRValueOnly_RejectsVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_rvalue_only, Qualified volatile &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileRValueOnly_MatchesVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_rvalue_only, Qualified volatile &&);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, VolatileRValueOnly_RejectsCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_rvalue_only, Qualified const volatile &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, VolatileRValueOnly_RejectsCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(volatile_rvalue_only, Qualified const volatile &&);
    TEST_EXPECT_FALSE(result);
}

// --- cv_rvalue_only: void cv_rvalue_only() const volatile & ---

TEST(MethodQualifiers, CVRValueOnly_RejectsMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_rvalue_only, Qualified &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVRValueOnly_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_rvalue_only, Qualified &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVRValueOnly_RejectsConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_rvalue_only, Qualified const &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVRValueOnly_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_rvalue_only, Qualified const &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVRValueOnly_RejectsVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_rvalue_only, Qualified volatile &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVRValueOnly_RejectsVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_rvalue_only, Qualified volatile &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVRValueOnly_RejectsCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_rvalue_only, Qualified const volatile &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, CVRValueOnly_MatchesCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(cv_rvalue_only, Qualified const volatile &&);
    TEST_EXPECT_TRUE(result);
}

// --- mutable_and_const: overloaded & and const & ---

TEST(MethodQualifiers, MutableAndConst_MatchesMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(mutable_and_const, Qualified &);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, MutableAndConst_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(mutable_and_const, Qualified &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, MutableAndConst_MatchesConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(mutable_and_const, Qualified const &);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, MutableAndConst_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(mutable_and_const, Qualified const &&);
    TEST_EXPECT_FALSE(result);
}

// --- all_lvalue: overloaded &, const&, volatile&, const volatile& ---

TEST(MethodQualifiers, AllLValue_MatchesMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_lvalue, Qualified &);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllLValue_MatchesMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_lvalue, Qualified &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, AllLValue_MatchesConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_lvalue, Qualified const &);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllLValue_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_lvalue, Qualified const &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, AllLValue_MatchesVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_lvalue, Qualified volatile &);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllLValue_RejectsVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_lvalue, Qualified volatile &&);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, AllLValue_MatchesCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_lvalue, Qualified const volatile &);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllLValue_MatchesCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_lvalue, Qualified const volatile &&);
    TEST_EXPECT_FALSE(result);
}

// --- all_rvalue: overloaded &&, const&&, volatile&&, const volatile&& ---

TEST(MethodQualifiers, AllRValue_RejectsMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_rvalue, Qualified &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, AllRValue_MatchesMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_rvalue, Qualified &&);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllRValue_RejectsConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_rvalue, Qualified const &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, AllRValue_MatchesConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_rvalue, Qualified const &&);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllRValue_RejectsVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_rvalue, Qualified volatile &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, AllRValue_MatchesVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_rvalue, Qualified volatile &&);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllRValue_RejectsCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_rvalue, Qualified const volatile &);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, AllRValue_MatchesCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_rvalue, Qualified const volatile &&);
    TEST_EXPECT_TRUE(result);
}

// --- all_value ---

TEST(MethodQualifiers, AllValue_MatchesMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_value, Qualified &);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllValue_MatchesMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_value, Qualified &&);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllValue_MatchesConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_value, Qualified const &);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllValue_MatchesConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_value, Qualified const &&);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllValue_MatchesVolatileLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_value, Qualified volatile &);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllValue_MatchesVolatileRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_value, Qualified volatile &&);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllValue_MatchesCVLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_value, Qualified const volatile &);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, AllValue_MatchesCVRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(all_value, Qualified const volatile &&);
    TEST_EXPECT_TRUE(result);
}

// --- with_args: methods with parameters ---

TEST(MethodQualifiers, WithArgs_MatchesMutableLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(with_args, Qualified &, int, double);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, WithArgs_RejectsMutableRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(with_args, Qualified &&, int, double);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, WithArgs_MatchesConstLValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(with_args, Qualified const &, int, double);
    TEST_EXPECT_TRUE(result);
}

TEST(MethodQualifiers, WithArgs_RejectsConstRValue)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(with_args, Qualified const &&, int, double);
    TEST_EXPECT_FALSE(result);
}

TEST(MethodQualifiers, WithArgs_RejectsWrongArgs)
{
    constexpr auto result = SCL_HAS_QUALIFIED_METHOD(with_args, Qualified &, float);
    TEST_EXPECT_FALSE(result);
}

// --- non-reference type (neither lvalue nor rvalue) always returns false ---

TEST(MethodQualifiers, NonReference_ReturnsFalse)
{
    constexpr auto lvalue_result = SCL_HAS_QUALIFIED_METHOD(lvalue_only, Qualified);
    TEST_EXPECT_FALSE(lvalue_result);
    constexpr auto rvalue_result = SCL_HAS_QUALIFIED_METHOD(rvalue_only, Qualified);
    TEST_EXPECT_FALSE(rvalue_result);
}
