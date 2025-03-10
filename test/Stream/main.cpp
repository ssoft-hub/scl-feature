#include <ScL/Feature.h>
#include <iostream>
#include <type_traits>

#if defined( __GNUC__ )
#   include <cxxabi.h>
#endif

using namespace ::ScL::Feature;

template < typename _Type >
void printTypeOf ()
{
#if defined( __GNUC__ )
    int status = 0;
    char * realname = abi::__cxa_demangle( typeid( _Type ).name(), nullptr, nullptr, &status );
#elif defined( _MSC_VER )
    const char * realname = typeid( _Type ).name();
#endif

    ::std::cout
        << realname;

    if ( ::std::is_const< _Type >::value )
        ::std::cout
            << " const";

    if ( ::std::is_lvalue_reference< _Type >::value )
        ::std::cout
            << " &";

    if ( ::std::is_rvalue_reference< _Type >::value )
        ::std::cout
            << " &&";

    ::std::cout
        << ::std::endl << ::std::flush;

#if defined( __GNUC__ )
    if ( status )
        free( realname );
#endif
}

class Stream
{
public:
//    template < typename _Type >
//    Stream & operator << ( const _Type & ) { return *this; }

    Stream & operator << ( int ) { return *this; }
};

int main ( int /*argc*/, char ** /*argv*/ )
{
    using Int = Wrapper< int >;
    using WStream = Stream;

    WStream s;
    Int i;

    s << i;

    return 0;
}
