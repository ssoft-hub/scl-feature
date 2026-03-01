#include <scl/feature/wrapper.h>

#include "BaseType.h"
#include "DerivedType.h"

using namespace ::scl::feature;

using Int = int;
using String = ::std::string;

using BaseInt = BaseType::Int;
using BaseString = BaseType::String;

using DerivedInt = DerivedType::Int;
using DerivedString = DerivedType::String;

using Tool = Heap::Unique;
using HeapInt = wrapper<int, Tool>;
using HeapString = wrapper< ::std::string, Tool>;

void printBase(BaseType const & base_value)
{
    ::std::cout << "Base: " << base_value.m_int << " " << base_value.m_string
                << "\n"; //::std::endl;
}

void testBase()
{
    wrapper<BaseType> default_base_value;
    wrapper<BaseType> contruct_base_value(10, "Constructed base type");
    wrapper<BaseType> move_base_value = wrapper<BaseType>(20, "Moved base type");

    default_base_value = contruct_base_value;

    printBase(*&::std::as_const(default_base_value));
    printBase(*&::std::as_const(contruct_base_value));
    printBase(*&::std::as_const(move_base_value));
}

void testBaseSetting()
{
    wrapper<BaseType> base_value;

    base_value->m_int = 1;
    base_value->m_string = "one";
    printBase(*&::std::as_const(base_value));

    base_value->m_int = BaseInt(2);
    base_value->m_string = BaseString("two");
    printBase(*&::std::as_const(base_value));

    base_value->m_int = DerivedInt(3);
    base_value->m_string = DerivedString("three");
    printBase(*&::std::as_const(base_value));

    base_value->m_int = HeapInt(4);
    base_value->m_string = HeapString("four");
    printBase(*&::std::as_const(base_value));
}

void printDerived(DerivedType const & derived_value)
{
    printBase(derived_value);
    ::std::cout << "Derived: " << derived_value.m_int << " " << derived_value.m_string
                << "\n"; // ::std::endl;
}

void testDerived()
{
    wrapper<DerivedType> default_derived_value;                                               //
    wrapper<DerivedType> contruct_derived_value(10, "Constructed derived type");              //
    wrapper<DerivedType> move_derived_value = wrapper<DerivedType>(20, "Moved derived type"); //

    default_derived_value = contruct_derived_value; //

    printDerived(*&::std::as_const(default_derived_value));
    printDerived(*&::std::as_const(contruct_derived_value));
    printDerived(*&::std::as_const(move_derived_value));
}

void testBaseDerivedSetting()
{
    using BaseTestType = wrapper<BaseType, Implicit::Raw>;
    using DerivedTestType = wrapper<DerivedType, Implicit::Raw>;

    wrapper<DerivedTestType> derived_value;

    derived_value->BaseType::m_int = 1;
    derived_value->BaseType::m_string = "one";
    derived_value->m_int = -1;
    derived_value->m_string = "negative one";
    printDerived(*&::std::as_const(derived_value));

    derived_value->BaseType::m_int = BaseInt(2);
    derived_value->BaseType::m_string = BaseString("two");
    derived_value->m_int = BaseInt(-2);
    derived_value->m_string = BaseString("negative two");
    printDerived(*&::std::as_const(derived_value));

    derived_value->BaseType::m_int = DerivedInt(3);
    derived_value->BaseType::m_string = DerivedString("three");
    derived_value->m_int = DerivedInt(-3);
    derived_value->m_string = DerivedString("negative three");
    printDerived(*&::std::as_const(derived_value));

    derived_value->BaseType::m_int = HeapInt(4);
    derived_value->BaseType::m_string = HeapString("four");
    derived_value->m_int = HeapInt(-4);
    derived_value->m_string = HeapString("negative four");
    printDerived(*&::std::as_const(derived_value));

    wrapper<BaseTestType> base_value;
    base_value = derived_value; // OK
    printBase(*&::std::as_const(base_value));

    // wrapper< DerivedType > other_derived_value = base_value; //ERROR
}

int main(int, char **)
{
    testBase();
    testBaseSetting();
    testDerived();
    testBaseDerivedSetting();
    return 0;
}
