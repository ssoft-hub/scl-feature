#include <ScL/Feature/Tool.h>
#include <ScL/Feature/Wrapper.h>
#include <iostream>

using namespace ::ScL::Feature;

struct BaseType
{};

struct DerivedType : public BaseType
{};

// #include <cxxabi.h>

template <typename _Type>
void bar()
{
    ::std::cout << typeid(_Type).name() << ::std::endl;

    // int status = 0;
    // char * realname = nullptr;
    // realname = abi::__cxa_demangle( typeid( _Type ).name(), 0, 0, &status );

    //::std::cout << realname << ::std::endl;

    // free( realname );
}

// Test of multi tool definition
void foo()
{
    // Wrapper< Wrapper< Wrapper< BaseType, Implicit::Shared >, Heap::Unique >,
    // ThreadSafe::RecursiveMutex > >
    using FirstType = Wrapper<Wrapper<Wrapper<BaseType, Implicit::Shared>, Heap::Unique>,
        ThreadSafe::RecursiveMutex>;

    // Wrapper< Wrapper< Wrapper< BaseType, Implicit::Shared >, Heap::Unique >,
    // ThreadSafe::RecursiveMutex > >
    using SecondType = Wrapper<BaseType,
        Implicit::Shared,
        Heap::Unique,
        ThreadSafe::RecursiveMutex>;

    bar<FirstType>();
    bar<SecondType>();
    static_assert(::std::is_same<FirstType, SecondType>{}, "");
}

// Test of tool duplicate removing
void foofoo()
{
    // Wrapper< Wrapper< Wrapper< BaseType, Implicit::Shared >, Heap::Unique >,
    // ThreadSafe::RecursiveMutex > >
    using FirstType = Wrapper<
        Wrapper<
            Wrapper<Wrapper<Wrapper<Wrapper<Wrapper<BaseType, Implicit::Shared>, Implicit::Shared>,
                                Heap::Unique>,
                        Heap::Unique>,
                Heap::Unique>,
            Heap::Unique>,
        ThreadSafe::RecursiveMutex>;

    // Wrapper< Wrapper< Wrapper< BaseType, Implicit::Shared >, Heap::Unique >,
    // ThreadSafe::RecursiveMutex > >
    using SecondType = Wrapper<BaseType,
        Implicit::Shared,
        Implicit::Shared,
        Implicit::Shared,
        Heap::Unique,
        Heap::Unique,
        ThreadSafe::RecursiveMutex,
        ThreadSafe::RecursiveMutex>;

    // Wrapper< Wrapper< Wrapper< BaseType, Implicit::Shared >, Heap::Unique >,
    // ThreadSafe::RecursiveMutex > >
    using ThirdType = Wrapper<
        Wrapper<Wrapper<Wrapper<BaseType, Implicit::Shared>, Implicit::Shared, Heap::Unique>,
            Heap::Unique,
            Heap::Unique,
            Heap::Unique>,
        ThreadSafe::RecursiveMutex>;

    bar<FirstType>();
    bar<SecondType>();
    bar<ThirdType>();

    static_assert(::std::is_same<FirstType, SecondType>{}, "");
    static_assert(::std::is_same<FirstType, ThirdType>{}, "");
}

