#pragma once

#include <gtest/gtest.h>

/// @file gtest_utils.h
/// @brief Shared test utilities for scl::utility tests.
///
/// Each macro verifies an expression both at compile time (`static_assert`)
/// and at runtime (corresponding GTest `EXPECT_*` macro).

/// @brief `static_assert(expr)` + `EXPECT_TRUE(expr)`
#define STATIC_EXPECT_TRUE(expr)  \
    static_assert((expr), #expr); \
    EXPECT_TRUE(expr)

/// @brief `static_assert(!expr)` + `EXPECT_FALSE(expr)`
#define STATIC_EXPECT_FALSE(expr)           \
    static_assert(!(expr), "!(" #expr ")"); \
    EXPECT_FALSE(expr)

/// @brief `static_assert(a == b)` + `EXPECT_EQ(a, b)`
#define STATIC_EXPECT_EQ(a, b)               \
    static_assert((a) == (b), #a " == " #b); \
    EXPECT_EQ(a, b)

/// @brief `static_assert(a != b)` + `EXPECT_NE(a, b)`
#define STATIC_EXPECT_NE(a, b)               \
    static_assert((a) != (b), #a " != " #b); \
    EXPECT_NE(a, b)

/// @brief `static_assert(a < b)` + `EXPECT_LT(a, b)`
#define STATIC_EXPECT_LT(a, b)             \
    static_assert((a) < (b), #a " < " #b); \
    EXPECT_LT(a, b)

/// @brief `static_assert(a <= b)` + `EXPECT_LE(a, b)`
#define STATIC_EXPECT_LE(a, b)               \
    static_assert((a) <= (b), #a " <= " #b); \
    EXPECT_LE(a, b)

/// @brief `static_assert(a > b)` + `EXPECT_GT(a, b)`
#define STATIC_EXPECT_GT(a, b)             \
    static_assert((a) > (b), #a " > " #b); \
    EXPECT_GT(a, b)

/// @brief `static_assert(a >= b)` + `EXPECT_GE(a, b)`
#define STATIC_EXPECT_GE(a, b)               \
    static_assert((a) >= (b), #a " >= " #b); \
    EXPECT_GE(a, b)
