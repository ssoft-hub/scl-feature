#pragma once

#ifndef DOXYGEN

#include <scl/feature/detail/value_lock.h>
#include <scl/feature/type_traits/wrapper.h>

#include <type_traits>

namespace scl
{
    template <typename Refer>
    using value_lock = ::scl::feature::detail::value_lock<Refer,
        ::scl::feature::is_wrapper_v<::std::remove_cvref_t<Refer>>
            ? ::scl::feature::detail::value_lock_case::wrapper
            : ::scl::feature::detail::value_lock_case::value>;
} // namespace scl

#else // DOXYGEN

namespace scl
{
    /** @addtogroup scl_feature_locking
     *  @{
     */

    /**
     * @brief Recursive lazy RAII lock through a wrapper chain.
     *
     * Stores references to every executor in the wrapper chain but does
     * **not** acquire any guard at construction.  Guards are activated lazily
     * via @c lock_for<Target>(), which locks only the layers required to reach
     * @p Target, and released automatically on destruction.
     *
     * For non-wrapper types @c lock_for() / @c value_as() are no-ops / direct
     * reference passes.
     *
     * @tparam Refer  Reference type — either @c wrapper<V,E> cv-ref or a plain
     *                value cv-ref.
     *
     * @code{.cpp}
     *   wrapper<wrapper<int, A>, B> ww{...};
     *   value_lock<decltype(ww) &> vl{ww};
     *   vl.lock_for<int &>();           // locks both B and A executors
     *   int & v = vl.value_as<int &>(); // safe: both guards held
     * @endcode
     */
    template <typename Refer>
    class value_lock
    {
    public:
        value_lock(value_lock &&) = delete;
        value_lock(value_lock const &) = delete;
        value_lock & operator=(value_lock &&) = delete;
        value_lock & operator=(value_lock const &) = delete;

        /**
         * @brief Constructs the lock, capturing references to every executor in the
         *        wrapper chain.  No guard is acquired at construction.
         *
         * @param ref  Reference to the outermost wrapper or plain value.
         */
        explicit value_lock(Refer ref);

        /**
         * @brief Releases all guards acquired via @c lock_for().
         *
         * Each inner @c wrapper_lock destructor calls @c unlock() — guards are
         * released in reverse order (innermost first).
         */
        ~value_lock();

        /**
         * @brief Activates guards for every wrapper layer needed to reach @p Target.
         *
         * @tparam Target  The desired value type (cv-ref qualified).
         *
         * If @p Target equals the outermost wrapper reference type, no guard is
         * acquired (identity — no unwrapping required).  Otherwise locks each layer
         * from the outside in until @p Target is reached.
         *
         * Calling @c lock_for() a second time with the same @p Target is safe but
         * will re-enter the idempotency check of the underlying @c wrapper_lock.
         *
         * For non-wrapper types @p Target must equal @c Refer; the call is a no-op.
         */
        template <typename Target>
        void lock_for();

        /**
         * @brief Returns the stored value at the level matching @p Target.
         *
         * @tparam Target  The desired value type (cv-ref qualified).
         *
         * @pre  @c lock_for<Target>() must have been called beforehand for any
         *       wrapper layer that guards the requested value.
         *
         * If @p Target equals the outermost wrapper reference type, returns the
         * wrapper itself without going deeper.  Otherwise delegates recursively to
         * the inner @c value_lock.
         */
        template <typename Target>
        Target value_as() const;
    };

    /** @} */ // end of group scl_feature_locking
} // namespace scl

#endif // DOXYGEN
