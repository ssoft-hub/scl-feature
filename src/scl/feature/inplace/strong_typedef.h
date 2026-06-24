#pragma once

#include <concepts>
#include <functional>
#include <utility>

#include <scl/feature/wrapper.h>
#include <scl/utility/type_traits/forward_like.h>

/**
 * @defgroup scl_feature_inplace_strong_typedef Strong Typedef Executor
 * @ingroup scl_feature_inplace
 * @brief Tag-discriminated executor that produces a distinct named type.
 * @{
 */

// Internal macro: expands the eight cv-ref constructors that forward from another
// instance of the same self_type.  Defined locally and undefined at end of file.
#define SCL_STRONG_TYPEDEF_CONSTRUCTOR_FOR_SELF_PROTOTYPE(cv_ref)          \
    constexpr explicit type(self_type cv_ref other) noexcept(              \
        ::std::is_nothrow_constructible_v<value_type, value_type cv_ref>)  \
        requires(::std::constructible_from<value_type, value_type cv_ref>) \
        : m_value{::std::forward<value_type cv_ref>(other.m_value)}        \
    {}

/// @internal Expands to constructors for all eight cv-ref qualifications of `self_type`.
#define SCL_STRONG_TYPEDEF_CONSTRUCTOR_FOR_SELF                         \
    SCL_STRONG_TYPEDEF_CONSTRUCTOR_FOR_SELF_PROTOTYPE(&)                \
    SCL_STRONG_TYPEDEF_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const &)          \
    SCL_STRONG_TYPEDEF_CONSTRUCTOR_FOR_SELF_PROTOTYPE(volatile &)       \
    SCL_STRONG_TYPEDEF_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const volatile &) \
    SCL_STRONG_TYPEDEF_CONSTRUCTOR_FOR_SELF_PROTOTYPE(&&)               \
    SCL_STRONG_TYPEDEF_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const &&)         \
    SCL_STRONG_TYPEDEF_CONSTRUCTOR_FOR_SELF_PROTOTYPE(volatile &&)      \
    SCL_STRONG_TYPEDEF_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const volatile &&)

namespace scl::feature::inplace
{
    /**
     * @brief Executor template that creates a distinct named type per tag value.
     *
     * Because the executor slot in @c wrapper<Value, Executor> expects a
     * @c template<typename> class, the tag must be embedded via a nested
     * template.  Two instantiations with different @p Tag values produce
     * unrelated C++ types, so no implicit conversion is possible between them.
     *
     * @tparam Tag  A value (typically a local struct) that uniquely identifies
     *              the strong-typedef domain.  Different tags → different types.
     *
     * @ingroup scl_feature_inplace_strong_typedef
     *
     * @code{.cpp}
     *    using UserId  = scl::wrapper<int, strong_typedef<struct UserIdTag{}>::template type>;
     *    using OrderId = scl::wrapper<int, strong_typedef<struct OrderIdTag{}>::template type>;
     *
     *    UserId  uid{1};
     *    OrderId oid{2};
     *    // uid = oid;  // error: incompatible types
     * @endcode
     */
    template <auto Tag>
    struct strong_typedef
    {
        /**
         * @brief Concrete executor type for @p Value, tagged with @p Tag.
         *
         * @tparam Value  The wrapped value type.
         *
         * @ingroup scl_feature_inplace_strong_typedef
         */
        // NOLINTBEGIN(cppcoreguidelines-special-member-functions, cppcoreguidelines-missing-std-forward)
        template <typename Value>
        class type
        {
            using self_type = type<Value>;

        public:
            using value_type = Value;

        public:
            template <typename... Args>
            constexpr explicit type(Args &&... args)
                : m_value{::std::forward<Args>(args)...}
            {}

            SCL_STRONG_TYPEDEF_CONSTRUCTOR_FOR_SELF

        public:
            /**
             * @brief Invokes @p func with @p args, forwarding through the executor.
             *
             * @tparam Self   cv-ref qualified self_type.
             * @tparam Func   callable type.
             * @tparam Args2  argument types forwarded to @p func.
             * @param  self   the executor instance.
             * @param  func   the callable to invoke.
             * @param  args   arguments forwarded to @p func.
             * @return result of invoking @p func with @p args.
             *
             * @ingroup scl_feature_inplace_strong_typedef
             */
            template <typename Self, typename Func, typename... Args2>
            static constexpr decltype(auto) execute(Self && self, Func && func, Args2 &&... args)
                requires(::std::same_as<::std::remove_cvref_t<Self>, self_type> &&
                    ::std::invocable<Func, Args2 && ...>)
            {
                return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args2>(args)...);
            }

            /**
             * @brief Returns the held value, preserving cv-ref qualifiers of @p self.
             *
             * @tparam Self  cv-ref qualified self_type.
             * @param  self  the executor instance.
             * @return forwarding reference to the held @c value_type.
             *
             * @ingroup scl_feature_inplace_strong_typedef
             */
            template <typename Self>
            static constexpr decltype(auto) value(Self && self)
                requires ::std::same_as<::std::remove_cvref_t<Self>, self_type>
            {
                return ::scl::forward_like<Self>(self.m_value);
            }

        public:
            value_type m_value;
        };
        // NOLINTEND(cppcoreguidelines-special-member-functions, cppcoreguidelines-missing-std-forward)
    };

    /**
     * @brief Convenience alias: a @c wrapper<Value> distinguished by @p Tag.
     *
     * @tparam Tag    unique tag value identifying the strong-typedef domain.
     * @tparam Value  the underlying value type.
     *
     * @ingroup scl_feature_inplace_strong_typedef
     *
     * @code{.cpp}
     *    using UserId  = make_strong_typedef<struct UserIdTag{},  int>;
     *    using OrderId = make_strong_typedef<struct OrderIdTag{}, int>;
     * @endcode
     */
    template <auto Tag, typename Value>
    using make_strong_typedef = ::scl::wrapper<Value, strong_typedef<Tag>::template type>;
} // namespace scl::feature::inplace

/** @} */ // end of group scl_feature_inplace_strong_typedef

#undef SCL_STRONG_TYPEDEF_CONSTRUCTOR_FOR_SELF
#undef SCL_STRONG_TYPEDEF_CONSTRUCTOR_FOR_SELF_PROTOTYPE
