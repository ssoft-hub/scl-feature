#include <ScL/Feature/Wrapper.h>
#include <ScL/Feature/Tool.h>
#include <iostream>

struct BaseType
{
};

struct DerivedType
    : public BaseType
{
};

//#include <cxxabi.h>

template < typename _Type >
void bar ()
{
    ::std::cout << typeid( _Type ).name() << ::std::endl;

    //int status = 0;
    //char * realname = nullptr;
    //realname = abi::__cxa_demangle( typeid( _Type ).name(), 0, 0, &status );

    //::std::cout << realname << ::std::endl;

    //free( realname );
}

// Test of multi tool definition
void foo ()
{
    // Wrapper< Wrapper< Wrapper< BaseType, Implicit::Shared >, Heap::Unique >, ThreadSafe::RecursiveMutex > >
    using FirstType =
        ::ScL::Feature::Wrapper< ::ScL::Feature::Wrapper< ::ScL::Feature::Wrapper< BaseType
            , ::ScL::Feature::Implicit::Shared >
            , ::ScL::Feature::Heap::Unique >
            , ::ScL::Feature::ThreadSafe::RecursiveMutex >;

    // Wrapper< Wrapper< Wrapper< BaseType, Implicit::Shared >, Heap::Unique >, ThreadSafe::RecursiveMutex > >
    using SecondType = ::ScL::Feature::Wrapper< BaseType
        , ::ScL::Feature::Implicit::Shared
        , ::ScL::Feature::Heap::Unique
        , ::ScL::Feature::ThreadSafe::RecursiveMutex >;

    bar< FirstType >();
    bar< SecondType >();
    static_assert( ::std::is_same< FirstType, SecondType >::value, "" );
}

// Test of tool duplicate removing
void foofoo ()
{
    // Wrapper< Wrapper< Wrapper< BaseType, Implicit::Shared >, Heap::Unique >, ThreadSafe::RecursiveMutex > >
    using FirstType = ::ScL::Feature::Wrapper< ::ScL::Feature::Wrapper< ::ScL::Feature::Wrapper<
        ::ScL::Feature::Wrapper< ::ScL::Feature::Wrapper< ::ScL::Feature::Wrapper< ::ScL::Feature::Wrapper< BaseType
            , ::ScL::Feature::Implicit::Shared >
            , ::ScL::Feature::Implicit::Shared >
            , ::ScL::Feature::Heap::Unique >
            , ::ScL::Feature::Heap::Unique >
            , ::ScL::Feature::Heap::Unique >
            , ::ScL::Feature::Heap::Unique >
            , ::ScL::Feature::ThreadSafe::RecursiveMutex >;

    // Wrapper< Wrapper< Wrapper< BaseType, Implicit::Shared >, Heap::Unique >, ThreadSafe::RecursiveMutex > >
    using SecondType = ::ScL::Feature::Wrapper< BaseType
        , ::ScL::Feature::Implicit::Shared
        , ::ScL::Feature::Implicit::Shared
        , ::ScL::Feature::Implicit::Shared
        , ::ScL::Feature::Heap::Unique
        , ::ScL::Feature::Heap::Unique
        , ::ScL::Feature::ThreadSafe::RecursiveMutex
        , ::ScL::Feature::ThreadSafe::RecursiveMutex >;

    // Wrapper< Wrapper< Wrapper< BaseType, Implicit::Shared >, Heap::Unique >, ThreadSafe::RecursiveMutex > >
    using ThirdType = ::ScL::Feature::Wrapper< ::ScL::Feature::Wrapper< ::ScL::Feature::Wrapper< ::ScL::Feature::Wrapper< BaseType
            , ::ScL::Feature::Implicit::Shared >
            , ::ScL::Feature::Implicit::Shared
            , ::ScL::Feature::Heap::Unique >
            , ::ScL::Feature::Heap::Unique
            , ::ScL::Feature::Heap::Unique
            , ::ScL::Feature::Heap::Unique >
            , ::ScL::Feature::ThreadSafe::RecursiveMutex >;

    bar< FirstType >();
    bar< SecondType >();
    bar< ThirdType >();

    static_assert( ::std::is_same< FirstType, SecondType >::value, "" );
    static_assert( ::std::is_same< FirstType, ThirdType >::value, "" );
}

