#include "ScL/Feature.h"
#include "ScL/Utility.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

#define MAYBE_UNUSED (void)

using namespace ::ScL;
using namespace ::ScL::Feature;

#if defined( __GNUC__ )
#   define DATA_FUNC_INFO __PRETTY_FUNCTION__
#elif defined( _MSC_VER )
#   define DATA_FUNC_INFO __FUNCSIG__
#endif

#define DATA_UNARY_OPERATOR_INT_PROTOTYPE( symbol, refer ) \
    void operator symbol ( int ) refer \
    { ::std::cout << DATA_FUNC_INFO << ::std::endl; } \

#define DATA_UNARY_OPERATOR_INT( symbol ) \
    DATA_UNARY_OPERATOR_INT_PROTOTYPE( SCL_SINGLE_ARG( symbol ), && ) \
    DATA_UNARY_OPERATOR_INT_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const && ) \
    DATA_UNARY_OPERATOR_INT_PROTOTYPE( SCL_SINGLE_ARG( symbol ), volatile && ) \
    DATA_UNARY_OPERATOR_INT_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const volatile && ) \
    DATA_UNARY_OPERATOR_INT_PROTOTYPE( SCL_SINGLE_ARG( symbol ), & ) \
    DATA_UNARY_OPERATOR_INT_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const & ) \
    DATA_UNARY_OPERATOR_INT_PROTOTYPE( SCL_SINGLE_ARG( symbol ), volatile & ) \
    DATA_UNARY_OPERATOR_INT_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const volatile & ) \

#define DATA_UNARY_OPERATOR_PROTOTYPE( symbol, refer ) \
    void operator symbol () refer \
    { ::std::cout << DATA_FUNC_INFO << ::std::endl; } \

#define DATA_UNARY_OPERATOR( symbol ) \
    DATA_UNARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), && ) \
    DATA_UNARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const && ) \
    DATA_UNARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), volatile && ) \
    DATA_UNARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const volatile && ) \
    DATA_UNARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), & ) \
    DATA_UNARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const & ) \
    DATA_UNARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), volatile & ) \
    DATA_UNARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const volatile & ) \

#define DATA_BINARY_OPERATOR_PROTOTYPE( symbol, refer ) \
    template < typename _Right > \
    void operator symbol ( _Right && ) refer \
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; } \

#define DATA_BINARY_OPERATOR( symbol ) \
    DATA_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), volatile && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const volatile && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), volatile & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const volatile & ) \

#define DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( symbol, this_refer, other_refer ) \
    void operator symbol ( ThisType other_refer ) this_refer \
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; } \

#define DATA_BINARY_OPERATOR_FOR_THIS( symbol ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &&, && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &&, const && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &&, volatile && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &&, const volatile && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &&, & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &&, const & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &&, volatile & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &&, const volatile & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &&, && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &&, const && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &&, volatile && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &&, const volatile && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &&, & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &&, const & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &&, volatile & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &&, const volatile & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &&, && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &&, const && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &&, volatile && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &&, const volatile && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &&, & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &&, const & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &&, volatile & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &&, const volatile & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &&, && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &&, const && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &&, volatile && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &&, const volatile && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &&, & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &&, const & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &&, volatile & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &&, const volatile & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &, && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &, const && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &, volatile && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &, const volatile && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &, & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &, const & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &, volatile & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &, const volatile & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &, && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &, const && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &, volatile && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &, const volatile && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &, & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &, const & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &, volatile & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &, const volatile & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &, && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &, const && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &, volatile && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &, const volatile && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &, & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &, const & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &, volatile & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &, const volatile & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &, && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &, const && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &, volatile && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &, const volatile && ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &, & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &, const & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &, volatile & ) \
    DATA_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &, const volatile & ) \

#define DATA_GLOBAL_BINARY_OPERATOR_PROTOTYPE( symbol, right_refer ) \
    template < typename _Left, typename _Right > \
    void operator symbol ( _Left && /*left*/, Data< _Right> right_refer ) \
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; } \

#define DATA_GLOBAL_BINARY_OPERATOR( symbol ) \
    DATA_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), && ) \
    DATA_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const && ) \
    DATA_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), volatile && ) \
    DATA_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const volatile && ) \
    DATA_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), & ) \
    DATA_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const & ) \
    DATA_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), volatile & ) \
    DATA_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const volatile & ) \

