#pragma once

#include <string>

#include <scl/feature/tool.h>
#include <scl/feature/wrapper.h>

struct BaseType
{
    using Int = int;
    using String = ::std::string;

    ::scl::feature::wrapper<Int> m_int;
    ::scl::feature::wrapper<String> m_string;

    BaseType(Int int_value = {}, String string_value = {})
        : m_int(int_value)
        , m_string(string_value)
    {}

    BaseType(BaseType &&) = default;
    BaseType(BaseType const &) = default;

    BaseType & operator=(BaseType &&) = default;
    BaseType & operator=(BaseType const &) = default;
};
