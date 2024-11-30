#pragma once
#ifndef SCL_FEATURE_ACCESS_VALUE_GUARD_H
#define SCL_FEATURE_ACCESS_VALUE_GUARD_H

#include <ScL/Feature/Access/WrapperGuard.h>
#include <ScL/Feature/Trait.h>
#include <ScL/Utility/SimilarRefer.h>

namespace ScL { namespace Feature { namespace Detail { template < typename _Value, typename _Tool > class Wrapper; }}}
namespace ScL { namespace Feature { namespace Detail { template < typename > struct ValueGuardHelper; }}}

namespace ScL { namespace Feature
{
    //! Указатель на экземпляр вложенного в Wrapper базового значения, к которому применены
    /// все особенности, реализуемые посредством используемых Wrapper.
    template < typename _Refer >
    using ValueGuard = typename ::ScL::Feature::Detail::ValueGuardHelper< _Refer >::Type;
}}

namespace ScL { namespace Feature { namespace Detail
{
    /*!
     * Указатель на экземпляр вложенного в Wrapper базового значения, к которому применены
     * все особенности, реализуемые посредством используемых Wrapper. Данный указатель применяется, если
     * тип вложенного экземпляра значения сам не является Wrapper.
     */
    template < typename _Refer >
    class DefaultValueGuard
    {
        using ThisType = DefaultValueGuard< _Refer >;

    public:
        using Refer = _Refer;
        using WrapperGuard = ::ScL::Feature::Detail::WrapperGuard< _Refer >;

        using ValueAccess = typename WrapperGuard::ValueAccess;
        using PointerAccess = typename WrapperGuard::PointerAccess;

        static_assert( ::std::is_reference< Refer >::value, "The template parameter _Refer must to be a reference type." );
        static_assert( !::ScL::Feature::isWrapper< ::std::decay_t< Refer > >(), "The template parameter _Refer must to be a not Wrapper type reference!" );

    private:
        WrapperGuard m_wrapper_guard;

    public:
        DefaultValueGuard ( Refer refer )
            : m_wrapper_guard( ::std::forward< Refer >( refer ) )
        {
        }

        DefaultValueGuard ( WrapperGuard && other )
            : m_wrapper_guard( ::std::forward< WrapperGuard >( other ) )
        {
        }

        DefaultValueGuard ( ThisType && other )
            : m_wrapper_guard( ::std::forward< WrapperGuard >( other.m_wrapper_guard ) )
        {
        }

        ValueAccess valueAccess () const
        {
            return m_wrapper_guard.valueAccess();
        }

        PointerAccess pointerAccess () const
        {
            return m_wrapper_guard.pointerAccess();
        }

        operator ValueAccess () const
        {
            return valueAccess();
        }
    };
}}}

namespace ScL { namespace Feature { namespace Detail
{
    template < typename _Refer >
    struct ValueGuardHelper
    {
        using Type = DefaultValueGuard< _Refer >;
    };
}}}

namespace ScL { namespace Feature { namespace Detail
{
    /*
     * Указатель на экземпляр вложенного в Wrapper базового значения, к которому применены
     * все особенности, реализуемые посредством используемых Wrapper. Данный указатель применяется, если
     * тип вложенного экземпляра значения сам является Wrapper.
     */
    template < typename _Refer >
    class SpecialValueGuard
    {
        using ThisType = SpecialValueGuard< _Refer >;

    public:
        using WrapperRefer = _Refer;
        using Wrapper = ::std::decay_t< WrapperRefer >;
        using Value = typename Wrapper::Value;
        using ValueRefer = ::ScL::SimilarRefer< Value, WrapperRefer >;
        using Holder = typename Wrapper::Holder;
        using HolderRefer = ::ScL::SimilarRefer< Holder, WrapperRefer >;

        using WrapperGuard = ::ScL::Feature::Detail::WrapperGuard< WrapperRefer >;
        using ValueGuard = ::ScL::Feature::ValueGuard< ValueRefer >;

        using WrapperAccess = typename WrapperGuard::WrapperAccess;
        using ValueAccess =  typename ValueGuard::ValueAccess;
        using PointerAccess = typename ValueGuard::PointerAccess;

        static_assert( ::std::is_reference< WrapperRefer >::value, "The template parameter _Refer must to be a reference type." );
        static_assert( ::ScL::Feature::isWrapper< Wrapper >(), "The template parameter _Refer must to be a Wrapper type reference!" );
        //static_assert( ::ScL::Feature::isSimilar< ValueRefer, WrapperRefer >(), "The Refer and ValueRefer must to be similar types!" );
        static_assert( ::ScL::Feature::isSimilar< HolderRefer, WrapperRefer >(), "The Refer and HolderRefer must to be similar types!" );

