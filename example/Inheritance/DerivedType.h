#include "BaseType.h"

struct AdditionType
{
    size_t dummy;

    virtual ~AdditionType() {}
};

struct DerivedType
    : public AdditionType
    , public BaseType
{
    using Tool = ::ScL::Feature::Implicit::Shared;
    using Int = ::ScL::Feature::Wrapper<int, Tool>;
    using String = ::ScL::Feature::Wrapper< ::std::string, Tool>;

    Int m_int;
    String m_string;

    DerivedType(Int int_value = Int(), String string_value = String())
        : BaseType(1, "Base string")
        , m_int(int_value)
        , m_string(string_value)
    {}

    DerivedType(DerivedType &&) = default;
    DerivedType(DerivedType const &) = default;

    DerivedType & operator=(DerivedType &&) = default;
    DerivedType & operator=(DerivedType const &) = default;
};
