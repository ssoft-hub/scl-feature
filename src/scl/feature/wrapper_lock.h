#pragma once

#ifndef DOXYGEN

#include <scl/feature/detail/wrapper_lock.h>
#include <scl/feature/type_traits/wrapper.h>

namespace scl
{
    template <typename Refer>
    using wrapper_lock = ::scl::feature::detail::wrapper_lock<Refer>;
} // namespace scl

#else // DOXYGEN

namespace scl
{
    /** @addtogroup scl_feature_locking
     *  @{
     */

    /**
     * @brief Lazy RAII lock for a single wrapper layer.
     *
     * Stores the wrapper reference and a reference to its executor, but does
     * **not** call @c guard() at construction.  The guard is activated only
     * when @c lock() is called explicitly, and released when @c unlock() is
     * called or the object is destroyed.
     *
     * For non-wrapper types @c lock() / @c unlock() are no-ops and @c value()
     * returns the reference unchanged.
     *
     * @tparam Refer  Reference type — either @c wrapper<V,E> cv-ref or a plain
     *                value cv-ref.
     *
     * @code{.cpp}
     *   wrapper<int, counting_executor> w{42, ctrs};
     *   wrapper_lock<decltype(w) &> lk{w};
     *   lk.lock();
     *   auto& val = lk.value();   // safe: guard held
     *   lk.unlock();
     * @endcode
     */
    template <typename Refer>
    class wrapper_lock
    {
    public:
        wrapper_lock(wrapper_lock &&) = delete;
        wrapper_lock(wrapper_lock const &) = delete;
        wrapper_lock & operator=(wrapper_lock &&) = delete;
        wrapper_lock & operator=(wrapper_lock const &) = delete;

        /**
         * @brief Constructs the lock storing @p ref.  No guard is acquired.
         *
         * @param ref  Reference to the wrapper or plain value to lock.
         */
        explicit wrapper_lock(Refer ref);

        /**
         * @brief Releases the guard if it was acquired.
         *
         * Calls @c unlock().  @c noexcept if @c executor_type::unguard() does not
         * exist or is @c noexcept (see @c is_unguard_noexcept_v).
         */
        ~wrapper_lock();

        /**
         * @brief Acquires the guard on the executor (idempotent).
         *
         * For wrapper types calls @c executor_type::guard() on the first call;
         * subsequent calls are no-ops.  For non-wrapper types always a no-op.
         * @c noexcept if @c executor_type::guard() does not exist or is @c noexcept
         * (see @c is_guard_noexcept_v).
         */
        void lock();

        /**
         * @brief Releases the guard on the executor (idempotent).
         *
         * For wrapper types calls @c executor_type::unguard() if the guard is held,
         * then clears the locked flag.  Subsequent calls are no-ops.
         * For non-wrapper types always a no-op.
         * @c noexcept if @c executor_type::unguard() does not exist or is @c noexcept
         * (see @c is_unguard_noexcept_v).
         */
        void unlock();

        /**
         * @brief Returns the wrapper reference itself without requiring the lock.
         *
         * Only available for wrapper types.
         */
        Refer wrapper_value() const noexcept;

        /**
         * @brief Returns the inner value through the executor.
         *
         * For wrapper types delegates to @c executor_type::value().
         * For non-wrapper types returns the held reference unchanged.
         *
         * @pre  For wrapper types @c lock() must have been called beforehand.
         */
        decltype(auto) value() const noexcept;
    };

    /** @} */ // end of group scl_feature_locking
} // namespace scl

#endif // DOXYGEN
