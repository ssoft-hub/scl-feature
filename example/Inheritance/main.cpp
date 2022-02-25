#include <ScL/Feature/Wrapper.h>
#include <ScL/Utility/Qualifier.h>

#include "BaseType.h"
#include "DerivedType.h"

using Int = int;
using String = ::std::string;

using BaseInt = BaseType::Int;
using BaseString = BaseType::String;

using DerivedInt = DerivedType::Int;
using DerivedString = DerivedType::String;

using Tool = ::ScL::Feature::Heap::Unique;
using HeapInt = ::ScL::Feature::Wrapper< int, Tool >;
using HeapString = ::ScL::Feature::Wrapper< ::std::string, Tool >;

void printBase ( const BaseType & base_value )
{
    ::std::cout
        << "Base: "
        << base_value.m_int << " "
        << base_value.m_string << "\n";
}

void testBase ()
{
    ::ScL::Feature::Wrapper< BaseType > default_base_value;
    ::ScL::Feature::Wrapper< BaseType > contruct_base_value( 10, "Constructed base type" );
    ::ScL::Feature::Wrapper< BaseType > move_base_value = ::ScL::Feature::Wrapper< BaseType >( 20, "Moved base type" );

    default_base_value = contruct_base_value;

    printBase( *&::ScL::asConst(default_base_value) );
    printBase( *&::ScL::asConst(contruct_base_value) );
    printBase( *&::ScL::asConst(move_base_value) );
}

void testBaseSetting ()
{
    ::ScL::Feature::Wrapper< BaseType > base_value;

    base_value->m_int = 1;
    base_value->m_string = "one";
    printBase( *&::ScL::asConst(base_value) );

    base_value->m_int = BaseInt( 2 );
    base_value->m_string = BaseString( "two" );
    printBase( *&::ScL::asConst(base_value) );

    base_value->m_int = DerivedInt( 3 );
    base_value->m_string = DerivedString( "three" );
    printBase( *&::ScL::asConst(base_value) );

    base_value->m_int = HeapInt( 4 );
    base_value->m_string = HeapString( "four" );
    printBase( *&::ScL::asConst(base_value) );
}

void printDerived ( const DerivedType & derived_value )
{
    printBase( derived_value );
    ::std::cout
        << "Derived: "
        << derived_value.m_int << " "
        << derived_value.m_string << "\n";
}

void testDerived ()
{
    ::ScL::Feature::Wrapper< DerivedType > default_derived_value; //
    ::ScL::Feature::Wrapper< DerivedType > contruct_derived_value( 10, "Constructed derived type" ); //
    ::ScL::Feature::Wrapper< DerivedType > move_derived_value = ::ScL::Feature::Wrapper< DerivedType >( 20, "Moved derived type" ); //

    default_derived_value = contruct_derived_value; //

    printDerived( *&::ScL::asConst(default_derived_value) );
    printDerived( *&::ScL::asConst(contruct_derived_value) );
    printDerived( *&::ScL::asConst(move_derived_value) );
}

void testBaseDerivedSetting ()
{
    using BaseTestType = ::ScL::Feature::Wrapper< BaseType, ::ScL::Feature::Implicit::Raw >;
    using DerivedTestType = ::ScL::Feature::Wrapper< DerivedType, ::ScL::Feature::Implicit::Raw >;

    ::ScL::Feature::Wrapper< DerivedTestType > derived_value;

    derived_value->BaseType::m_int = 1;
    derived_value->BaseType::m_string = "one";
    derived_value->m_int = -1;
    derived_value->m_string = "negative one";
    printDerived( *&::ScL::asConst( derived_value ) );

    derived_value->BaseType::m_int = BaseInt( 2 );
    derived_value->BaseType::m_string = BaseString( "two" );
    derived_value->m_int = BaseInt( -2 );
    derived_value->m_string = BaseString( "negative two" );
    printDerived( *&::ScL::asConst( derived_value ) );

    derived_value->BaseType::m_int = DerivedInt( 3 );
    derived_value->BaseType::m_string = DerivedString( "three" );
    derived_value->m_int = ::ScL::asConst( DerivedInt( -3 ) );
    derived_value->m_string = DerivedString( "negative three" );
    printDerived( *&::ScL::asConst( derived_value ) );

    derived_value->BaseType::m_int = HeapInt( 4 );
    derived_value->BaseType::m_string = HeapString( "four" );
    derived_value->m_int = HeapInt( -4 );
    derived_value->m_string = HeapString( "negative four" );
    printDerived( *&::ScL::asConst( derived_value ) );

    ::ScL::Feature::Wrapper< BaseTestType > base_value;
    base_value = derived_value; // OK
    printBase( *&::ScL::asConst( base_value ) );

    //Wrapper< DerivedType > other_derived_value = base_value; //ERROR
}

int main ( int, char ** )
{
    testBase();
    testBaseSetting();
    testDerived();
    testBaseDerivedSetting();
    return 0;
}
