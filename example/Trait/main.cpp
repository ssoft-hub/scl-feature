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
    static_assert( ::std::is_same< FirstType, SecondType >{}, "" );
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

    static_assert( ::std::is_same< FirstType, SecondType >{}, "" );
    static_assert( ::std::is_same< FirstType, ThirdType >{}, "" );
}

int main ( int /*argc*/, char ** /*argv*/ )
{
    foo();
    foofoo();

    static_assert( ::ScL::Feature::isThisCompatibleWithOther< ::ScL::Feature::Wrapper< int >, ::ScL::Feature::Wrapper< int > >(), "" );
    ::std::cout
        << "isThisCompatibleWithOther< ::ScL::Feature::Wrapper< int >, ::ScL::Feature::Wrapper< int > >()" << ::std::endl
        << true << " = "
        << ::ScL::Feature::isThisCompatibleWithOther< ::ScL::Feature::Wrapper< int >, ::ScL::Feature::Wrapper< int > >() << ::std::endl;

    static_assert( !::ScL::Feature::isThisCompatibleWithOther< ::ScL::Feature::Wrapper< int >, ::ScL::Feature::Wrapper< double > >(), "" );
    ::std::cout
        << "isThisCompatibleWithOther< ::ScL::Feature::Wrapper< int >, ::ScL::Feature::Wrapper< double > >()" << ::std::endl
        << false << " = "
        << ::ScL::Feature::isThisCompatibleWithOther< ::ScL::Feature::Wrapper< int >, ::ScL::Feature::Wrapper< double > >() << ::std::endl;

    static_assert( !::ScL::Feature::isThisCompatibleWithOther< ::ScL::Feature::Wrapper< BaseType >, ::ScL::Feature::Wrapper< DerivedType > >(), "" );
    ::std::cout
        << "isThisCompatibleWithOther< ::ScL::Feature::Wrapper< BaseType >, ::ScL::Feature::Wrapper< DerivedType > >()" << ::std::endl
        << false << " = "
        << ::ScL::Feature::isThisCompatibleWithOther< ::ScL::Feature::Wrapper< BaseType >, ::ScL::Feature::Wrapper< DerivedType > >() << ::std::endl;

    static_assert( ::ScL::Feature::isThisCompatibleWithOther< ::ScL::Feature::Wrapper< DerivedType >, ::ScL::Feature::Wrapper< BaseType > >(), "" );
    ::std::cout
        << "isThisCompatibleWithOther< ::ScL::Feature::Wrapper< DerivedType >, ::ScL::Feature::Wrapper< BaseType > >()" << ::std::endl
        << true << " = "
        << ::ScL::Feature::isThisCompatibleWithOther< ::ScL::Feature::Wrapper< DerivedType >, ::ScL::Feature::Wrapper< BaseType > >() << ::std::endl;

    using FirstType = ::ScL::Feature::Wrapper< BaseType
        , ::ScL::Feature::Implicit::Shared
        , ::ScL::Feature::Heap::Unique
        , ::ScL::Feature::ThreadSafe::RecursiveMutex >;

    using SecondType = ::ScL::Feature::Wrapper< DerivedType
        , ::ScL::Feature::Implicit::Shared
        , ::ScL::Feature::Heap::Unique
        , ::ScL::Feature::ThreadSafe::RecursiveMutex >;

    static_assert( !::ScL::Feature::isThisCompatibleWithOther< FirstType, SecondType >(), "" );
    ::std::cout
        << "isThisCompatibleWithOther< FirstType, SecondType >()" << ::std::endl
        << false << " = "
        << ::ScL::Feature::isThisCompatibleWithOther< FirstType, SecondType >() << ::std::endl;

    static_assert( ::ScL::Feature::isThisCompatibleWithOther< SecondType, FirstType >(), "" );
    ::std::cout
        << "isThisCompatibleWithOther< SecondType, FirstType >()" << ::std::endl
        << true << " = "
        << ::ScL::Feature::isThisCompatibleWithOther< SecondType, FirstType >() << ::std::endl;

    using FirstPartType = ::ScL::Feature::Wrapper< BaseType, ::ScL::Feature::Implicit::Shared >;
    using SecondPartType = ::ScL::Feature::Wrapper< DerivedType, ::ScL::Feature::Implicit::Shared >;

    static_assert( ::ScL::Feature::IsThisCompatibleWithPartOfOther< FirstPartType, FirstType >(), "" );
    ::std::cout
        << "IsThisCompatibleWithPartOfOther< FirstPartType, FirstType >()" << ::std::endl
        << true << " = "
        << ::ScL::Feature::IsThisCompatibleWithPartOfOther< FirstPartType, FirstType >() << ::std::endl;

    static_assert( ::ScL::Feature::IsThisCompatibleWithPartOfOther< SecondPartType, FirstType >(), "" );
    ::std::cout
        << "IsThisCompatibleWithPartOfOther< SecondPartType, FirstType >()" << ::std::endl
        << true << " = "
        << ::ScL::Feature::IsThisCompatibleWithPartOfOther< SecondPartType, FirstType >() << ::std::endl;

    static_assert( !::ScL::Feature::IsThisCompatibleWithPartOfOther< FirstPartType, SecondType >(), "" );
    ::std::cout
        << "IsThisCompatibleWithPartOfOther< FirstPartType, SecondType >()" << ::std::endl
        << false << " = "
        << ::ScL::Feature::IsThisCompatibleWithPartOfOther< FirstPartType, SecondType >() << ::std::endl;

    static_assert( ::ScL::Feature::IsThisCompatibleWithPartOfOther< SecondPartType, SecondType >(), "" );
    ::std::cout
        << "IsThisCompatibleWithPartOfOther< SecondPartType, SecondType >()" << ::std::endl
        << true << " = "
        << ::ScL::Feature::IsThisCompatibleWithPartOfOther< SecondPartType, SecondType >() << ::std::endl;

    static_assert( !::ScL::Feature::IsThisCompatibleWithPartOfOther< FirstType, FirstType >(), "" );
    ::std::cout
        << "IsThisCompatibleWithPartOfOther< FirstType, FirstType >()" << ::std::endl
        << false << " = "
        << ::ScL::Feature::IsThisCompatibleWithPartOfOther< FirstType, FirstType >() << ::std::endl;

    static_assert( !::ScL::Feature::IsThisCompatibleWithPartOfOther< FirstType, SecondType >(), "" );
    ::std::cout
        << "IsThisCompatibleWithPartOfOther< FirstType, SecondType >()" << ::std::endl
        << false << " = "
        << ::ScL::Feature::IsThisCompatibleWithPartOfOther< FirstType, SecondType >() << ::std::endl;

    static_assert( !::ScL::Feature::IsThisCompatibleWithPartOfOther< SecondType, FirstType >(), "" );
    ::std::cout
        << "IsThisCompatibleWithPartOfOther< SecondType, FirstType >()" << ::std::endl
        << false << " = "
        << ::ScL::Feature::IsThisCompatibleWithPartOfOther< SecondType, FirstType >() << ::std::endl;

    static_assert( !::ScL::Feature::IsThisCompatibleWithPartOfOther< FirstPartType, SecondPartType >(), "" );
    ::std::cout
        << "IsThisCompatibleWithPartOfOther< FirstPartType, SecondPartType >()" << ::std::endl
        << false << " = "
        << ::ScL::Feature::IsThisCompatibleWithPartOfOther< FirstPartType, SecondPartType >() << ::std::endl;

    FirstType first_value;
    FirstPartType first_part_value;

    FirstType check_value( first_part_value );
    FirstPartType check_part_value( first_value );

    return 0;
}