int main ( int /*argc*/, char ** /*argv*/ )
{
    foo();
    foofoo();

    static_assert( ::ScL::Feature::isCompatible< ::ScL::Feature::Wrapper< int >, ::ScL::Feature::Wrapper< int > >(), "" );
    ::std::cout
        << "isCompatible< ::ScL::Feature::Wrapper< int >, ::ScL::Feature::Wrapper< int > >()" << ::std::endl
        << true << " = "
        << ::ScL::Feature::isCompatible< ::ScL::Feature::Wrapper< int >, ::ScL::Feature::Wrapper< int > >() << ::std::endl;

    static_assert( !::ScL::Feature::isCompatible< ::ScL::Feature::Wrapper< int >, ::ScL::Feature::Wrapper< double > >(), "" );
    ::std::cout
        << "isCompatible< ::ScL::Feature::Wrapper< int >, ::ScL::Feature::Wrapper< double > >()" << ::std::endl
        << false << " = "
        << ::ScL::Feature::isCompatible< ::ScL::Feature::Wrapper< int >, ::ScL::Feature::Wrapper< double > >() << ::std::endl;

    static_assert( ::ScL::Feature::isCompatible< ::ScL::Feature::Wrapper< BaseType >, ::ScL::Feature::Wrapper< DerivedType > >(), "" );
    ::std::cout
        << "isCompatible< ::ScL::Feature::Wrapper< BaseType >, ::ScL::Feature::Wrapper< DerivedType > >()" << ::std::endl
        << true << " = "
        << ::ScL::Feature::isCompatible< ::ScL::Feature::Wrapper< BaseType >, ::ScL::Feature::Wrapper< DerivedType > >() << ::std::endl;

    static_assert( !::ScL::Feature::isCompatible< ::ScL::Feature::Wrapper< DerivedType >, ::ScL::Feature::Wrapper< BaseType > >(), "" );
    ::std::cout
        << "isCompatible< ::ScL::Feature::Wrapper< DerivedType >, ::ScL::Feature::Wrapper< BaseType > >()" << ::std::endl
        << false << " = "
        << ::ScL::Feature::isCompatible< ::ScL::Feature::Wrapper< DerivedType >, ::ScL::Feature::Wrapper< BaseType > >() << ::std::endl;

    using FirstType = ::ScL::Feature::Wrapper< BaseType
        , ::ScL::Feature::Implicit::Shared
        , ::ScL::Feature::Heap::Unique
        , ::ScL::Feature::ThreadSafe::RecursiveMutex >;

    using SecondType = ::ScL::Feature::Wrapper< DerivedType
        , ::ScL::Feature::Implicit::Shared
        , ::ScL::Feature::Heap::Unique
        , ::ScL::Feature::ThreadSafe::RecursiveMutex >;

    static_assert( ::ScL::Feature::isCompatible< FirstType, SecondType >(), "" );
    ::std::cout
        << "isCompatible< FirstType, SecondType >()" << ::std::endl
        << true << " = "
        << ::ScL::Feature::isCompatible< FirstType, SecondType >() << ::std::endl;

    static_assert( !::ScL::Feature::isCompatible< SecondType, FirstType >(), "" );
    ::std::cout
        << "isCompatible< SecondType, FirstType >()" << ::std::endl
        << false << " = "
        << ::ScL::Feature::isCompatible< SecondType, FirstType >() << ::std::endl;

    using FirstPartType = ::ScL::Feature::Wrapper< BaseType, ::ScL::Feature::Implicit::Shared >;
    using SecondPartType = ::ScL::Feature::Wrapper< DerivedType, ::ScL::Feature::Implicit::Shared >;

    static_assert( ::ScL::Feature::isThisPartOfOther< FirstPartType, FirstType >(), "" );
    ::std::cout
        << "isThisPartOfOther< FirstPartType, FirstType >()" << ::std::endl
        << true << " = "
        << ::ScL::Feature::isThisPartOfOther< FirstPartType, FirstType >() << ::std::endl;

    static_assert( !::ScL::Feature::isThisPartOfOther< SecondPartType, FirstType >(), "" );
    ::std::cout
        << "isThisPartOfOther< SecondPartType, FirstType >()" << ::std::endl
        << false << " = "
        << ::ScL::Feature::isThisPartOfOther< SecondPartType, FirstType >() << ::std::endl;

    static_assert( ::ScL::Feature::isThisPartOfOther< FirstPartType, SecondType >(), "" );
    ::std::cout
        << "isThisPartOfOther< FirstPartType, SecondType >()" << ::std::endl
        << true << " = "
        << ::ScL::Feature::isThisPartOfOther< FirstPartType, SecondType >() << ::std::endl;

    static_assert( ::ScL::Feature::isThisPartOfOther< SecondPartType, SecondType >(), "" );
    ::std::cout
        << "isThisPartOfOther< SecondPartType, SecondType >()" << ::std::endl
        << true << " = "
        << ::ScL::Feature::isThisPartOfOther< SecondPartType, SecondType >() << ::std::endl;

    static_assert( !::ScL::Feature::isThisPartOfOther< FirstType, FirstType >(), "" );
    ::std::cout
        << "isThisPartOfOther< FirstType, FirstType >()" << ::std::endl
        << false << " = "
        << ::ScL::Feature::isThisPartOfOther< FirstType, FirstType >() << ::std::endl;

    static_assert( !::ScL::Feature::isThisPartOfOther< FirstType, SecondType >(), "" );
    ::std::cout
        << "isThisPartOfOther< FirstType, SecondType >()" << ::std::endl
        << false << " = "
        << ::ScL::Feature::isThisPartOfOther< FirstType, SecondType >() << ::std::endl;

    static_assert( !::ScL::Feature::isThisPartOfOther< SecondType, FirstType >(), "" );
    ::std::cout
        << "isThisPartOfOther< SecondType, FirstType >()" << ::std::endl
        << false << " = "
        << ::ScL::Feature::isThisPartOfOther< SecondType, FirstType >() << ::std::endl;

    static_assert( !::ScL::Feature::isThisPartOfOther< FirstPartType, SecondPartType >(), "" );
    ::std::cout
        << "isThisPartOfOther< FirstPartType, SecondPartType >()" << ::std::endl
        << false << " = "
        << ::ScL::Feature::isThisPartOfOther< FirstPartType, SecondPartType >() << ::std::endl;

    FirstType first_value;
    FirstPartType first_part_value;

    FirstType check_value( first_part_value );
    FirstPartType check_part_value( first_value );

    return 0;
}
