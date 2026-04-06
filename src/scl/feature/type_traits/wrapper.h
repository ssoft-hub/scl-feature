#pragma once

#include <scl/feature/concepts/executor.h>
#include <scl/utility/type_traits/forward_like.h>

#include <type_traits>

namespace scl::feature::detail
{
    template <typename Value, template <typename> class Executor>
        requires ::scl::feature::concepts::executor<Executor<Value>>
    class wrapper;
} // namespace scl::feature::detail

namespace scl::feature
{
    /**
     * @ingroup scl_feature_type_traits
     * @brief Checks whether @p Type is a @c wrapper specialization.
     *
     * cv-ref qualifiers on @p Type are stripped before the check (@c remove_cvref_t).
     *
     * @tparam Type  Type to check.
     *
     * @code{.cpp}
     *    static_assert(!is_wrapper_v<int>);
     *    static_assert( is_wrapper_v<wrapper<int, feature::inplace::plain>>);
     *    static_assert( is_wrapper_v<wrapper<int, feature::inplace::plain> const>);  // cv-ref stripped
     * @endcode
     */
    template <typename Type>
    inline constexpr bool is_wrapper_v = ::std::is_same_v<::std::remove_cvref_t<Type>, Type>
        ? false
        : is_wrapper_v<::std::remove_cvref_t<Type>>;

    template <typename Type, template <typename> class Executor>
    inline constexpr bool is_wrapper_v<::scl::feature::detail::wrapper<Type, Executor>> = true;

    // -------------------------------------------------------------------------

    /**
     * @ingroup scl_feature_type_traits
     * @brief Checks whether @p Test is compatible with (usable in place of) @p Expected.
     *
     * For non-wrapper types the check is: @p Test is the same type as @p Expected,
     * or @p Test is publicly derived from @p Expected.
     * For @c wrapper specializations that share the same template-template argument
     * the check is applied recursively to their value types.
     * cv-ref qualifiers are stripped from both arguments before the check (@c remove_cvref_t).
     *
     * @tparam Expected  Target type.
     * @tparam Test      Type to verify compatibility of.
     *
     * @code{.cpp}
     *   using P1 = wrapper<int,    feature::inplace::plain>;
     *   using P2 = wrapper<double, feature::inplace::plain>;
     *   using U1 = wrapper<int,    feature::inplace::uninitialized>;
     *
     *   static_assert( is_compatible_with_v<P1, P1>);         // identical types
     *   static_assert( is_compatible_with_v<P1, P1 const &>); // cv-ref stripped
     *   static_assert(!is_compatible_with_v<P1, P2>);         // incompatible value types
     *   static_assert(!is_compatible_with_v<P1, U1>);         // different template param
     *   static_assert(!is_compatible_with_v<int, P1>);        // non-wrapper vs wrapper
     *   static_assert( is_compatible_with_v<int, int>);       // same non-wrapper type
     * @endcode
     */
    template <typename Expected, typename Test>
    inline constexpr bool is_compatible_with_v =
        (::std::is_same_v<::std::remove_cvref_t<Expected>, Expected> &&
            ::std::is_same_v<::std::remove_cvref_t<Test>, Test>)
        ? (::std::is_same_v<Expected, Test> || ::std::is_base_of_v<Expected, Test>)
        : is_compatible_with_v<::std::remove_cvref_t<Expected>, ::std::remove_cvref_t<Test>>;

    template <typename Expected, typename Test, template <typename> class Executor>
    inline constexpr bool is_compatible_with_v<::scl::feature::detail::wrapper<Expected, Executor>,
        ::scl::feature::detail::wrapper<Test, Executor>> = is_compatible_with_v<Expected, Test>;

    // -------------------------------------------------------------------------

    /**
     * @ingroup scl_feature_type_traits
     * @brief Checks whether @p Expected (a wrapper) recursively contains a value
     *        that is compatible with @p Test (also a wrapper).
     *
     * Peels layers off @p Expected one at a time; at each level checks whether
     * the unwrapped value is compatible with the whole of @p Test via
     * @c is_compatible_with_v.  Returns @c false if either argument is not a
     * @c wrapper specialization.
     * cv-ref qualifiers are stripped from both arguments before the check (@c remove_cvref_t).
     *
     * @tparam Expected  Outer wrapper whose contents are inspected.
     * @tparam Test      Wrapper to match against.
     *
     * @code{.cpp}
     *   using W   = wrapper<int,                     feature::inplace::plain>;
     *   using WW  = wrapper<W,                       feature::inplace::plain>;
     *   using WWW = wrapper<WW,                      feature::inplace::plain>;
     *
     *   static_assert( is_compatible_with_part_of_v<WW,  W>);   // WW's inner value IS W
     *   static_assert( is_compatible_with_part_of_v<WWW, W>);   // two levels deep
     *   static_assert(!is_compatible_with_part_of_v<W,  WW>);   // W does not contain WW
     *   static_assert(!is_compatible_with_part_of_v<int, W>);   // Expected not a wrapper
     *   static_assert(!is_compatible_with_part_of_v<W, int>);   // Test not a wrapper
     * @endcode
     */
    template <typename Expected, typename Test>
    inline constexpr bool is_compatible_with_part_of_v =
        (::std::is_same_v<::std::remove_cvref_t<Expected>, Expected> &&
            ::std::is_same_v<::std::remove_cvref_t<Test>, Test>)
        ? false
        : is_compatible_with_part_of_v<::std::remove_cvref_t<Expected>, ::std::remove_cvref_t<Test>>;

