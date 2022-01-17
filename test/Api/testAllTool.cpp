//#include <memory.h>
//#include <iostream>
//#include "MyType.h"
//#include <ModelKit.h>

// using namespace ::ScL;

//template < typename _TestType >
//void testTool ()
//{
//    Instance< _TestType > value;
//    Instance< _TestType > other;

//    other = _TestType();
//    value = ::std::move( other ); // other not valid!!!
//    other = Instance< _TestType >();
//    other = value;

//    value->m_first_name = "first name";
//    value->m_last_name = "last name";
//    value->m_age = 50;
//    value->m_stature = 178.5;

//    // Имеет смысл применять гаранты (указатели), если необходимо снизить
//    // накладные расходы связанные с их конкретным инструментом _Tool
//    // (например, системные блокировки и т.п.)

//    // Гарант свойств для применения константного значения value
//    auto cvalue_ptr = &castConst( value );

//    // value
//    ::std::cout
//        << "Test tool:" << ::std::endl
//        << cvalue_ptr->m_first_name << ::std::endl
//        << cvalue_ptr->m_last_name << ::std::endl
//        << cvalue_ptr->m_age << ::std::endl
//        << cvalue_ptr->m_stature << ::std::endl;
//}

//template < typename _Type >
//void testToolByTool ()
//{
//    testTool< _Type >();
//    testTool< Instance< _Type, ::Cpp::Inplace::DefaultTool > >();
//    testTool< Instance< _Type, ::Cpp::Raw::HeapTool > >();
//    testTool< Instance< _Type, ::Implicit::RawTool > >();
//    testTool< Instance< _Type, ::Std::Mutex::AtomicTool > >();
//    testTool< Instance< _Type, ::Std::Shared::ImplicitTool > >();
//    testTool< Instance< _Type, ::Std::Shared::HeapTool > >();
//    testTool< Instance< _Type, ::Std::Unique::HeapTool > >();
//}

//void testAllTool ()
//{
//    testToolByTool< MyType >();
//    testToolByTool< Instance< MyType, ::Cpp::Inplace::DefaultTool > >();
//    testToolByTool< Instance< MyType, ::Cpp::Raw::HeapTool > >();
//    testToolByTool< Instance< MyType, ::Implicit::RawTool > >();
//    testToolByTool< Instance< MyType, ::Std::Mutex::AtomicTool > >();
//    testToolByTool< Instance< MyType, ::Std::Shared::ImplicitTool > >();
//    testToolByTool< Instance< MyType, ::Std::Shared::HeapTool > >();
//    testToolByTool< Instance< MyType, ::Std::Unique::HeapTool > >();
//}
