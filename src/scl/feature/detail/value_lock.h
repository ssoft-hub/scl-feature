#pragma once

#include <scl/feature/concepts/wrapper.h>
#include <scl/feature/detail/wrapper_lock.h>
#include <scl/utility/concepts/reference.h>
#include <scl/utility/type_traits/forward_like.h>

#include <cassert>
#include <memory>
#include <type_traits>
#include <utility>

namespace scl::feature::detail
{
    enum class value_lock_case : bool
    {
        value = false,
        wrapper = true,
    };

    template <::scl::concepts::reference Refer,
        value_lock_case Case = ::scl::feature::is_wrapper_v<Refer> ? value_lock_case::wrapper : value_lock_case::value>
    class value_lock;

    template <::scl::concepts::reference Refer>
    class value_lock<Refer, value_lock_case::value>
    {
    public:
        value_lock(value_lock &&) = delete;
        value_lock(value_lock const &) = delete;
        value_lock & operator=(value_lock &&) = delete;
        value_lock & operator=(value_lock const &) = delete;
        ~value_lock() = default;

        constexpr explicit value_lock(Refer ref) noexcept
            : m_ref{::std::forward<Refer>(ref)}
        {}

        template <::std::convertible_to<Refer> Target>
        static constexpr void lock_for() noexcept
        {}

        template <::std::convertible_to<Refer> Target>
        [[nodiscard]]
        constexpr Target value_as() noexcept(::std::is_nothrow_convertible_v<Refer, Target>)
        {
            return ::std::forward<Refer>(m_ref);
        }

    private:
        Refer m_ref;
    };

    // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
    template <concepts::wrapper_reference WrapperRefer>
    class value_lock<WrapperRefer, value_lock_case::wrapper>
    {
        using wrapper_type = ::std::remove_cvref_t<WrapperRefer>;
        using value_type = typename wrapper_type::value_type;
        using value_refer = ::scl::forward_like_t<WrapperRefer, value_type>;

        using outer_lock_type = ::scl::feature::detail::wrapper_lock<WrapperRefer, wrapper_lock_case::wrapper>;
        using inner_lock_type = ::scl::feature::detail::value_lock<value_refer,
            ::scl::feature::is_wrapper_v<value_refer> ? value_lock_case::wrapper : value_lock_case::value>;

    public:
        value_lock(value_lock &&) = delete;
        value_lock(value_lock const &) = delete;
        value_lock & operator=(value_lock &&) = delete;
        value_lock & operator=(value_lock const &) = delete;

        constexpr explicit value_lock(WrapperRefer ref) /**/
            noexcept(noexcept(outer_lock_type{::std::declval<WrapperRefer>()}))
            : m_outer_lock{::std::forward<WrapperRefer>(ref)}
        {}

        constexpr ~value_lock() /**/
            noexcept(::std::is_nothrow_destructible_v<inner_lock_type> && ::std::is_nothrow_destructible_v<outer_lock_type>)
        {
            if (m_outer_lock.locked())
                ::std::destroy_at(::std::addressof(m_inner_lock));
        }

        template <concepts::convertible_from<WrapperRefer> Target>
        constexpr void lock_for() noexcept(lock_for_noexcept<Target>())
        {
            if constexpr (!::std::is_convertible_v<WrapperRefer, Target>)
            {
                m_outer_lock.lock();
                ::std::construct_at(::std::addressof(m_inner_lock), m_outer_lock.value());
                m_inner_lock.template lock_for<Target>();
            }
        }

        template <concepts::convertible_from<WrapperRefer> Target>
        [[nodiscard]]
        constexpr Target value_as() noexcept(value_as_noexcept<Target>())
        {
            if constexpr (!::std::is_convertible_v<WrapperRefer, Target>)
            {
                assert(m_outer_lock.locked() && "value_as<Target>() requires a prior lock_for<Target>()");
                return m_inner_lock.template value_as<Target>();
            }
            else
                return m_outer_lock.wrapper_value();
        }

    private:
        template <typename Target>
        static constexpr bool lock_for_noexcept() noexcept
        {
            if constexpr (::std::is_convertible_v<WrapperRefer, Target>)
                return true;
            else
                return noexcept(::std::declval<outer_lock_type &>().lock()) &&
                    noexcept(::std::declval<outer_lock_type &>().value()) &&
                    noexcept(inner_lock_type{::std::declval<outer_lock_type &>().value()}) &&
                    noexcept(::std::declval<inner_lock_type &>().template lock_for<Target>());
        }

        template <typename Target>
        static constexpr bool value_as_noexcept() noexcept
        {
            if constexpr (::std::is_convertible_v<WrapperRefer, Target>)
                return noexcept(::std::declval<outer_lock_type &>().wrapper_value());
            else
                return noexcept(::std::declval<inner_lock_type &>().template value_as<Target>());
        }

    private:
        outer_lock_type m_outer_lock;

        union
        {
            inner_lock_type m_inner_lock;
        };
    };
    // NOLINTEND(cppcoreguidelines-pro-type-union-access)

} // namespace scl::feature::detail
