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
    template < typename _WrapperRefer >
    struct Caster
    {
        static_assert( ::ScL::Feature::isWrapper< ::std::decay_t< _WrapperRefer > >() );
        using WrapperRefer = _WrapperRefer;
        using Wrapper = ::std::decay_t< WrapperRefer >;
        using ValueRefer = ::ScL::SimilarRefer< typename Wrapper::Value, WrapperRefer >;

        _WrapperRefer m_wrapper;

        Caster ( _WrapperRefer wrapper_refer )
            : m_wrapper( ::std::forward< _WrapperRefer >( wrapper_refer ) )
        {}

        operator ValueRefer () &&
        {
            return ::std::forward< ValueRefer >( *&m_wrapper );
        }

    //    template < typename _Type >
    //    operator ::ScL::SimilarRefer< _Type, WrapperRefer > () &&
    //    {
    //        return ::std::forward< ValueRefer >( *&m_wrapper );
    //    }
    };
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
    using MyWrapper = ::ScL::Feature::Wrapper< MyData >;
    using ::ScL::Feature::cast;

    MyWrapper data;
    MyWrapper const c_data;
    MyWrapper volatile v_data;
    MyWrapper const volatile cv_data;

    foo( cast( data ) );
    foo( cast( ::std::move( data ) ) );
    foo( cast( c_data ) );
    foo( cast( ::std::move( c_data ) ) );
    foo( cast( v_data ) );
    foo( cast( ::std::move( v_data ) ) );
    foo( cast( cv_data ) );
    foo( cast( ::std::move( cv_data ) ) );

    return 0;
}
