#include <ScL/Feature.h>

struct MyData {};

void foo ( MyData && ) { ::std::cout << "MyData && " << ::std::endl; }
void foo ( MyData const && ) { ::std::cout << "MyData const && " << ::std::endl; }
void foo ( MyData & ) { ::std::cout << "MyData & " << ::std::endl; }
void foo ( MyData const & ) { ::std::cout << "MyData const & " << ::std::endl; }
void foo ( MyData volatile && ) { ::std::cout << "MyData volatile && " << ::std::endl; }
void foo ( MyData const volatile && ) { ::std::cout << "MyData const volatile && " << ::std::endl; }
void foo ( MyData volatile & ) { ::std::cout << "MyData volatile & " << ::std::endl; }
void foo ( MyData const volatile & ) { ::std::cout << "MyData const volatile & " << ::std::endl; }

namespace ScL::Feature::Detail
{
    template < typename Type_ >
    struct Caster
    {
        using ValueRefer = Type_;

        ValueRefer m_refer;

        Caster ( ValueRefer refer )
            : m_refer{ ::std::forward< ValueRefer >( refer ) }
        {}

        operator ValueRefer () &&
        {
            return ::std::forward< ValueRefer >( m_refer );
        }
    };

    template <typename Type_>
    Type_ refer ()
    {
        static ::std::decay_t<Type_> value;
        return ::std::forward<Type_>(value);
    }

#define CASTER( ref ) \
    template < typename Type_, typename Tool_ > \
    struct Caster< Wrapper< Type_, Tool_ > ref > : Caster< Type_ ref > \
    { \
        using WrapperRefer = Wrapper< Type_, Tool_ > ref; \
        using WrapperType = ::std::decay_t< WrapperRefer >; \
        using ValueRefer = ::ScL::SimilarRefer< typename WrapperType::Value, WrapperRefer >; \
     \
        WrapperRefer m_wrapper_refer; \
     \
        Caster ( WrapperRefer wrapper_refer ) \
            : Caster< Type_ ref >{ ::std::forward< Type_ ref >( refer<Type_ ref>()/*wrapper_refer.m_value*/ ) } \
            , m_wrapper_refer{ ::std::forward< WrapperRefer >( wrapper_refer ) } \
        {} \
     \
        operator ValueRefer () && \
        { \
            return ::std::forward< ValueRefer >( refer<Type_ ref>() ); \
        } \
    }; \

    CASTER( & )
    CASTER( const & )
    CASTER( volatile & )
    CASTER( const volatile & )
    CASTER( && )
    CASTER( const && )
    CASTER( volatile && )
    CASTER( const volatile && )
}

namespace ScL::Feature
{
    template < typename _Wrapper,
        typename = ::std::enable_if_t< ::ScL::Feature::isWrapper< ::std::decay_t< _Wrapper > >() > >
    inline ::ScL::Feature::Detail::Caster< _Wrapper && > cast ( _Wrapper && wrapper )
    {
        return ::std::forward< _Wrapper && >( wrapper );
    }
}

int main ( int, char ** )
{
    using MyWrapper = ::ScL::Feature::Wrapper< MyData, ::ScL::Feature::Inplace::Optional, ::ScL::Feature::Implicit::Raw >;
    using ::ScL::Feature::cast;

    MyWrapper data;
    MyWrapper const c_data;
    MyWrapper volatile v_data;
    MyWrapper const volatile cv_data;

    foo( cast( data ) );
    foo( cast( ::std::move( data ) ) );
    foo( cast( c_data ) );

    // CLANG makes error without static_cast
    // MSVC and GCC do not require static_cast
    foo( static_cast< MyData const & >( cast( ::std::move( c_data ) ) ) );
    foo( static_cast< MyData volatile & >( cast( v_data ) ) );
    foo( static_cast< MyData volatile && >( cast( ::std::move( v_data ) ) ) );
    foo( static_cast< MyData const volatile & >( cast( cv_data ) ) );
    foo( static_cast< MyData const volatile && >( cast( ::std::move( cv_data ) ) ) );

    return 0;
}