    private:
        WrapperGuard m_wrapper_guard;
        ValueGuard m_value_guard;

    public:
        SpecialValueGuard ( WrapperRefer refer )
            : m_wrapper_guard( ::std::forward< WrapperRefer >( refer ) )
            , m_value_guard( ::ScL::Feature::Detail::HolderInterface::value< HolderRefer >( m_wrapper_guard.holderAccess() ) )
        {
        }

        SpecialValueGuard ( WrapperGuard && other )
            : m_wrapper_guard( ::std::forward< WrapperGuard >( other ) )
            , m_value_guard( ::ScL::Feature::Detail::HolderInterface::value< HolderRefer >( m_wrapper_guard.holderAccess() ) )
        {
        }

        SpecialValueGuard ( ThisType && other )
            : m_wrapper_guard( ::std::forward< WrapperGuard >( other.m_wrapper_guard ) )
            , m_value_guard( ::std::forward< ValueGuard >( other.m_value_guard ) )
        {
        }

        WrapperAccess wrapperAccess () const
        {
            return m_wrapper_guard.wrapperAccess();
        }

        ValueAccess valueAccess () const
        {
            return m_value_guard.valueAccess();
        }

        PointerAccess pointerAccess () const
        {
            return m_value_guard.pointerAccess();
        }

        operator ValueAccess () const
        {
            return valueAccess();
        }
    };
}}}

namespace ScL { namespace Feature { namespace Detail
{
    template < typename _Value, typename _Tool >
    struct ValueGuardHelper< ::ScL::Feature::Detail::Wrapper< _Value, _Tool > & >
    {
        using Type = ::ScL::Feature::Detail::SpecialValueGuard< ::ScL::Feature::Detail::Wrapper< _Value, _Tool > & >;
    };

    template < typename _Value, typename _Tool >
    struct ValueGuardHelper< ::ScL::Feature::Detail::Wrapper< _Value, _Tool > && >
    {
        using Type = ::ScL::Feature::Detail::SpecialValueGuard< ::ScL::Feature::Detail::Wrapper< _Value, _Tool > && >;
    };

    template < typename _Value, typename _Tool >
    struct ValueGuardHelper< const ::ScL::Feature::Detail::Wrapper< _Value, _Tool > & >
    {
        using Type = ::ScL::Feature::Detail::SpecialValueGuard< const ::ScL::Feature::Detail::Wrapper< _Value, _Tool > & >;
    };

    template < typename _Value, typename _Tool >
    struct ValueGuardHelper< const ::ScL::Feature::Detail::Wrapper< _Value, _Tool > && >
    {
        using Type = ::ScL::Feature::Detail::SpecialValueGuard< const ::ScL::Feature::Detail::Wrapper< _Value, _Tool > && >;
    };

    template < typename _Value, typename _Tool >
    struct ValueGuardHelper< volatile ::ScL::Feature::Detail::Wrapper< _Value, _Tool > & >
    {
        using Type = ::ScL::Feature::Detail::SpecialValueGuard< volatile ::ScL::Feature::Detail::Wrapper< _Value, _Tool > & >;
    };

    template < typename _Value, typename _Tool >
    struct ValueGuardHelper< volatile ::ScL::Feature::Detail::Wrapper< _Value, _Tool > && >
    {
        using Type = ::ScL::Feature::Detail::SpecialValueGuard< volatile ::ScL::Feature::Detail::Wrapper< _Value, _Tool > && >;
    };

    template < typename _Value, typename _Tool >
    struct ValueGuardHelper< const volatile ::ScL::Feature::Detail::Wrapper< _Value, _Tool > & >
    {
        using Type = ::ScL::Feature::Detail::SpecialValueGuard< const volatile ::ScL::Feature::Detail::Wrapper< _Value, _Tool > & >;
    };

    template < typename _Value, typename _Tool >
    struct ValueGuardHelper< const volatile ::ScL::Feature::Detail::Wrapper< _Value, _Tool > && >
    {
        using Type = ::ScL::Feature::Detail::SpecialValueGuard< const volatile ::ScL::Feature::Detail::Wrapper< _Value, _Tool > && >;
    };


    // disabled
    template < typename _Value, typename _Tool >
    struct ValueGuardHelper< ::ScL::Feature::Detail::Wrapper< _Value, _Tool > > {};
    template < typename _Value, typename _Tool >
    struct ValueGuardHelper< const ::ScL::Feature::Detail::Wrapper< _Value, _Tool > > {};
    template < typename _Value, typename _Tool >
    struct ValueGuardHelper< volatile ::ScL::Feature::Detail::Wrapper< _Value, _Tool > > {};
    template < typename _Value, typename _Tool >
    struct ValueGuardHelper< const volatile ::ScL::Feature::Detail::Wrapper< _Value, _Tool > > {};
}}}

#endif