class Dummy {};

template < typename _Type >
class Data
{
    static_assert( !::std::is_reference< _Type >::value, "_Type must to be a not reference." );
    using ThisType = Data< _Type >;

public:
    _Type m_value;

public:
    template < typename ... _Arguments >
    Data ( _Arguments && ... arguments ) : m_value( ::std::forward< _Arguments >( arguments ) ... )
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }

    template < typename _Other >
    Data ( Data< _Other > && other ) : m_value( ::std::forward< _Other >( other.m_value ) )
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename _Other >
    Data ( const Data< _Other > && other ) : m_value( ::std::forward< const _Other >( other.m_value ) )
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename _Other >
    Data ( volatile Data< _Other > && other ) : m_value( ::std::forward< _Other >( other.m_value ) )
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename _Other >
    Data ( const volatile Data< _Other > && other ) : m_value( ::std::forward< const _Other >( other.m_value ) )
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename _Other >
    Data ( Data< _Other > & other ) : m_value( other.m_value )
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename _Other >
    Data ( const Data< _Other > & other ) : m_value( other.m_value )
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename _Other >
    Data ( volatile Data< _Other > & other ) : m_value( other.m_value )
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename _Other >
    Data ( const volatile Data< _Other > & other ) : m_value( other.m_value )
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }

    Data ( ThisType && other ) : m_value( ::std::forward< _Type >( other.m_value ) )
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    Data ( const ThisType && other ) : m_value( ::std::forward< const _Type >( other.m_value ) )
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    Data ( volatile ThisType && other ) : m_value( ::std::forward< volatile _Type >( other.m_value ) )
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    Data ( const volatile ThisType && other ) : m_value( ::std::forward< const volatile _Type >( other.m_value ) )
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    Data ( ThisType & other ) : m_value( other.m_value )
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    Data ( const ThisType & other ) : m_value( other.m_value )
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    Data ( volatile ThisType & other ) : m_value( other.m_value )
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    Data ( const volatile ThisType & other ) : m_value( other.m_value )
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }

    ~Data ()
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }

    void valueMethod ()
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    void valueConstMethod () const
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    void valueVolatileMethod () volatile
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    void valueConstVolatileMethod () const volatile
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }

    void rvalueMethod () &&
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    void rvalueConstMethod () const &&
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    void rvalueVolatileMethod () volatile &&
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    void rvalueConstVolatileMethod () const volatile &&
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }

    void lvalueMethod () &
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    void lvalueConstMethod () const &
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    void lvalueVolatileMethod () volatile &
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    void lvalueConstVolatileMethod () const volatile &
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }

    template < typename _Index > void operator [] ( _Index && ) &&
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename _Index > void operator [] ( _Index && ) const &&
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename _Index > void operator [] ( _Index && ) volatile &&
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename _Index > void operator [] ( _Index && ) const volatile &&
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename _Index > void operator [] ( _Index && ) &
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename _Index > void operator [] ( _Index && ) const &
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename _Index > void operator [] ( _Index && ) volatile &
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename _Index > void operator [] ( _Index && ) const volatile &
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }

    template < typename ... _Arguments > void operator () ( _Arguments && ... ) &&
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename ... _Arguments > void operator () ( _Arguments && ... ) const &&
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename ... _Arguments > void operator () ( _Arguments && ... ) volatile &&
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename ... _Arguments > void operator () ( _Arguments && ... ) const volatile &&
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename ... _Arguments > void operator () ( _Arguments && ... ) &
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename ... _Arguments > void operator () ( _Arguments && ... ) const &
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename ... _Arguments > void operator () ( _Arguments && ... ) volatile &
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }
    template < typename ... _Arguments > void operator () ( _Arguments && ... ) const volatile &
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; }

    DATA_BINARY_OPERATOR( = )
    DATA_BINARY_OPERATOR_FOR_THIS( = )

    DATA_UNARY_OPERATOR( * )
    DATA_UNARY_OPERATOR( & )
    DATA_UNARY_OPERATOR( -> )
    DATA_BINARY_OPERATOR( ->* )
    DATA_BINARY_OPERATOR( SCL_SINGLE_ARG( , ) )

    DATA_UNARY_OPERATOR( + )
    DATA_UNARY_OPERATOR( - )
    DATA_UNARY_OPERATOR( ++ )
    DATA_UNARY_OPERATOR( -- )
    DATA_UNARY_OPERATOR_INT( ++ )
    DATA_UNARY_OPERATOR_INT( -- )
    DATA_UNARY_OPERATOR( ~ )
    DATA_UNARY_OPERATOR( ! )

    DATA_BINARY_OPERATOR( * )
    DATA_BINARY_OPERATOR( / )
    DATA_BINARY_OPERATOR( % )
    DATA_BINARY_OPERATOR( + )
    DATA_BINARY_OPERATOR( - )
    /* Compound assignment */
    DATA_BINARY_OPERATOR( *= )
    DATA_BINARY_OPERATOR( /= )
    DATA_BINARY_OPERATOR( %= )
    DATA_BINARY_OPERATOR( +=)
    DATA_BINARY_OPERATOR( -= )
    DATA_BINARY_OPERATOR( <<= )
    DATA_BINARY_OPERATOR( >>= )
    DATA_BINARY_OPERATOR( &= )
    DATA_BINARY_OPERATOR( ^= )
    DATA_BINARY_OPERATOR( |= )
    /* Increment and decrement */
    /* Relational and comparison operators */
    DATA_BINARY_OPERATOR( == )
    DATA_BINARY_OPERATOR( != )
    DATA_BINARY_OPERATOR( < )
    DATA_BINARY_OPERATOR( <= )
    DATA_BINARY_OPERATOR( > )
    DATA_BINARY_OPERATOR( >= )
    /* Logical operators */
    DATA_BINARY_OPERATOR( && )
    DATA_BINARY_OPERATOR( || )
    /* Bitwise operators */
    DATA_BINARY_OPERATOR( & )
    DATA_BINARY_OPERATOR( ^ )
    DATA_BINARY_OPERATOR( | )
    DATA_BINARY_OPERATOR( << )
    DATA_BINARY_OPERATOR( >> )

    template < typename ... _Arguments >
    static ThisType make ( _Arguments && ... arguments ) { return Data( ::std::forward< _Arguments >( arguments ) ... ); }
};

