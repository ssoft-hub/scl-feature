#include <ScL/Feature.h>

struct MyData {};

void foo ( MyData & ) { ::std::cout << "MyData & " << ::std::endl; }
void foo ( MyData const & ) { ::std::cout << "MyData const & " << ::std::endl; }
void foo ( MyData volatile & ) { ::std::cout << "MyData volatile & " << ::std::endl; }
void foo ( MyData const volatile & ) { ::std::cout << "MyData const volatile & " << ::std::endl; }
void foo ( MyData && ) { ::std::cout << "MyData && " << ::std::endl; }
void foo ( MyData const && ) { ::std::cout << "MyData const && " << ::std::endl; }
void foo ( MyData volatile && ) { ::std::cout << "MyData volatile && " << ::std::endl; }
void foo ( MyData const volatile && ) { ::std::cout << "MyData const volatile && " << ::std::endl; }

template <typename Type_>
void testCast ()
{
    using ::ScL::Feature::cast;

    Type_ data;
    Type_ const c_data;
    Type_ volatile v_data;
    Type_ const volatile cv_data;

    foo( cast( data ) );
    foo( cast( ::std::move( data ) ) );
    foo( cast( c_data ) );

    // CLANG makes error without static_cast
    // MSVC and GCC do not require static_cast
    foo( static_cast< MyData const && >( cast( ::std::move( c_data ) ) ) );
    foo( static_cast< MyData volatile & >( cast( v_data ) ) );
    foo( static_cast< MyData volatile && >( cast( ::std::move( v_data ) ) ) );
    foo( static_cast< MyData const volatile & >( cast( cv_data ) ) );
    foo( static_cast< MyData const volatile && >( cast( ::std::move( cv_data ) ) ) );
}

int main ( int, char ** )
{
    using namespace ::ScL::Feature;

    testCast<MyData>();
    testCast<Wrapper<MyData, Inplace::Optional>>();
    testCast<Wrapper<MyData, Inplace::Optional, Implicit::Raw>>();
    testCast<Wrapper<MyData, Inplace::Debug, Inplace::Optional, Implicit::Raw>>();

    return 0;
}