int main(int /*argc*/, char ** /*argv*/)
{
    foo();
    foofoo();

    static_assert(isThisCompatibleWithOther<Wrapper<int>, Wrapper<int> >(), "");
    ::std::cout << "isThisCompatibleWithOther< Wrapper< int >, "
                   "Wrapper< int > >()"
                << ::std::endl
                << true << " = " << isThisCompatibleWithOther<Wrapper<int>, Wrapper<int> >()
                << ::std::endl;

    static_assert(!isThisCompatibleWithOther<Wrapper<int>, Wrapper<double> >(), "");
    ::std::cout << "isThisCompatibleWithOther< Wrapper< int >, "
                   "Wrapper< double > >()"
                << ::std::endl
                << false << " = " << isThisCompatibleWithOther<Wrapper<int>, Wrapper<double> >()
                << ::std::endl;

    static_assert(!isThisCompatibleWithOther<Wrapper<BaseType>, Wrapper<DerivedType> >(), "");
    ::std::cout << "isThisCompatibleWithOther< Wrapper< BaseType >, "
                   "Wrapper< DerivedType > >()"
                << ::std::endl
                << false << " = "
                << isThisCompatibleWithOther<Wrapper<BaseType>, Wrapper<DerivedType> >()
                << ::std::endl;

    static_assert(isThisCompatibleWithOther<Wrapper<DerivedType>, Wrapper<BaseType> >(), "");
    ::std::cout << "isThisCompatibleWithOther< Wrapper< DerivedType >, "
                   "Wrapper< BaseType > >()"
                << ::std::endl
                << true << " = "
                << isThisCompatibleWithOther<Wrapper<DerivedType>, Wrapper<BaseType> >()
                << ::std::endl;

    using FirstType = Wrapper<BaseType, Implicit::Shared, Heap::Unique, ThreadSafe::RecursiveMutex>;

    using SecondType = Wrapper<DerivedType,
        Implicit::Shared,
        Heap::Unique,
        ThreadSafe::RecursiveMutex>;

    static_assert(!isThisCompatibleWithOther<FirstType, SecondType>(), "");
    ::std::cout << "isThisCompatibleWithOther< FirstType, SecondType >()" << ::std::endl
                << false << " = " << isThisCompatibleWithOther<FirstType, SecondType>()
                << ::std::endl;

    static_assert(isThisCompatibleWithOther<SecondType, FirstType>(), "");
    ::std::cout << "isThisCompatibleWithOther< SecondType, FirstType >()" << ::std::endl
                << true << " = " << isThisCompatibleWithOther<SecondType, FirstType>()
                << ::std::endl;

    using FirstPartType = Wrapper<BaseType, Implicit::Shared>;
    using SecondPartType = Wrapper<DerivedType, Implicit::Shared>;

    static_assert(IsThisCompatibleWithPartOfOther<FirstPartType, FirstType>(), "");
    ::std::cout << "IsThisCompatibleWithPartOfOther< FirstPartType, FirstType >()" << ::std::endl
                << true << " = " << IsThisCompatibleWithPartOfOther<FirstPartType, FirstType>()
                << ::std::endl;

    static_assert(IsThisCompatibleWithPartOfOther<SecondPartType, FirstType>(), "");
    ::std::cout << "IsThisCompatibleWithPartOfOther< SecondPartType, FirstType >()" << ::std::endl
                << true << " = " << IsThisCompatibleWithPartOfOther<SecondPartType, FirstType>()
                << ::std::endl;

    static_assert(!IsThisCompatibleWithPartOfOther<FirstPartType, SecondType>(), "");
    ::std::cout << "IsThisCompatibleWithPartOfOther< FirstPartType, SecondType >()" << ::std::endl
                << false << " = " << IsThisCompatibleWithPartOfOther<FirstPartType, SecondType>()
                << ::std::endl;

    static_assert(IsThisCompatibleWithPartOfOther<SecondPartType, SecondType>(), "");
    ::std::cout << "IsThisCompatibleWithPartOfOther< SecondPartType, SecondType >()" << ::std::endl
                << true << " = " << IsThisCompatibleWithPartOfOther<SecondPartType, SecondType>()
                << ::std::endl;

    static_assert(!IsThisCompatibleWithPartOfOther<FirstType, FirstType>(), "");
    ::std::cout << "IsThisCompatibleWithPartOfOther< FirstType, FirstType >()" << ::std::endl
                << false << " = " << IsThisCompatibleWithPartOfOther<FirstType, FirstType>()
                << ::std::endl;

    static_assert(!IsThisCompatibleWithPartOfOther<FirstType, SecondType>(), "");
    ::std::cout << "IsThisCompatibleWithPartOfOther< FirstType, SecondType >()" << ::std::endl
                << false << " = " << IsThisCompatibleWithPartOfOther<FirstType, SecondType>()
                << ::std::endl;

    static_assert(!IsThisCompatibleWithPartOfOther<SecondType, FirstType>(), "");
    ::std::cout << "IsThisCompatibleWithPartOfOther< SecondType, FirstType >()" << ::std::endl
                << false << " = " << IsThisCompatibleWithPartOfOther<SecondType, FirstType>()
                << ::std::endl;

    static_assert(!IsThisCompatibleWithPartOfOther<FirstPartType, SecondPartType>(), "");
    ::std::cout << "IsThisCompatibleWithPartOfOther< FirstPartType, SecondPartType >()"
                << ::std::endl
                << false << " = "
                << IsThisCompatibleWithPartOfOther<FirstPartType, SecondPartType>() << ::std::endl;

    FirstType first_value;
    FirstPartType first_part_value;

    FirstType check_value(first_part_value);
    FirstPartType check_part_value(first_value);

    return 0;
}