/* RIGHT-SIDE INSTANCE OPERATORS */

/* Arithmetic operators */
DATA_GLOBAL_BINARY_OPERATOR( * )
DATA_GLOBAL_BINARY_OPERATOR( / )
DATA_GLOBAL_BINARY_OPERATOR( % )
DATA_GLOBAL_BINARY_OPERATOR( + )
DATA_GLOBAL_BINARY_OPERATOR( - )
/* Compound assignment */
DATA_GLOBAL_BINARY_OPERATOR( *= )
DATA_GLOBAL_BINARY_OPERATOR( /= )
DATA_GLOBAL_BINARY_OPERATOR( %= )
DATA_GLOBAL_BINARY_OPERATOR( += )
DATA_GLOBAL_BINARY_OPERATOR( -= )
DATA_GLOBAL_BINARY_OPERATOR( <<= )
DATA_GLOBAL_BINARY_OPERATOR( >>= )
DATA_GLOBAL_BINARY_OPERATOR( &= )
DATA_GLOBAL_BINARY_OPERATOR( ^= )
DATA_GLOBAL_BINARY_OPERATOR( |= )
/* Relational and comparison operators */
DATA_GLOBAL_BINARY_OPERATOR( == )
DATA_GLOBAL_BINARY_OPERATOR( != )
DATA_GLOBAL_BINARY_OPERATOR( < )
DATA_GLOBAL_BINARY_OPERATOR( <= )
DATA_GLOBAL_BINARY_OPERATOR( > )
DATA_GLOBAL_BINARY_OPERATOR( >= )
/* Logical operators */
DATA_GLOBAL_BINARY_OPERATOR( && )
DATA_GLOBAL_BINARY_OPERATOR( || )
/* Bitwise operators */
DATA_GLOBAL_BINARY_OPERATOR( & )
DATA_GLOBAL_BINARY_OPERATOR( ^ )
DATA_GLOBAL_BINARY_OPERATOR( | )
DATA_GLOBAL_BINARY_OPERATOR( << )
DATA_GLOBAL_BINARY_OPERATOR( >> )

