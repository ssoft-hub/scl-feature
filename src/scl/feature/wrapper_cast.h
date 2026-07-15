#pragma once

#ifndef DOXYGEN

#include <scl/feature/concepts/wrapper.h>
#include <scl/feature/detail/wrapper_cast.h>

#include <utility>

namespace scl
{
    template <typename Refer>
    using wrapper_caster = ::scl::feature::detail::wrapper_caster<Refer>;

    template <typename Type>
    [[nodiscard]]
    constexpr decltype(auto) wrapper_cast(Type && w) noexcept
    {
        return ::scl::wrapper_caster<Type &&>{::std::forward<Type>(w)};
    }

} // namespace scl

#else // DOXYGEN

namespace scl
{
    /** @addtogroup scl_feature_wrapper_cast
     *  @{
     */

    /**
     * @brief Lazy-locking cast proxy for a wrapper or plain value reference.
     *
     * @c wrapper_caster<Refer> holds a @c value_lock (all executor references
     * captured, no guard acquired yet) and exposes implicit conversion operators
     * for every reachable level in the wrapper chain.  Guards are activated
     * **only** at the moment of conversion.
     *
     * The proxy must be used as an rvalue; all conversion operators are
     * @c &&-qualified.
     *
     * @warning The proxy borrows a reference to its argument and does not extend the
     *          argument's lifetime.  Consume it within the same full-expression; do not store
     *          the proxy, especially from a temporary — @c auto @c p @c = @c wrapper_cast(makeW());
     *          leaves @c p referring to a destroyed wrapper once that full-expression ends.
     *
     * @tparam Refer  Reference type passed to @c wrapper_cast().
     *
     * @code{.cpp}
     *   wrapper<int, mutex_executor> w{42};
     *
     *   int x = wrapper_cast(w);         // value copied; guard active during copy
     *   auto proxy = wrapper_cast(w);
     *   int & r = std::move(proxy);      // guard active until proxy is destroyed
     * @endcode
     */
    template <typename Refer>
    class [[nodiscard]] wrapper_caster
    {
    public:
        wrapper_caster(wrapper_caster &&) = delete;
        wrapper_caster(wrapper_caster const &) = delete;
        wrapper_caster & operator=(wrapper_caster &&) = delete;
        wrapper_caster & operator=(wrapper_caster const &) = delete;

        /**
         * @brief Constructs the proxy, capturing references to every executor in the
         *        wrapper chain via an internal @c value_lock.  No guard is acquired.
         *
         * @param ref  Reference to the outermost wrapper or plain value.
         */
        explicit wrapper_caster(Refer ref);

        /**
         * @brief Releases all guards acquired during conversion.
         *
         * If a conversion operator was invoked, the internal @c value_lock releases
         * all held guards in reverse order (innermost first).
         * If no conversion was performed, no guards are released.
         */
        ~wrapper_caster();

        /**
         * @brief Implicit conversion to the reference type @p Refer.
         *
         * Activates guards for all wrapper layers needed to reach the target type
         * and returns the value.  The proxy must be an rvalue at the call site.
         *
         * One overload is available for each type reachable in the wrapper chain
         * (the wrapper itself, its @c value_type, and so on recursively).
         *
         * @note Acquired guards remain held until the @c wrapper_caster is destroyed,
         *       not until the end of the conversion expression.
         */
        [[nodiscard]]
        operator Refer() &&;

        /**
         * @brief Acquires the guards needed to reach @p T and returns the value.
         *
         * @tparam T  Target reference type — must be reachable from @p Refer via
         *            @c concepts::convertible_from.
         *
         * The proxy must be an rvalue at the call site.
         *
         * @note Acquired guards remain held until the @c wrapper_caster is destroyed.
         */
        template <typename T>
        [[nodiscard]]
        T to() &&
            requires ::scl::feature::concepts::convertible_from<T, Refer>;
    };

    /**
     * @brief Returns a lazy-locking cast proxy for a wrapper reference.
     *
     * Guards are **not** acquired at the call site — they are activated lazily
     * when the returned @c wrapper_caster<Wrapper> is converted to the desired
     * target type.
     *
     * @tparam Wrapper  A @c wrapper specialisation (cv-ref qualifiers are ignored
     *                  for the concept check but propagated into the proxy).
     * @param  w        Forwarding reference to the wrapper.
     * @return          A @c wrapper_caster<Wrapper> proxy.
     */
    template <typename Wrapper>
    [[nodiscard]]
    constexpr decltype(auto) wrapper_cast(Wrapper && w);

    /**
     * @brief Pass-through overload for non-wrapper types — returns the reference unchanged.
     *
     * Allows generic code to call @c wrapper_cast() uniformly on both wrapper
     * and non-wrapper values.
     *
     * @tparam Value  Any non-wrapper type.
     * @param  v      Forwarding reference to the value.
     * @return        @p v forwarded as-is (no allocation, no guard).
     */
    template <typename Value>
    [[nodiscard]]
    constexpr decltype(auto) wrapper_cast(Value && v) noexcept;

    /** @} */ // end of group scl_feature_wrapper_cast
} // namespace scl

#endif // DOXYGEN
