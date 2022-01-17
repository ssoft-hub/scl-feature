#pragma once

#include <ModelKit.h>
#include <string>

struct MyType
{
    ScL::Instance< ::std::string > m_first_name;
    ScL::Instance< ::std::string > m_last_name;
    ScL::Instance< int > m_age;
    ScL::Instance< double > m_stature;
};