void testConstructors ()
{
    using TestData = Wrapper< Data< double > >;

    TestData one;
    TestData two( 1 );
    TestData three( 0.1 );
    TestData four( one );
    TestData five( asConst( one ) );
    TestData six( TestData::Value::make() );
    TestData seven( asConst( TestData::Value::make() ) );

    TestData() = 10;
    one = 10;

    MAYBE_UNUSED one;
    MAYBE_UNUSED two;
    MAYBE_UNUSED three;
    MAYBE_UNUSED four;
    MAYBE_UNUSED five;
    MAYBE_UNUSED six;
    MAYBE_UNUSED seven;
}

void testSameToolConstructors ()
{
    using TestData = Wrapper< Data< double > >;
    using SameData = Wrapper< Data< int > >;

    SameData one;
    TestData four( one );
    TestData five( asConst( one ) );
    TestData six( SameData::Value::make() );
    TestData seven( asConst( SameData::Value::make() ) );

    MAYBE_UNUSED one;
    MAYBE_UNUSED four;
    MAYBE_UNUSED five;
    MAYBE_UNUSED six;
    MAYBE_UNUSED seven;
}

void testDiffToolConstructors ()
{
    using TestData = Wrapper< Data< double > >;
    using SameData = Wrapper< Data< int >, Implicit::Shared >;

    SameData one;
    TestData four( one );
    TestData five( asConst( one ) );
    TestData six( SameData::Value::make() );
    TestData seven( asConst( SameData::Value::make() ) );

    MAYBE_UNUSED one;
    MAYBE_UNUSED four;
    MAYBE_UNUSED five;
    MAYBE_UNUSED six;
    MAYBE_UNUSED seven;
}

void testAccess ()
{
    //using Key = int;//Wrapper< int >;
    using Key = Wrapper< ::std::string, Implicit::Raw >;
    using Value = Wrapper< Data< int > >;
    using Map = Wrapper< ::std::map< Key, Value > >;

    Map map;
    Key key;
    map[ *&key ] = 10;
}

template < typename _Data >
void testConstructors ()
{
    using TestData = _Data;
    using CTestData = const TestData;
    using VTestData = volatile TestData;
    using CVTestData = const volatile TestData;
    using ::std::move;

    TestData lvalue;
    CTestData lvalue_c;
    VTestData lvalue_v;
    CVTestData lvalue_cv;

    { TestData data; }
    { TestData data = lvalue; }
    { TestData data = lvalue_c; }
    { TestData data = lvalue_v; }
    { TestData data = lvalue_cv; }
    { TestData data = ::std::move( lvalue ); }
    { TestData data = ::std::move( lvalue_c ); }
    { TestData data = ::std::move( lvalue_v ); }
    { TestData data = ::std::move( lvalue_cv ); }
}

template < typename _Data, typename _Other >
void testAssignmentOperator ()
{
    using Data = _Data;
    using Other = _Other;

    Data data;
    Other other;

    data = other;
    data = asConst( other );
    data = asVolatile( other );
    data = asConstVolatile( other );
    data = ::std::move( other );
    data = asConst( ::std::move( other ) );
    data = asVolatile( ::std::move( other ) );
    data = asConstVolatile( ::std::move( other ) );

    ::std::move( data ) = other;
    ::std::move( data ) = asConst( other );
    ::std::move( data ) = asVolatile( other );
    ::std::move( data ) = asConstVolatile( other );
    ::std::move( data ) = ::std::move( other );
    ::std::move( data ) = asConst( ::std::move( other ) );
    ::std::move( data ) = asVolatile( ::std::move( other ) );
    ::std::move( data ) = asConstVolatile( ::std::move( other ) );
}

