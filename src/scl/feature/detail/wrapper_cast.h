#pragma once

#include <scl/feature/detail/value_lock.h>
#include <scl/feature/type_traits/wrapper.h>
#include <scl/utility/type_traits/forward_like.h>

#include <type_traits>
#include <utility>

namespace scl::feature::detail
{
    enum class wrapper_cast_case : bool
    {
        value = false,
        wrapper = true,
    };

    // -------------------------------------------------------------------------
    // cast_mixin — CRTP base that injects one implicit conversion operator per
    // reachable level in the wrapper chain.
    //
    // The bool parameter IsWrapper routes to the two specialisations below,
    // avoiding macros while covering all cv-ref combinations.
    // -------------------------------------------------------------------------

    template <typename Caster,
        typename Refer,
        wrapper_cast_case Case =
            ::scl::feature::is_wrapper_v<::std::remove_cvref_t<Refer>> ? wrapper_cast_case::wrapper : wrapper_cast_case::value>
        requires ::std::is_reference_v<Refer>
    class cast_mixin;

    // Non-wrapper leaf: one operator for the plain reference type.
    template <typename Caster, typename Refer>
        requires ::std::is_reference_v<Refer>
    class cast_mixin<Caster, Refer, wrapper_cast_case::value>
    {
    public:
        operator Refer() && { return static_cast<Caster &&>(*this).template get<Refer>(); }
    };

    // Wrapper level: one operator for the wrapper reference itself plus
    // inherited operators for every inner level via recursive base.
    template <typename Caster, typename WrapperRefer>
        requires ::std::is_reference_v<WrapperRefer> &&
        ::scl::feature::is_wrapper_v<::std::remove_cvref_t<WrapperRefer>>
    class cast_mixin<Caster, WrapperRefer, wrapper_cast_case::wrapper>
        : public cast_mixin<Caster, ::scl::forward_like_t<WrapperRefer, typename ::std::remove_cvref_t<WrapperRefer>::value_type>>
    {
    public:
        operator WrapperRefer() &&
        {
            return static_cast<Caster &&>(*this).template get<WrapperRefer>();
        }
    };

    // -------------------------------------------------------------------------
    // wrapper_caster — lazy-locking proxy returned by wrapper_cast().
    //
    // Constructs a value_lock (all executor references stored, no guard yet).
    // Each implicit conversion operator (provided by cast_mixin) calls get<T>()
    // which activates the necessary guards just before returning the value.
    //
    // All conversions are &&-qualified: the caster must be used as an rvalue.
    // Copy and move constructors are deleted to prevent moving the lock state.
    // -------------------------------------------------------------------------

    template <typename Refer>
        requires ::std::is_reference_v<Refer>
    class [[nodiscard]] wrapper_caster : public cast_mixin<wrapper_caster<Refer>, Refer>
    {
        using lock_type = ::scl::feature::detail::value_lock<Refer,
            ::scl::feature::is_wrapper_v<::std::remove_cvref_t<Refer>> ? value_lock_case::wrapper : value_lock_case::value>;

    public:
        wrapper_caster(wrapper_caster &&) = delete;
        wrapper_caster(wrapper_caster const &) = delete;
        wrapper_caster & operator=(wrapper_caster &&) = delete;
        wrapper_caster & operator=(wrapper_caster const &) = delete;
        ~wrapper_caster() = default;

        constexpr explicit wrapper_caster(Refer ref)
            : m_lock{::std::forward<Refer>(ref)}
        {}

        /// Activates guards for layers needed to reach @p T, then returns the value.
        /// Called by cast_mixin's conversion operators.
        template <typename T>
        T get() &&
        {
            m_lock.template lock_for<T>();
            return m_lock.template value_as<T>();
        }

    private:
        lock_type m_lock;
    };

} // namespace scl::feature::detail
