#pragma once

#include <scl/feature/detail/wrapper_guard.h>
#include <scl/feature/type_traits/wrapper.h>

#include <type_traits>

namespace scl::feature
{
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
     *    feature::wrapper_guard<decltype(w) &> g{w};
     *    // g.value() == 42
     * @endcode
     */
    template <typename Refer>
    using wrapper_guard = detail::wrapper_guard<Refer,
        ::scl::feature::is_wrapper_v<::std::remove_cvref_t<Refer>>
            ? detail::wrapper_guard_case::wrapper
            : detail::wrapper_guard_case::value>;
} // namespace scl::feature
