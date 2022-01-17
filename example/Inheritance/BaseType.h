#pragma once

#include <iostream>
#include <string>

#include <ScL/Feature/Wrapper.h>
#include <ScL/Feature/Tool.h>

struct BaseType
{
    using Int = int;
    using String = ::std::string;

    ::ScL::Feature::Wrapper< Int > m_int;
    ::ScL::Feature::Wrapper< String > m_string;

    BaseType (
        Int int_value = Int(),
        String string_value = String() )
            : m_int( int_value )
            , m_string( string_value )
    {
    }

    BaseType ( BaseType && ) = default;
    BaseType ( const BaseType & ) = default;

    BaseType & operator = ( BaseType && ) = default;
    BaseType & operator = ( const BaseType & ) = default;
};
