#pragma once

/// @file
/// @brief Executor that holds a value in uninitialized aligned storage.
/// @ingroup scl_feature_executors

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>

#include <scl/utility/type_traits/forward_like.h>

#define SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(cv_ref)                                                              \
    constexpr explicit uninitialized(self_type cv_ref other) /**/                                                        \
        noexcept                                                                                                         \
        requires(::std::is_trivially_copyable_v<value_type> && ::std::constructible_from<value_type, value_type cv_ref>) \
    {                                                                                                                    \
        ::std::ranges::copy(other.m_storage, m_storage);                                                                 \
    }

#define SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF                         \
    SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(&)                \
    SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const &)          \
    SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(volatile &)       \
    SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const volatile &) \
    SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(&&)               \
    SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const &&)         \
    SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(volatile &&)      \
    SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const volatile &&)

#define SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE(cv_ref)                                                              \
    constexpr uninitialized & operator=(self_type cv_ref other) /**/                                                    \
        noexcept                                                                                                        \
        requires(::std::is_trivially_copyable_v<value_type> && ::std::assignable_from<value_type &, value_type cv_ref>) \
    {                                                                                                                   \
        ::std::ranges::copy(other.m_storage, m_storage);                                                                \
        return *this;                                                                                                   \
    }

#define SCL_EXECUTOR_ASSIGNMENT_FOR_SELF                         \
    SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE(&)                \
    SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE(const &)          \
    SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE(volatile &)       \
    SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE(const volatile &) \
    SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE(&&)               \
    SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE(const &&)         \
    SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE(volatile &&)      \
    SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE(const volatile &&)

namespace scl::feature::inplace
{
    /**
     * @brief Executor that holds @p Value in uninitialized aligned storage.
     * @ingroup scl_feature_executors
     *
     * Provides correctly sized and aligned storage without constructing or
     * destroying the object automatically, enabling deferred initialization.
     *
     * @note Copy/move construction and assignment copy the raw storage bytes, so
     *       those operations are constrained to @c std::is_trivially_copyable_v
     *       value types — byte-copying a non-trivially-copyable object is undefined
     *       behaviour.  The executor type itself remains usable in pure type-level
     *       contexts (e.g. wrapper nesting and trait queries) for any @p Value.
     *
     * @tparam Value  The wrapped value type.
     *
     * @code{.cpp}
     *    wrapper<int, feature::inplace::uninitialized> w{};
     * @endcode
     */
    // NOLINTBEGIN(cppcoreguidelines-special-member-functions, cppcoreguidelines-missing-std-forward, cppcoreguidelines-pro-type-member-init, cppcoreguidelines-c-copy-assignment-signature, bugprone-unhandled-self-assignment)
    template <typename Value>
    class uninitialized
    {
        using self_type = uninitialized<Value>;

    public:
        using value_type = Value;

    public:
        constexpr uninitialized() noexcept = default;

        SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF
        SCL_EXECUTOR_ASSIGNMENT_FOR_SELF

    public:
        template <typename Self, typename Func, typename... Args>
        static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args)
            requires(::std::same_as<::std::remove_cvref_t<Self>, self_type> && ::std::invocable<Func, Args && ...>)
        {
            return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
        }

        template <typename Self>
        static constexpr decltype(auto) value(Self && self)
            requires ::std::same_as<::std::remove_cvref_t<Self>, self_type>
        {
            // reinterpret_cast<value_type*> for raw storage access;
            // const correctness is restored by forward_like<Self>.
            return reinterpret_cast<::scl::forward_like_t<Self, value_type>>(*self.m_storage); // NOLINT(*-reinterpret-cast)
        }

    private:
        alignas(value_type)::std::byte m_storage[sizeof(value_type)]; // NOLINT(*-avoid-c-arrays)
    };
    // NOLINTEND(cppcoreguidelines-special-member-functions, cppcoreguidelines-missing-std-forward, cppcoreguidelines-pro-type-member-init, cppcoreguidelines-c-copy-assignment-signature, bugprone-unhandled-self-assignment)
} // namespace scl::feature::inplace

#undef SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF
#undef SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE
#undef SCL_EXECUTOR_ASSIGNMENT_FOR_SELF
#undef SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE
