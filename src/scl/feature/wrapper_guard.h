#pragma once

#ifndef DOXYGEN

#include <scl/feature/detail/wrapper_guard.h>
#include <scl/feature/type_traits/wrapper.h>

namespace scl
{
    template <typename Refer>
    using wrapper_guard = ::scl::feature::detail::wrapper_guard<Refer>;
} // namespace scl

#else // DOXYGEN

namespace scl
{
    /** @addtogroup scl_feature_locking
     *  @{
     */

    /**
     * @brief RAII guard providing access to the value held by a wrapper or a plain reference.
     *
     * For wrapper types calls @c guard() on the executor at construction and
     * @c unguard() at destruction (both are no-ops if the executor does not define them).
     * Exposes @c value() to reach the stored value with the same cv- and
     * ref-qualifiers as the incoming reference.
     *
     * For non-wrapper types simply holds the reference; no guard/unguard is performed.
     *
     * @tparam Refer  Reference type — either @c wrapper<V,E> cv-ref or a plain value cv-ref.
     *
     * @code{.cpp}
     *    wrapper<int, feature::inplace::plain> w{42};
     *    wrapper_guard<decltype(w) &> g{w};
     *    // g.value() == 42
     * @endcode
     */
    template <typename Refer>
    class wrapper_guard
    {
    public:
        wrapper_guard(wrapper_guard &&) = delete;
        wrapper_guard(wrapper_guard const &) = delete;
        wrapper_guard & operator=(wrapper_guard &&) = delete;
        wrapper_guard & operator=(wrapper_guard const &) = delete;

        /**
         * @brief Constructs the guard and acquires access to @p v.
         *
         * For wrapper types calls @c executor_type::guard() immediately (if defined).
         * For non-wrapper types simply stores the reference — no side effects.
         *
         * @param v  Reference to the wrapper or plain value to guard.
         */
        explicit wrapper_guard(Refer v);

        /**
         * @brief Releases the guard acquired at construction.
         *
         * For wrapper types calls @c executor_type::unguard() (if defined).
         * For non-wrapper types is a no-op.
         */
        ~wrapper_guard();

        /**
         * @brief Returns the stored value with the same cv- and ref-qualifiers as @p Refer.
         *
         * For wrapper types delegates to @c executor_type::value().
         * For non-wrapper types returns the held reference unchanged.
         *
         * @pre  For wrapper types the guard is held (guaranteed by the constructor).
         */
        decltype(auto) value() const noexcept;
    };

    /** @} */ // end of group scl_feature_locking
} // namespace scl

#endif // DOXYGEN
