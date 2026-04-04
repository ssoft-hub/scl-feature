#pragma once

#ifndef DOXYGEN

#include <scl/feature/detail/wrapper.h>
#include <scl/feature/inplace/plain.h>

#include <type_traits>

namespace scl::feature::detail
{
    template <typename State, template <typename> class... Executor>
    struct wrapper_fold
    {
        using type = State;
    };

    template <typename State, template <typename> class Executor, template <typename> class... Rest>
    struct wrapper_fold<State, Executor, Rest...>
    {
        using type = typename wrapper_fold<::scl::feature::detail::wrapper<State, Executor>, Rest...>::type;
    };

    template <typename Value, template <typename> class Executor, template <typename> class... Rest>
    struct wrapper_fold<::scl::feature::detail::wrapper<Value, Executor>, Executor, Rest...>
    {
        using type = typename wrapper_fold<::scl::feature::detail::wrapper<Value, Executor>, Rest...>::type;
    };
} // namespace scl::feature::detail

namespace scl
{
    template <typename Value, template <typename> class... Executors>
    using wrapper = ::std::conditional_t<sizeof...(Executors) == 0,
        typename ::scl::feature::detail::wrapper_fold<Value, ::scl::feature::inplace::plain>::type,
        typename ::scl::feature::detail::wrapper_fold<Value, Executors...>::type>;
} // namespace scl

#else // DOXYGEN

namespace scl
{
    /** @addtogroup scl_feature_wrapper
     *  @{
     */

    /**
     * @brief Composable proxy wrapper that delegates to @p Value through executors.
     *
     * Each executor is a class template @c E such that @c E<Value> proxies
     * method calls and may add cross-cutting behaviour (e.g. COW, thread
     * safety, deferred invocation).  Executors are composed left-to-right;
     * adjacent duplicates in the list are collapsed.  Defaults to
     * @c feature::inplace::plain — a zero-overhead direct proxy — when no executor
     * is specified.
     *
     * @tparam Value      The wrapped value type.
     * @tparam Executors  Zero or more executor class templates.
     *
     * @code{.cpp}
     *    wrapper<int> a{42};                                   // feature::inplace::plain (default)
     *    wrapper<int, feature::inplace::plain> b{42};          // explicit plain
     *    wrapper<int, feature::inplace::uninitialized> c{};    // deferred construction
     * @endcode
     */
    template <typename Value, template <typename> class... Executors>
    class wrapper
    {
    public:
        /// The wrapped value type.
        using value_type = Value;

        /// The outermost executor instantiation — @c Executor<value_type>.
        using executor_type = /* implementation defined */;

        /**
         * @brief Default construction — default-initialises the executor.
         *
         * Participates in overload resolution only if @c executor_type is
         * default-initialisable.
         */
        constexpr wrapper()
            requires ::std::default_initializable<executor_type>;

        /**
         * @brief Direct construction — forwards all arguments to the executor.
         *
         * Participates in overload resolution only if @c executor_type is
         * constructible from @p Args.
         *
         * @tparam Args  Argument types forwarded to the executor constructor.
         * @param  args  Arguments forwarded to the executor constructor.
         */
        template <typename... Args>
        constexpr explicit wrapper(Args &&... args)
            requires ::std::constructible_from<executor_type, Args...>;

        /**
         * @brief Copy / move construction from the same wrapper type.
         *
         * All eight cv-ref qualifications of @c wrapper<Value, Executors...>
         * are accepted.  Each overload forwards @p other's executor through
         * @c scl::forward_like so that the correct copy or move path is taken.
         * Each overload participates in overload resolution only if
         * @c executor_type is constructible from the corresponding cv-ref of
         * @c executor_type (i.e. if the executor supports that operation).
         *
         * @param other  Source wrapper (any cv-ref qualification).
         */
        constexpr wrapper(wrapper & other)
            requires ::std::constructible_from<executor_type, executor_type &>;
        /// @copydoc wrapper(wrapper &)
        constexpr wrapper(wrapper const & other)
            requires ::std::constructible_from<executor_type, executor_type const &>;
        /// @copydoc wrapper(wrapper &)
        constexpr wrapper(wrapper volatile & other)
            requires ::std::constructible_from<executor_type, executor_type volatile &>;
        /// @copydoc wrapper(wrapper &)
        constexpr wrapper(wrapper const volatile & other)
            requires ::std::constructible_from<executor_type, executor_type const volatile &>;
        /// @copydoc wrapper(wrapper &)
        constexpr wrapper(wrapper && other)
            requires ::std::constructible_from<executor_type, executor_type &&>;
        /// @copydoc wrapper(wrapper &)
        constexpr wrapper(wrapper const && other)
            requires ::std::constructible_from<executor_type, executor_type const &&>;
        /// @copydoc wrapper(wrapper &)
        constexpr wrapper(wrapper volatile && other)
            requires ::std::constructible_from<executor_type, executor_type volatile &&>;
        /// @copydoc wrapper(wrapper &)
        constexpr wrapper(wrapper const volatile && other)
            requires ::std::constructible_from<executor_type, executor_type const volatile &&>;

        /**
         * @brief Converting construction from a compatible wrapper type.
         *
         * Accepts any wrapper whose executor chain can be unwrapped / re-wrapped
         * into this wrapper's executor via @c wrapper_constructor_resolver.
         * Participates in overload resolution only if @p Other is a wrapper type
         * other than @c self_type and @c executor_type is constructible from the
         * resolved value.
         *
         * @tparam Other  Source wrapper type (cv-ref qualifiers are deduced).
         * @param  other  Source wrapper.
         */
        template <concepts::wrapper Other>
        constexpr wrapper(Other && other)
            requires(!::std::same_as<::std::remove_cvref_t<Other>, wrapper> &&
                ::std::constructible_from<executor_type,
                    decltype(wrapper_constructor_resolver<wrapper, Other &&>{other}.resolve())>);
    };

    /** @} */ // end of group scl_feature_wrapper
} // namespace scl

#endif // DOXYGEN
