#pragma once

#include <string>

#include <ScL/Feature/Tool.h>
#include <ScL/Feature/Wrapper.h>

struct BaseType
{
    using Int = int;
    using String = ::std::string;

    ::ScL::Feature::Wrapper<Int> m_int;
    ::ScL::Feature::Wrapper<String> m_string;

    BaseType(Int int_value = {}, String string_value = {})
        : m_int(int_value)
        , m_string(string_value)
    {}

    BaseType(BaseType &&) = default;
    BaseType(BaseType const &) = default;

    BaseType & operator=(BaseType &&) = default;
    BaseType & operator=(BaseType const &) = default;
};
