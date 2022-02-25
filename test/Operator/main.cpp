#include <ScL/Feature.h>

class Test
{
    int m_int;
public:
    const int & operator [] ( int ) const
    {
        return m_int;
    }
};

using namespace ::ScL::Feature;
using namespace ::ScL::Meta;

static_assert( Detector< SquareBracketsUnstrictOperation, Test, int >::isDetected(), "" );
//static_assert( Detector< SquareBracketsMemberStrictOperation, Test, int >::isDetected(), "" );


#include <iomanip>
#include <ostream>
#include <vector>

::std::ostream & my ( ::std::ostream & stream )
{
    return stream;
}

void wrapper ()
{
    using Test = ::std::vector< int >;
    Wrapper< Test > test;
    auto n = test[ 1.0 ];
    ::std::cout << ::std::setbase( 16 );
    ::std::cout << n << n << test[ 1 ] << "\n";

    Wrapper< decltype( ::std::cout ) > * stream_ptr = nullptr;
    auto & stream = *stream_ptr;
    stream << ::std::setbase( 16 );

    using Stream = decltype( ::std::cout );
    using WStream = Wrapper< decltype( ::std::cout ) >;
    using Endl = decltype( ::std::endl( ::std::declval< Stream & >() ) )(*)( Stream & );

    stream.operator << < Endl >( ::std::endl );
    stream << Endl( ::std::endl );

    static_assert( ::ScL::Meta::Detector< ::ScL::Meta::ShiftLeftUnstrictOperation, WStream, Endl >::isDetected(), "" );

}

void test ()
{
    Test test;
    auto n = test[ 1.0 ];
    ::std::cout << n << ::std::endl;
}

int main ( int, char ** )
{
    wrapper();
    return 0;
}
