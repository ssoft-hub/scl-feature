#pragma once

#include <scl/feature/type_traits/executor.h>

namespace scl::feature::concepts
{
    /**
     * @ingroup scl_feature_concepts
     * @brief Satisfied when @p Type implements the executor interface.
     *
     * An executor is a class template that defines the storage strategy and
     * access policy for an @c scl::wrapper.  @p Type must provide the
     * following static methods, callable for @c Type&, @c Type&&, and
     * @c Type const&:
     *
     * | Method                           | Role                                      |
     * |----------------------------------|-------------------------------------------|
     * | @c access(Self&&)                 | Returns a reference to the held value.    |
     * | @c execute(Self&&, Func&&)       | Invokes a callable in executor's context. |
     *
     * Optional methods (@c guard, @c unguard) may be provided to implement
     * RAII locking; their presence is detected separately via
     * @c is_guard_noexcept_v / @c is_unguard_noexcept_v.
     *
     * cv-ref qualifiers on @p Type are stripped before the check (@c remove_cvref_t).
     *
     * @tparam Type  Type to check.
     *
     * @see scl::feature::is_executor_v
     * @see scl::feature::inplace::plain
     * @see scl::feature::inplace::uninitialized
     *
     * @code{.cpp}
     *   static_assert( executor<feature::inplace::plain<int>>);
     *   static_assert( executor<feature::inplace::plain<int> const>);  // cv-ref stripped
     *   static_assert(!executor<int>);
     * @endcode
     */
    template <typename Type>
    concept executor = ::scl::feature::is_executor_v<Type>;
} // namespace scl::feature::concepts
