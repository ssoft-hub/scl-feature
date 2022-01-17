#include "BaseType.h"

struct AdditionType
{
    size_t dummy;
    virtual ~AdditionType () {}
};

struct DerivedType
    : public AdditionType
    , public BaseType
{
    using Tool = ::ScL::Feature::Implicit::Shared;
    using Int = ::ScL::Feature::Wrapper< int, Tool >;
    using String = ::ScL::Feature::Wrapper< ::std::string, Tool >;

    ::ScL::Feature::Wrapper< Int > m_int;
    ::ScL::Feature::Wrapper< String > m_string;

    DerivedType (
            Int int_value = Int(),
            String string_value = String() )
        : BaseType( 1, "Base string" )
        , m_int( int_value )
        , m_string( string_value )
    {
    }

    DerivedType ( DerivedType && ) = default;
    DerivedType ( const DerivedType & ) = default;

    DerivedType & operator = ( DerivedType && ) = default;
    DerivedType & operator = ( const DerivedType & ) = default;
};
