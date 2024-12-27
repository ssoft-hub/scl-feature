#ifndef SCL_FEATURE_CASTER_H
#define SCL_FEATURE_CASTER_H

#include <ScL/Feature/Trait.h>
#include <ScL/Utility/SimilarRefer.h>
#include <utility>

namespace ScL::Feature::Detail
{
    template < typename Refer_ >
    struct Caster
    {
        using ValueRefer = Refer_;

        ValueRefer m_refer;

        Caster ( ValueRefer refer )
            : m_refer{ ::std::forward< ValueRefer >( refer ) }
        {}

        operator ValueRefer () &&
        {
            return ::std::forward< ValueRefer >( m_refer );
        }
    };

    //TODO:
    template <typename Type_>
    Type_ refer ()
    {
        static ::std::decay_t<Type_> value;
        return ::std::forward<Type_>(value);
    }

#define SCL_FEATURE_CASTER( ref ) \
    template < typename Type_, typename Tool_ > \
    struct Caster< Wrapper< Type_, Tool_ > ref > : Caster< Type_ ref > \
    { \
        using WrapperRefer = Wrapper< Type_, Tool_ > ref; \
        using WrapperType = ::std::decay_t< WrapperRefer >; \
        using ValueRefer = ::ScL::SimilarRefer< typename WrapperType::Value, WrapperRefer >; \
        using ParentCaster = Caster< Type_ ref >; \
     \
        WrapperRefer m_wrapper_refer; \
     \
        Caster ( WrapperRefer wrapper_refer ) \
            : ParentCaster{ ::std::forward< Type_ ref >( refer<Type_ ref>()/*wrapper_refer.m_value*/ ) } \
            , m_wrapper_refer{ ::std::forward< WrapperRefer >( wrapper_refer ) } \
        {} \
     \
        operator ValueRefer () && \
        { \
            return ::std::forward< ValueRefer >( refer<Type_ ref>() ); \
        } \
    }; \

    SCL_FEATURE_CASTER( & )
    SCL_FEATURE_CASTER( const & )
    SCL_FEATURE_CASTER( volatile & )
    SCL_FEATURE_CASTER( const volatile & )
    SCL_FEATURE_CASTER( && )
    SCL_FEATURE_CASTER( const && )
    SCL_FEATURE_CASTER( volatile && )
    SCL_FEATURE_CASTER( const volatile && )
}

namespace ScL::Feature
{
    template < typename Type_ >
    inline auto cast ( Type_ && value )
        -> ::std::enable_if_t< !::ScL::Feature::isWrapper< ::std::decay_t< Type_ > >(), Type_ && >
    {
        return ::std::forward< Type_ && >( value );
    }

    template < typename _Wrapper >
    inline auto cast ( _Wrapper && wrapper )
        -> ::std::enable_if_t< ::ScL::Feature::isWrapper< ::std::decay_t< _Wrapper > >(),
            ::ScL::Feature::Detail::Caster< _Wrapper && > >
    {
        return ::std::forward< _Wrapper && >( wrapper );
    }
}

#endif // CASTER_H