template < typename _Data >
void testWrapperUnaryOperators ()
{
    using TestData = _Data;

    TestData data;
    TestData & lvalue = data;

    // lvalue
    lvalue[0];
    asConst( lvalue )[0];
    asVolatile( lvalue )[0];
    asConstVolatile( lvalue )[0];

    lvalue(0, 1);
    asConst( lvalue )(0, 1);
    asVolatile( lvalue )(0, 1);
    asConstVolatile( lvalue )(0, 1);

    +lvalue;
    +asConst( lvalue );
    +asVolatile( lvalue );
    +asConstVolatile( lvalue );

    -lvalue;
    -asConst( lvalue );
    -asVolatile( lvalue );
    -asConstVolatile( lvalue );

    ++lvalue;
    ++asConst( lvalue );
    ++asVolatile( lvalue );
    ++asConstVolatile( lvalue );

    --lvalue;
    --asConst( lvalue );
    --asVolatile( lvalue );
    --asConstVolatile( lvalue );

    lvalue++;
    asConst( lvalue )++;
    asVolatile( lvalue )++;
    asConstVolatile( lvalue )++;

    lvalue--;
    asConst( lvalue )--;
    asVolatile( lvalue )--;
    asConstVolatile( lvalue )--;

    !lvalue;
    !asConst( lvalue );
    !asVolatile( lvalue );
    !asConstVolatile( lvalue );

    ~lvalue;
    ~asConst( lvalue );
    ~asVolatile( lvalue );
    ~asConstVolatile( lvalue );

    // rvalue
    ::std::move( lvalue )[0];
    asConst( ::std::move( lvalue ) )[0];
    asVolatile( ::std::move( lvalue ) )[0];
    asConstVolatile( ::std::move( lvalue ) )[0];

    ::std::move( lvalue )(0, 1);
    asConst( ::std::move( lvalue ) )(0, 1);
    asVolatile( ::std::move( lvalue ) )(0, 1);
    asConstVolatile( ::std::move( lvalue ) )(0, 1);

    +::std::move( lvalue );
    +asConst( ::std::move( lvalue ) );
    +asVolatile( ::std::move( lvalue ) );
    +asConstVolatile( ::std::move( lvalue ) );

    -::std::move( lvalue );
    -asConst( ::std::move( lvalue ) );
    -asVolatile( ::std::move( lvalue ) );
    -asConstVolatile( ::std::move( lvalue ) );

    ++::std::move( lvalue );
    ++asConst( ::std::move( lvalue ) );
    ++asVolatile( ::std::move( lvalue ) );
    ++asConstVolatile( ::std::move( lvalue ) );

    --::std::move( lvalue );
    --asConst( ::std::move( lvalue ) );
    --asVolatile( ::std::move( lvalue ) );
    --asConstVolatile( ::std::move( lvalue ) );

    ::std::move( lvalue )++;
    asConst( ::std::move( lvalue ) )++;
    asVolatile( ::std::move( lvalue ) )++;
    asConstVolatile( ::std::move( lvalue ) )++;

    ::std::move( lvalue )--;
    asConst( ::std::move( lvalue ) )--;
    asVolatile( ::std::move( lvalue ) )--;
    asConstVolatile( ::std::move( lvalue ) )--;

    !::std::move( lvalue );
    !asConst( ::std::move( lvalue ) );
    !asVolatile( ::std::move( lvalue ) );
    !asConstVolatile( ::std::move( lvalue ) );

    ~::std::move( lvalue );
    ~asConst( ::std::move( lvalue ) );
    ~asVolatile( ::std::move( lvalue ) );
    ~asConstVolatile( ::std::move( lvalue ) );
}

