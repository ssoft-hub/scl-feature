#pragma once
#ifndef SCL_FEATURE_CASTER_H
#define SCL_FEATURE_CASTER_H

#include <ScL/Feature/Access/ValueLock.h>
#include <ScL/Feature/Trait.h>
#include <ScL/Feature/Wrapper.h>
#include <ScL/Utility/SimilarRefer.h>
#include <utility>

#define ConceptCaster typename
#define ConceptReference typename
#define ConceptNonReference typename
#define ConceptWrapper typename
#define ConceptNonWrapper typename


namespace ScL::Feature::Detail
{
    template <ConceptCaster Caster_, ConceptReference Ref_>
    struct CastMixIn
    {
        operator Ref_ () && {
            return static_cast<Caster_ &&>(*this).template reference<Ref_>();
        }
    };

#define SCL_FEATURE_CAST_MIXIN( ref ) \
    template < typename Caster_, typename Type_, typename Tool_ > \
    struct CastMixIn< Caster_, Wrapper< Type_, Tool_ > ref > \
        : public CastMixIn< Caster_, Type_ ref > \
    { \
        operator Wrapper< Type_, Tool_ > ref () && { \
            return static_cast<Caster_ &&>(*this).template reference<Wrapper< Type_, Tool_ > ref>(); \
        } \
    }; \

    SCL_FEATURE_CAST_MIXIN( & )
    SCL_FEATURE_CAST_MIXIN( const & )
    SCL_FEATURE_CAST_MIXIN( volatile & )
    SCL_FEATURE_CAST_MIXIN( const volatile & )
    SCL_FEATURE_CAST_MIXIN( && )
    SCL_FEATURE_CAST_MIXIN( const && )
    SCL_FEATURE_CAST_MIXIN( volatile && )
    SCL_FEATURE_CAST_MIXIN( const volatile && )
}

namespace ScL::Feature::Detail
{
    template <ConceptReference Ref_>
    struct Caster : public ::ScL::Feature::Detail::CastMixIn<Caster<Ref_>, Ref_>
    {
        static_assert( ::std::is_reference_v<Ref_>, "Ref_ must be a reference type." );

        using Refer = Ref_;
        using Locker = ::ScL::Feature::ValueLock<Refer>;

        Locker m_locker;

        Caster ( Refer refer ) : m_locker{::std::forward<Refer>(refer)} {}

        template <ConceptReference Type_>
        Type_ reference () &&
        {
            m_locker.template lockFor<Type_>();
            return m_locker.template valueAccessFor<Type_>();
        }
    };
}

namespace ScL::Feature
{
    template <ConceptNonWrapper Type_>
    inline auto cast ( Type_ && value )
        -> ::std::enable_if_t<!::ScL::Feature::isWrapper< ::std::remove_reference_t<Type_>>(),
    Type_ &&>
    {
        return ::std::forward<Type_ &&>( value );
    }

    template <ConceptWrapper Wrapper_>
    inline auto cast ( Wrapper_ && wrapper )
        -> ::std::enable_if_t<::ScL::Feature::isWrapper< ::std::remove_reference_t< Wrapper_>>(),
    ::ScL::Feature::Detail::Caster< Wrapper_ &&>>
    {
        return ::ScL::Feature::Detail::Caster<Wrapper_ &&>( ::std::forward<Wrapper_ &&>( wrapper ) );
    }
}

#endif
