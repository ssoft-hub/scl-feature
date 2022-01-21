#pragma once

#include <ScL/Feature.h>
#include <string>

struct MyType
{
    ScL::Feature::Wrapper< ::std::string > m_first_name;
    ScL::Feature::Wrapper< ::std::string > m_last_name;
    ScL::Feature::Wrapper< int > m_age;
    ScL::Feature::Wrapper< double > m_stature;
};