template < typename _Left, typename _Right >
void testWrapperBinaryOperatorsSpec ()
{
    using ::std::move;

    _Left left;
    _Right right;

    // lvalue / lvalue
    left + right;
    left - right;
    left * right;
    left / right;
    left % right;

    left == right;
    left != right;
    left > right;
    left < right;
    left >= right;
    left <= right;
    left && right;
    left || right;
    left & right;
    left | right;
    left ^ right;
    left >> right;
    left << right;

    left += right;
    left -= right;
    left *= right;
    left /= right;
    left %= right;
    left &= right;
    left |= right;
    left ^= right;
    left >>= right;
    left <<= right;

    // lvalue / rvalue
    left + ::std::move( right );
    left - ::std::move( right );
    left * ::std::move( right );
    left / ::std::move( right );
    left % ::std::move( right );

    left == ::std::move( right );
    left != ::std::move( right );
    left > ::std::move( right );
    left < ::std::move( right );
    left >= ::std::move( right );
    left <= ::std::move( right );
    left && ::std::move( right );
    left || ::std::move( right );
    left & ::std::move( right );
    left | ::std::move( right );
    left ^ ::std::move( right );
    left >> ::std::move( right );
    left << ::std::move( right );

    left += ::std::move( right );
    left -= ::std::move( right );
    left *= ::std::move( right );
    left /= ::std::move( right );
    left %= ::std::move( right );
    left &= ::std::move( right );
    left |= ::std::move( right );
    left ^= ::std::move( right );
    left >>= ::std::move( right );
    left <<= ::std::move( right );

    // rvalue / lvalue
    ::std::move( left ) + right;
    ::std::move( left ) - right;
    ::std::move( left ) * right;
    ::std::move( left ) / right;
    ::std::move( left ) % right;

    ::std::move( left ) == right;
    ::std::move( left ) != right;
    ::std::move( left ) > right;
    ::std::move( left ) < right;
    ::std::move( left ) >= right;
    ::std::move( left ) <= right;
    ::std::move( left ) && right;
    ::std::move( left ) || right;
    ::std::move( left ) & right;
    ::std::move( left ) | right;
    ::std::move( left ) ^ right;
    ::std::move( left ) >> right;
    ::std::move( left ) << right;

    ::std::move( left ) += right;
    ::std::move( left ) -= right;
    ::std::move( left ) *= right;
    ::std::move( left ) /= right;
    ::std::move( left ) %= right;
    ::std::move( left ) &= right;
    ::std::move( left ) |= right;
    ::std::move( left ) ^= right;
    ::std::move( left ) >>= right;
    ::std::move( left ) <<= right;

    // rvalue / rvalue
    ::std::move( left ) + ::std::move( right );
    ::std::move( left ) - ::std::move( right );
    ::std::move( left ) * ::std::move( right );
    ::std::move( left ) / ::std::move( right );
    ::std::move( left ) % ::std::move( right );

    ::std::move( left ) == ::std::move( right );
    ::std::move( left ) != ::std::move( right );
    ::std::move( left ) > ::std::move( right );
    ::std::move( left ) < ::std::move( right );
    ::std::move( left ) >= ::std::move( right );
    ::std::move( left ) <= ::std::move( right );
    ::std::move( left ) && ::std::move( right );
    ::std::move( left ) || ::std::move( right );
    ::std::move( left ) & ::std::move( right );
    ::std::move( left ) | ::std::move( right );
    ::std::move( left ) ^ ::std::move( right );
    ::std::move( left ) >> ::std::move( right );
    ::std::move( left ) << ::std::move( right );

    ::std::move( left ) += ::std::move( right );
    ::std::move( left ) -= ::std::move( right );
    ::std::move( left ) *= ::std::move( right );
    ::std::move( left ) /= ::std::move( right );
    ::std::move( left ) %= ::std::move( right );
    ::std::move( left ) &= ::std::move( right );
    ::std::move( left ) |= ::std::move( right );
    ::std::move( left ) ^= ::std::move( right );
    ::std::move( left ) >>= ::std::move( right );
    ::std::move( left ) <<= ::std::move( right );
}

