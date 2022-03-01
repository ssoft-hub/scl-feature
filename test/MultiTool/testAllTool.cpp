#include <memory.h>
#include <iostream>
#include <ScL/Utility/Qualifier.h>
#include "MyType.h"

using namespace ::ScL::Feature;

template < typename _TestType >
void testTool ()
{
    Wrapper< _TestType > value;
    Wrapper< _TestType > other;

    other = _TestType();
    value = ::std::move( other ); // other not valid!!!
    other = Wrapper< _TestType >();
    other = value;

    value->m_first_name = "first name";
    value->m_last_name = "last name";
    value->m_age = 50;
    value->m_stature = 178.5;

    // Имеет смысл применять гаранты (указатели), если необходимо снизить
    // накладные расходы связанные с их конкретным инструментом _Tool
    // (например, системные блокировки и т.п.)

    // Гарант свойств для применения константного значения value
    auto cvalue_ptr = &::ScL::asConst( value );

    // value
    ::std::cout
        << "Test tool:" << ::std::endl
        << cvalue_ptr->m_first_name << ::std::endl
        << cvalue_ptr->m_last_name << ::std::endl
        << cvalue_ptr->m_age << ::std::endl
        << cvalue_ptr->m_stature << ::std::endl;
}

template < typename _Type >
void testToolByTool ()
{
    testTool< _Type >();

    testTool< Wrapper< _Type, Heap::Raw > >();
    testTool< Wrapper< _Type, Heap::Shared > >();
    testTool< Wrapper< _Type, Heap::Unique > >();

    testTool< Wrapper< _Type, Implicit::Raw > >();
    testTool< Wrapper< _Type, Implicit::Shared > >();

    testTool< Wrapper< _Type, Inplace::Debug > >();
    testTool< Wrapper< _Type, Inplace::Default > >();
    testTool< Wrapper< _Type, Inplace::Uninitialized > >();
    testTool< Wrapper< _Type, Inplace::Optional > >();

    testTool< Wrapper< _Type, ThreadSafe::Atomic > >();
    testTool< Wrapper< _Type, ThreadSafe::Mutex > >();
    testTool< Wrapper< _Type, ThreadSafe::RecursiveMutex > >();
    testTool< Wrapper< _Type, ThreadSafe::SharedMutex > >();
}

void testAllTool ()
{
    testToolByTool< MyType >();

    // Too long compilation
    //testToolByTool< Wrapper< MyType, Heap::Raw > >();
    //testToolByTool< Wrapper< MyType, Heap::Shared > >();
    //testToolByTool< Wrapper< MyType, Heap::Unique > >();

    //testToolByTool< Wrapper< MyType, Implicit::Raw > >();
    //testToolByTool< Wrapper< MyType, Implicit::Shared > >();

    //testToolByTool< Wrapper< MyType, Inplace::Debug > >();
    //testToolByTool< Wrapper< MyType, Inplace::Default > >();
    //testToolByTool< Wrapper< MyType, Inplace::Uninitialized > >();

    //testToolByTool< Wrapper< MyType, ThreadSafe::Atomic > >();
    //testToolByTool< Wrapper< MyType, ThreadSafe::Mutex > >();
    //testToolByTool< Wrapper< MyType, ThreadSafe::RecursiveMutex > >();
    //testToolByTool< Wrapper< MyType, ThreadSafe::SharedMutex > >();
}
