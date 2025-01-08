#pragma once
#ifndef SCL_FEATURE_DETAIL_PROPERTY_REFLECTION_H
#define SCL_FEATURE_DETAIL_PROPERTY_REFLECTION_H

#include <ScL/Feature/Wrapper.h>

namespace ScL::Feature
{
    using PropertyTool = Inplace::Default;
}

#define SCL_REFLECT_PROPERTY( Type, property ) \
    private: \
        Wrapper<Type> dlhf_ ## property ## _ljhp; \
    public: \
        Wrapper<Type> & property{ dlhf_ ## property ## _ljhp }; \

#define SCL_PROPERTY_DECLTYPE( property ) \
    decltype(dlhf_ ## property ## _ljhp) \

#endif
