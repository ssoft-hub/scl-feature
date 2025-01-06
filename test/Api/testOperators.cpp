#include "ScL/Feature.h"
#include "ScL/Utility.h"
#include <iostream>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>

#define maybe_unused void

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

// Visual Studio limitation
#define DATA_GLOBAL_BINARY_OPERATOR_PROTOTYPE( symbol, right_refer )
/*
 #define DATA_GLOBAL_BINARY_OPERATOR_PROTOTYPE( symbol, right_refer ) \
    template < typename _Left, typename _Right > \
    void operator symbol ( _Left &&, Data< _Right> right_refer ) \
        { ::std::cout << DATA_FUNC_INFO << ::std::endl; } \
*/

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
    static_assert( !::std::is_reference< _Type >{}, "_Type must to be a not reference." );
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

    (maybe_unused) one;
    (maybe_unused) two;
    (maybe_unused) three;
    (maybe_unused) four;
    (maybe_unused) five;
    (maybe_unused) six;
    (maybe_unused) seven;
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

    (maybe_unused) one;
    (maybe_unused) four;
    (maybe_unused) five;
    (maybe_unused) six;
    (maybe_unused) seven;
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

    (maybe_unused) one;
    (maybe_unused) four;
    (maybe_unused) five;
    (maybe_unused) six;
    (maybe_unused) seven;
}

void testAccess ()
{
    using Key = Wrapper< ::std::string, Implicit::Raw >;
    using Value = Wrapper< ::std::string, Heap::Raw >;
    //using Value = Wrapper< Data< int > >;
    //using Map = Wrapper< ::std::map< Key, Value > >;
    using Map = Wrapper< ::std::unordered_map< Key, Value > >;

    Map map;
    Key key = "Hello";
    map[ key ] = "World!";

    std::cout << "Iterate and print keys and values of unordered_map, using auto:\n";
    for( const auto & n : map )
        std::cout << "Key:[" << n->first << "] Value:[" << n->second << "]\n";
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

    testWrapperBinaryOperatorsSpec< _Right, _Left >();
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
void testAllOperators ()
{
    testAssignmentOperator< _Type, _Other >();
    testWrapperUnaryOperators< _Type >();
    testWrapperBinaryOperators< _Type, _Other >();
}

void testFeature ()
{
    using WrapperInt = Wrapper< Data< int > >;
    using WrapperDouble = Wrapper< Data< double >, Inplace::Uninitialized >;

    testAllOperators< WrapperInt, WrapperDouble >();
}

void testRangeOperators ()
{
    using Vector = Wrapper< ::std::vector< int >, ::ScL::Feature::Inplace::Debug >;

    Vector const values = { 0, 1, 2, 3, 4 };

    // Testing value access using operator []
    size_t count = values.size();
    for ( size_t i = 0; i < count; ++i )
        ::std::cout << values[ i ] << ::std::endl;

    // Testing global methods begin/end
    for ( auto iter = ::std::begin( values ); iter != ::std::end( values ); ++iter )
        ::std::cout << (*iter) << ::std::endl;

    // Testing reflected methods begin/end
    for ( auto iter = values.begin(); iter != values.end(); ++iter )
        ::std::cout << (*iter) << ::std::endl;

    // Testing ranged 'for'
    for ( const auto & value : values )
        ::std::cout << value << ::std::endl;
}
