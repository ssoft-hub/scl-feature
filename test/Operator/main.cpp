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

static_assert( isDetected< SquareBracketsUnstrictOperation, Test, int >(), "" );
static_assert( isDetected< SquareBracketsMemberStrictOperation, Test const, int >(), "" );


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
    Wrapper< Test > test{{0,1,2,3,4,5}};
    Wrapper< int > i{1};
    auto n = test[ 1.0 ];
    ::std::cout << ::std::setbase( 16 );

    static_assert( ::ScL::Meta::isDetected< ::ScL::Meta::ShiftLeftMemberStrictOperation, decltype( ::std::cout ), int >(), "" );
    //static_assert( ::ScL::Feature::Detail::Operator::Binary::DoesShiftLeftOperatorExist< decltype( ::std::cout ) &, Wrapper< int > & >::value, "" );

    ::std::cout << i;
    ::std::cout << n << n << test[ 1 ] << ::std::flush << ::std::endl;

    //using SStream = Wrapper< std::ostringstream >;
    using SStream = std::ostringstream;

    SStream sstream;
    sstream << n << n << test[ 1 ] << ::std::flush << ::std::endl;

    using Stream = decltype( ::std::cout );
    using Endl = decltype( ::std::endl( ::std::declval< Stream & >() ) )(*)( Stream & );

    //sstream.operator << < Endl >( ::std::endl );
    sstream << Endl( ::std::endl );
    sstream << ::std::endl;

    static_assert( ::ScL::Meta::isDetected< ::ScL::Meta::ShiftLeftUnstrictOperation, SStream, Endl >(), "" );

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
