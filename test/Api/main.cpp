//#ifdef _MSC_VER
//#pragma warning( disable : 4552 4553 )
//#endif

#include <ScL/Feature.h>
#include <ScL/Utility.h>
#include <type_traits>
#include <vector>
#include <iostream>

#if defined( __GNUC__ )
#   include <cxxabi.h>
#endif

using namespace ::ScL;
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

    if ( ::std::is_const< _Type >{} )
        ::std::cout
            << " const";

    if ( ::std::is_lvalue_reference< _Type >{} )
        ::std::cout
            << " &";

    if ( ::std::is_rvalue_reference< _Type >{} )
        ::std::cout
            << " &&";

    ::std::cout
        << ::std::endl << ::std::flush;

#if defined( __GNUC__ )
    if ( status )
        free( realname );
#endif
}

struct TestType
{
    void rvalueMethod () && {}
    void rvalueConstMethod () const && {}
    void lvalueMethod () & {}
    void lvalueConstMethod () const & {}
};

void compileTestWrapperTrancpatancy ()
{
    using namespace ::ScL;
    using namespace ::ScL::Feature;

    TestType().rvalueMethod();
    TestType().rvalueConstMethod();
    asConst( TestType() ).rvalueConstMethod();

    // по правилам C++ такой код не собирается для временных экземпляров (taking address of temporary)
    //(*&TestType()).rvalueMethod();
    //(*&TestType()).rvalueConstMethod();
    //(*&castConst( TestType() ) ).rvalueConstMethod();

    // по похожим причинам с помощью обертки нельзя через operator -> вызвать rvalue методы
    //::ScL::Detail::ReferPointer< TestType && >( TestType() )->rvalueMethod();
    //::ScL::Detail::ReferPointer< TestType && >( TestType() )->rvalueConstMethod();
    //castConst( ::ScL::Detail::ReferPointer< TestType && >( TestType() ) )->rvalueConstMethod();

    // как следствие, не собирается код вида
    //Wrapper< TestType >()->rvalueMethod();
    //Wrapper< TestType >()->rvalueConstMethod();
    //castConst( Wrapper< TestType >() )->rvalueConstMethod();

    //TestType().lvalueMethod(); // T&& не во всех компиляторах преобразуется к T&
    TestType().lvalueConstMethod();
    asConst( TestType() ).lvalueConstMethod();

    Wrapper< TestType >()->lvalueMethod();
    Wrapper< TestType >()->lvalueConstMethod();
    asConst( Wrapper< TestType >() )->lvalueConstMethod();

    TestType value;
    Wrapper< TestType > instance;

    instance->lvalueMethod();
    instance->lvalueConstMethod();
    value.lvalueMethod();
    value.lvalueConstMethod();
}

extern void testWrapperValue ();
extern void testWrapperContainer ();

extern void testConstructors ();
extern void testSameToolConstructors ();
extern void testDiffToolConstructors ();

extern void testFeature ();
extern void testRangeOperators ();

int main ( int /*argc*/, char ** /*argv*/ )
{
    testWrapperValue();
    testWrapperContainer();

    testConstructors ();
    testSameToolConstructors ();
    testDiffToolConstructors ();
    testRangeOperators();

    compileTestWrapperTrancpatancy();
    testFeature();

    return 0;
}

void testWrapperValue ()
{
    { [[maybe_unused]] Wrapper< double > value; }
    { [[maybe_unused]] Wrapper< double > value{}; }
    { [[maybe_unused]] Wrapper< double > value{ double() }; }
    { [[maybe_unused]] Wrapper< double > value = double(); }
    { [[maybe_unused]] Wrapper< double > value{ Wrapper< double >() }; }
    { [[maybe_unused]] Wrapper< double > value = Wrapper< double >(); }
    // { [[maybe_unused]] Wrapper< double > value{ Wrapper< int >() }; }
    // { [[maybe_unused]] Wrapper< double > value = Wrapper< int >(); }
}

void testWrapperContainer ()
{
    //using Container = ::std::vector< double >;
    using Container = Wrapper< ::std::vector< double >, Implicit::Shared >;

    { [[maybe_unused]] Wrapper< Container > value; }
    { [[maybe_unused]] Wrapper< Container > value{{ 0.0, 1.0, 2.0, 3.0, 4.0 }}; }
    { [[maybe_unused]] Wrapper< Container > value = {{ 0.0, 1.0, 2.0, 3.0, 4.0 }}; }
    { [[maybe_unused]] Wrapper< Container > value{ Container{} }; }
    { [[maybe_unused]] Wrapper< Container > value = Container{}; }
    {
        Wrapper< Container > container;
        for ( int i = 0; i < 10; ++i )
            container.push_back( i );

        for ( auto value : container )
            value *= 2;

        auto v0 = container[5];
        auto v1 = asConst( container )[5];
        auto v2 = &(asConst( container )[5]);
        [[maybe_unused]]
        auto v3 = *v2;

        printTypeOf< decltype( v0 ) >();
        printTypeOf< decltype( v1 ) >();
        printTypeOf< decltype( v2 ) >();
        printTypeOf< decltype( v3 ) >();

        for ( int i = 0; i < 10; ++i )
            container[ i ] = asConst( container )[ 9 - i ];
    }
}