template < typename _Left, typename _Right >
void testWrapperBinaryOperators ()
{
    // NOTE: Можно проверить любой.
    // Все вместе могут выдать testOperators.o: 'File too big'
    // Для проверки достаточно любого

    testWrapperBinaryOperatorsSpec< _Left, _Right >();

    //testWrapperBinaryOperatorsSpec< _Left, const _Right >();
    //testWrapperBinaryOperatorsSpec< _Left, volatile _Right >();
    //testWrapperBinaryOperatorsSpec< _Left, const volatile _Right >();
    //testWrapperBinaryOperatorsSpec< const _Left, _Right >();
    //testWrapperBinaryOperatorsSpec< const _Left, const _Right >();
    //testWrapperBinaryOperatorsSpec< const _Left, volatile _Right >();
    //testWrapperBinaryOperatorsSpec< const _Left, const volatile _Right >();
    //testWrapperBinaryOperatorsSpec< volatile _Left, _Right >();
    //testWrapperBinaryOperatorsSpec< volatile _Left, const _Right >();
    //testWrapperBinaryOperatorsSpec< volatile _Left, volatile _Right >();
    //testWrapperBinaryOperatorsSpec< volatile _Left, const volatile _Right >();
    //testWrapperBinaryOperatorsSpec< const volatile _Left, _Right >();
    //testWrapperBinaryOperatorsSpec< const volatile _Left, const _Right >();
    //testWrapperBinaryOperatorsSpec< const volatile _Left, volatile _Right >();
    //testWrapperBinaryOperatorsSpec< const volatile _Left, const volatile _Right >();

    //testWrapperBinaryOperatorsSpec< _Right, _Left >();
    //testWrapperBinaryOperatorsSpec< _Right, const _Left >();
    //testWrapperBinaryOperatorsSpec< _Right, volatile _Left >();
    //testWrapperBinaryOperatorsSpec< _Right, const volatile _Left >();
    //testWrapperBinaryOperatorsSpec< const _Right, _Left >();
    //testWrapperBinaryOperatorsSpec< const _Right, const _Left >();
    //testWrapperBinaryOperatorsSpec< const _Right, volatile _Left >();
    //testWrapperBinaryOperatorsSpec< const _Right, const volatile _Left >();
    //testWrapperBinaryOperatorsSpec< volatile _Right, _Left >();
    //testWrapperBinaryOperatorsSpec< volatile _Right, const _Left >();
    //testWrapperBinaryOperatorsSpec< volatile _Right, volatile _Left >();
    //testWrapperBinaryOperatorsSpec< volatile _Right, const volatile _Left >();
    //testWrapperBinaryOperatorsSpec< const volatile _Right, _Left >();
    //testWrapperBinaryOperatorsSpec< const volatile _Right, const _Left >();
    //testWrapperBinaryOperatorsSpec< const volatile _Right, volatile _Left >();
    //testWrapperBinaryOperatorsSpec< const volatile _Right, const volatile _Left >();
}

template < typename _Type, typename _Other >
void testAll ()
{
    testConstructors< _Type >();
    //testAssignmentOperator< _Type, _Type >();
    testAssignmentOperator< _Type, _Other >();
    testWrapperUnaryOperators< _Type >();
    //testWrapperBinaryOperators< _Type, _Type >();
    testWrapperBinaryOperators< _Type, _Other >();
    //testWrapperBinaryOperators< _Other, _Type >();
}

void testFeature ()
{
    using WrapperInt = Wrapper< Data< int > >;
    using WrapperDaouble = Wrapper< Data< double >, Inplace::Uninitialized >;

    testAll< WrapperInt, WrapperDaouble >();
}

void testRangeOperators ()
{
    using Vector = Wrapper< ::std::vector< int >, ::ScL::Feature::Inplace::Debug >;

    Vector values{ { 0, 1, 2, 3, 4 } };

    size_t count = values->size();
    for ( size_t i = 0; i < count; ++i )
        ::std::cout << *&(values[ i ]) << ::std::endl;

    for ( auto iter = ::std::begin( values ); iter != ::std::end( values ); ++iter )
        ::std::cout << *&(*iter) << ::std::endl;

    for ( auto iter = ::std::begin( values ), end_iter = ::std::end( values ); iter != end_iter; ++iter )
        ::std::cout << *&(*iter) << ::std::endl;

    for ( const auto & value : values )
        ::std::cout << *&value << ::std::endl;

    //for ( int & value : container )
    //    ::std::cout << *&value << ::std::endl;
    //for ( int value : container )
    //    value *= 2;
}

//#include <vector>
//#include "MyType.h"

//using TenMyTypes = MyType[10];

//struct OtherType
//{
//    Wrapper< TenMyTypes > m_my_datas;
//};

//using OtherTypes = ::std::vector< OtherType >;

//#include <list>

//void testMemberOperators ()
//{
//    using MyType = int;
//    using OtherType = int;
//    using OtherTypes = ::std::list< int >;

//    Wrapper< MyType > my_data;
//    Wrapper< OtherType > other_data;
//    Wrapper< OtherTypes > other_datas;

//    other_datas->push_back( *&other_data );
//    other_datas->push_back( *&other_data );
//    other_datas->push_back( *&other_data );

//    my_data->m_age = 10;

//    Wrapper< OtherTypes >()[0];

//    for ( size_t i = 0; i < other_datas->size(); ++i )
//    {
//        other_datas[ i ] = other_data;
//        for ( size_t j = 0; j < 10; ++j )
//        {
//            other_datas[ i ][ j ]->m_age = 10;
//        }
//    }
//}