    template <typename ExpectedValue, template <typename> class ExpectedExecutor, typename TestValue, template <typename> class TestExecutor>
    inline constexpr bool is_compatible_with_part_of_v<::scl::feature::detail::wrapper<ExpectedValue, ExpectedExecutor>,
        ::scl::feature::detail::wrapper<TestValue, TestExecutor>> =
        is_compatible_with_v<::std::remove_cvref_t<ExpectedValue>,
            ::scl::feature::detail::wrapper<TestValue, TestExecutor>> ||
        is_compatible_with_part_of_v<::std::remove_cvref_t<ExpectedValue>,
            ::scl::feature::detail::wrapper<TestValue, TestExecutor>>;

    // -------------------------------------------------------------------------

    /**
     * @ingroup scl_feature_type_traits
     * @brief Checks whether @p Test (a wrapper) recursively contains a value
     *        that is compatible with @p Expected (also a wrapper).
     *
     * Peels layers off @p Test one at a time; at each level checks whether
     * the unwrapped value is compatible with the whole of @p Expected via
     * @c is_compatible_with_v.  Returns @c false if either argument is not a
     * @c wrapper specialization.
     * cv-ref qualifiers are stripped from both arguments before the check (@c remove_cvref_t).
     *
     * @tparam Expected  Wrapper to match against.
     * @tparam Test      Outer wrapper whose contents are inspected.
     *
     * @code{.cpp}
     *   using W   = wrapper<int,                     feature::inplace::plain>;
     *   using WW  = wrapper<W,                       feature::inplace::plain>;
     *   using WWW = wrapper<WW,                      feature::inplace::plain>;
     *
     *   static_assert( is_part_compatible_with_v<W,  WW>);    // WW's inner value IS W
     *   static_assert( is_part_compatible_with_v<W,  WWW>);   // two levels deep
     *   static_assert(!is_part_compatible_with_v<WW, W>);     // W does not contain WW
     *   static_assert(!is_part_compatible_with_v<int, W>);    // Expected not a wrapper
     *   static_assert(!is_part_compatible_with_v<W, int>);    // Test not a wrapper
     * @endcode
     */
    template <typename Expected, typename Test>
    inline constexpr bool is_part_compatible_with_v =
        (::std::is_same_v<::std::remove_cvref_t<Expected>, Expected> &&
            ::std::is_same_v<::std::remove_cvref_t<Test>, Test>)
        ? false
        : is_part_compatible_with_v<::std::remove_cvref_t<Expected>, ::std::remove_cvref_t<Test>>;

    template <typename ExpectedValue, template <typename> class ExpectedExecutor, typename TestValue, template <typename> class TestExecutor>
    inline constexpr bool is_part_compatible_with_v<::scl::feature::detail::wrapper<ExpectedValue, ExpectedExecutor>,
        ::scl::feature::detail::wrapper<TestValue, TestExecutor>> =
        is_compatible_with_v<::scl::feature::detail::wrapper<ExpectedValue, ExpectedExecutor>,
            ::std::remove_cvref_t<TestValue>> ||
        is_part_compatible_with_v<::scl::feature::detail::wrapper<ExpectedValue, ExpectedExecutor>,
            ::std::remove_cvref_t<TestValue>>;

    // -------------------------------------------------------------------------

    namespace detail
    {
        enum class convertible_from_case : bool
        {
            value = false,
            wrapper = true,
        };

        template <typename Target,
            typename Refer,
            convertible_from_case Case =
                ::scl::feature::is_wrapper_v<Refer> ? convertible_from_case::wrapper : convertible_from_case::value>
        struct is_convertible_from_impl;

        template <typename Target, typename Refer>
        struct is_convertible_from_impl<Target, Refer, convertible_from_case::value>
            : ::std::bool_constant<::std::is_convertible_v<Refer, Target>>
        {};

        template <typename Target, typename WrapperRefer>
        struct is_convertible_from_impl<Target, WrapperRefer, convertible_from_case::wrapper>
            : ::std::bool_constant<::std::is_convertible_v<WrapperRefer, Target> ||
                  is_convertible_from_impl<Target,
                      ::scl::forward_like_t<WrapperRefer, typename ::std::remove_cvref_t<WrapperRefer>::value_type>>::value>
        {};
    } // namespace detail

    /**
     * @ingroup scl_feature_type_traits
     * @brief Checks whether @p Target is convertible from @p Refer directly or
     *        through any level of the wrapper chain.
     *
     * Extends @c std::is_convertible to wrapper chains:
     * - base case: @p Refer is directly convertible to @p Target, or
     * - recursive case: @p Refer is a wrapper and @p Target is convertible from
     *   the inner value reference type.
     *
     * This is the condition under which
     * @c value_lock<Refer>::lock_for<Target>() and
     * @c value_lock<Refer>::value_as<Target>() are well-formed.
     *
     * @tparam Target  The desired reference type (cv-ref qualified).
     * @tparam Refer   The starting reference type (outermost wrapper or plain value).
     */
    template <typename Target, typename Refer>
    inline constexpr bool is_convertible_from_v =
        ::scl::feature::detail::is_convertible_from_impl<Target, Refer>::value;

} // namespace scl::feature
