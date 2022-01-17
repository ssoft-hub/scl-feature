//#ifdef _MSC_VER
//#pragma warning( disable : 4552 4553 )
//#endif

#include <ScL/Feature.h>
#include <ScL/Utility.h>
#include <type_traits>
#include <vector>
#include <iostream>
#include <memory>

#define mayby_unused void

#if defined( __GNUC__ )
#   include <cxxabi.h>
#endif

using namespace ::ScL;
using namespace ::ScL::Feature;

template < typename _Type >
void printTypeOf ()
{
#if defined( __GNUC__ )
    int status = 0;
    char * realname = abi::__cxa_demangle( typeid( _Type ).name(), nullptr, nullptr, &status );
#elif defined( _MSC_VER )
    const char * realname = typeid( _Type ).name();
#endif

    ::std::cout
        << realname;

    if ( ::std::is_const< _Type >::value )
        ::std::cout
            << " const";

    if ( ::std::is_lvalue_reference< _Type >::value )
        ::std::cout
            << " &";

    if ( ::std::is_rvalue_reference< _Type >::value )
        ::std::cout
            << " &&";

    ::std::cout
        << ::std::endl << ::std::flush;

#if defined( __GNUC__ )
    if ( status )
        free( realname );
#endif
}

template < typename _Type >
void printTypeOf ( const _Type & )
{
    printTypeOf< _Type >();
}

template < typename _Type, typename _Other >
constexpr bool isSimilar ( _Type &&, _Other && )
{
    static_assert( ::ScL::Feature::is_similar< _Type, _Other >, "Error" );
    return ::ScL::Feature::is_similar< _Type, _Other >;
}

struct My
{
    void rvalueMethod () && {}
    void rvalueConstMethod () const && {}
    void lvalueMethod () & {}
    void lvalueConstMethod () const & {}
};

void compileTestWrapperTrancpatancy ()
{
    using namespace ::ScL;
    using namespace ::ScL::Feature;

    Wrapper< My > instance;
    My value;
    My().rvalueMethod();
    My().rvalueConstMethod();
    asConst( My() ).rvalueConstMethod();

    ::std::make_shared< My >()->lvalueMethod();

    // не работает (taking address of temporary)
    // так как значение не обязательно должно быть размещено в памяти
    // оно может быть размещено в регистрах ЦП, и как следствие,
    // не иметь адреса.
    //(*&My()).rvalueMethod();
    //(*&My()).rvalueConstMethod();
    //(*&castConst( My() ) ).rvalueConstMethod();

    // по похожим причинам нельзя через operator -> вызвать rvalue методы
    //::ScL::Detail::ReferPointer< My && >( My() )->rvalueMethod();
    //::ScL::Detail::ReferPointer< My && >( My() )->rvalueConstMethod();
    //castConst( ::ScL::Detail::ReferPointer< My && >( My() ) )->rvalueConstMethod();

    // как следствие, не работает
    //Wrapper< My >()->rvalueMethod();
    //Wrapper< My >()->rvalueConstMethod();
    //castConst( Wrapper< My >() )->rvalueConstMethod();

    //My().lvalueMethod(); // почему-то T&& не преобразуется к T&
    My().lvalueConstMethod();
    asConst( My() ).lvalueConstMethod();

    Wrapper< My >()->lvalueMethod();
    Wrapper< My >()->lvalueConstMethod();
    asConst( Wrapper< My >() )->lvalueConstMethod();

    instance->lvalueMethod();
    instance->lvalueConstMethod();
    value.lvalueMethod();
    value.lvalueConstMethod();

    isSimilar( Wrapper< My >(), My() );
    //isSimilar( Wrapper< My >(), *&Wrapper< My >() );
    isSimilar( instance, value );
    //isSimilar( instance, *&instance );
}

extern void testResultOf();
extern void testWrapperValue ();
extern void testWrapperContainer ();

extern void testConstructors ();
extern void testSameToolConstructors ();
extern void testDiffToolConstructors ();
extern void testBinaryOperators ();

extern void testFeature ();
extern void testRangeOperators ();

int main ( int /*argc*/, char ** /*argv*/ )
{
    testRangeOperators();

    testConstructors ();
    testSameToolConstructors ();
    testDiffToolConstructors ();

    compileTestWrapperTrancpatancy();
    //testResultOf();
    testWrapperContainer();
    //testBinaryOperators();
    testFeature();
//    testMemberOperators();
//    testConstructor();
//    testAllTool();
//    testRelation();


////    int int_value; // OK
////    const int int_value = 0; // ERROR

//    Wrapper< int > int_value; // OK
////    const Wrapper< int > int_value = 0; // ERROR
////    Wrapper< const int > int_value = 0; // ERROR

////    Wrapper< Wrapper< int, ImplicitTool > > int_value; // OK
////    const Wrapper< Wrapper< int, ImplicitTool > > int_value; // ERROR
////    Wrapper< const Wrapper< int, ImplicitTool > > int_value; // ERROR
////    Wrapper< Wrapper< const int, ImplicitTool > > int_value; // ERROR

////    Wrapper< UniqueEnd< int, ToolType > > int_value( UniqueEnd< int, ToolType >::make( 10 ) ); // OK
////    const Wrapper< UniqueEnd< int, ToolType > > int_value; // OK
////    Wrapper< const UniqueEnd< int, ToolType > > int_value; // OK
////    Wrapper< UniqueEnd< const int, ToolType > > int_value; // ERROR

////    Wrapper< SharedEnd< int, ToolType >  > int_value; // OK
////    const Wrapper< SharedEnd< int, ToolType >  > int_value; // OK
////    Wrapper< const SharedEnd< int, ToolType >  > int_value; // OK
////    Wrapper< SharedEnd< const int, ToolType > > int_value; // ERROR

//    (*&int_value) += 12345;
//    *&castConst(int_value);

//    Wrapper< ::std::string > name;
//    name = "Hello";

//    Wrapper< Wrapper< Item, Tool >, ImplicitTool > item;
//    item->m_int = 67890;
//    item->m_string = "Item";
//    item->m_unique_string = "Changed unique string";
//    item->m_shared_string = "Change shared string";
//    item->m_refer_string = "Change refer string";

//    ::std::cout
//        << castConst(int_value) << ::std::endl
//        << castConst(name) << ::std::endl
//        << castConst(item)->m_int << ::std::endl
//        << castConst(item)->m_string << ::std::endl
//        << castConst(item)->m_unique_string << ::std::endl
//        << castConst(item)->m_shared_string << ::std::endl
//        << castConst(item)->m_refer_string << ::std::endl
//    ;

//    Wrapper< Wrapper< Item, Tool >, ImplicitTool > other_item;// = item;
////    (*&other_item ).m_refer = (*&castConst(item ).m_string;
//    other_item = item;

//    other_item->m_int = 1;
//    other_item->m_string = "Word";

//    ::std::cout
//        << castConst(other_item)->m_int << ::std::endl
//        << castConst(other_item)->m_string << ::std::endl
////        << castConst(other_item)->m_string_refer << ::std::endl
//    ;

    return 0;
}


//// Альтернативный синтаксис
//void syntaxExample ()
//{
//    struct ExampleType
//    {
//        int m_member;
//    };

//    Wrapper< ExampleType > value;

//    // Синтаксис подобный значению:
//    // распределяет значение в соответствии с типом value.
//    value = ExampleType();

//    // Доступ к неконстантному внутреннему значению
//    (*&value);  // совместим с типом отличным от Wrapper

//    // Доступ к константному внутреннему значению
//    (*&castConst(value)); // совместим с типом отличным от Wrapper

//    // Доступ к неконстантному члену класса
//    value->m_member;    // не совместим с типом отличным от Wrapper
//    (&value)->m_member; // совместим с типом отличным от Wrapper
//    (*&value).m_member; // совместим с типом отличным от Wrapper

//    // Доступ к константному члену класса
//    castConst(value)->m_member;      // не совместим с типом отличным от Wrapper
//    (&castConst(value))->m_member;   // совместим с типом отличным от Wrapper
//    (*&castConst(value)).m_member;   // совместим с типом отличным от Wrapper
//}

//void operatorExample ()
//{
//    Wrapper< int > test_value;
//    Wrapper< int > left_value;
//    Wrapper< int > right_value;
//    int cpp_value = 1;

//    left_value + right_value;
//    cpp_value + right_value;
//    left_value + cpp_value;

//    left_value - right_value;
//    cpp_value - right_value;
//    left_value - cpp_value;

//    +test_value;
//    -test_value;

//    left_value * right_value;
//    cpp_value * right_value;
//    left_value * cpp_value;

//    left_value / right_value;
//    cpp_value / right_value;
//    left_value / cpp_value;

//    left_value % right_value;
//    cpp_value % right_value;
//    left_value % cpp_value;

//    ++test_value;
//    --test_value;
//    test_value++;
//    test_value--;

//    left_value == right_value;
//    cpp_value == right_value;
//    left_value == cpp_value;

//    left_value != right_value;
//    cpp_value != right_value;
//    left_value != cpp_value;

//    left_value > right_value;
//    cpp_value > right_value;
//    left_value > cpp_value;

//    left_value < right_value;
//    cpp_value < right_value;
//    left_value < cpp_value;

//    left_value >= right_value;
//    cpp_value >= right_value;
//    left_value >= cpp_value;

//    left_value <= right_value;
//    cpp_value <= right_value;
//    left_value <= cpp_value;

//    !test_value;

//    left_value && right_value;
//    cpp_value && right_value;
//    left_value && cpp_value;

//    left_value || right_value;
//    cpp_value || right_value;
//    left_value || cpp_value;

//    ~test_value;

//    left_value & right_value;
//    cpp_value & right_value;
//    left_value & cpp_value;

//    left_value | right_value;
//    cpp_value | right_value;
//    left_value | cpp_value;

//    left_value ^ right_value;
//    cpp_value ^ right_value;
//    left_value ^ cpp_value;

//    left_value << right_value;
//    cpp_value << right_value;
//    left_value << cpp_value;

//    left_value >> right_value;
//    cpp_value >> right_value;
//    left_value >> cpp_value;

//    left_value += right_value;
//    cpp_value += right_value;
//    left_value += cpp_value;

//    left_value -= right_value;
//    cpp_value -= right_value;
//    left_value -= cpp_value;

//    left_value *= right_value;
//    cpp_value *= right_value;
//    left_value *= cpp_value;

//    left_value /= right_value;
//    cpp_value /= right_value;
//    left_value /= cpp_value;

//    left_value %= right_value;
//    cpp_value %= right_value;
//    left_value %= cpp_value;

//    left_value &= right_value;
//    cpp_value &= right_value;
//    left_value &= cpp_value;

//    left_value |= right_value;
//    cpp_value |= right_value;
//    left_value |= cpp_value;

//    left_value ^= right_value;
//    cpp_value ^= right_value;
//    left_value ^= cpp_value;

//    left_value <<= right_value;
//    cpp_value <<= right_value;
//    left_value <<= cpp_value;

//    left_value >>= right_value;
//    cpp_value >>= right_value;
//    left_value >>= cpp_value;
//}

struct Foo {
    void Bar() { // do something
    }
};

//template < typename TOwner, void(TOwner::*func)()>
template < typename _Refer, typename _Function >
void Call( _Refer p, _Function func ) {
    (p.*func)();
}
int testFoo() {
    Foo a;
    Call<Foo &>(a, &Foo::Bar);
    return 0;
}

//using Res = ::std::result_of_t< double & (std::vector<double>&, int&)>;

template < typename _Refer, typename ... _Arguments >
decltype(auto) squareBrackets ( _Refer refer, _Arguments && ... arguments )
{
    return refer.operator[] ( ::std::forward< _Arguments >( arguments ) ... );
}

//template < typename _Refer, typename ... _Arguments >
//int squareBrackets1 ( _Refer refer, _Arguments && ... arguments )
//{
//    return 1;
//}

//using Container = ::std::vector< double >;

//template < typename _Refer, typename _Function, typename ... _Arguments >
//decltype(auto) execute ( _Refer refer, _Function function, _Arguments && ... arguments )
//{
//    printTypeOf< _Refer >();
//    printTypeOf< Container & >();
//    printTypeOf< _Function >();
//    printTypeOf( function );
//    using R = ::std::result_of_t< decltype( function )( _Refer, _Arguments && ... ) >;
//    using F = ::std::remove_reference_t< R >;
//    printTypeOf< R >();
//    printTypeOf< F >();

//    //using F = ::std::remove_reference_t< R >;
//    auto f = function( refer, ::std::forward< _Arguments >( arguments ) ... );
//    return f;
//}

//void testResultOf ()
//{
//    using R = ::std::result_of_t< decltype( squareBrackets< Container &, size_t > )&( Container &, size_t ) >;
//    using F = ::std::remove_reference_t< R >;
//    auto func = &squareBrackets< Container &, size_t >;

//    using R1 = ::std::result_of_t< decltype( func )( Container &, size_t ) >;
//    using F1 = ::std::remove_reference_t< R1 >;

//    printTypeOf< const double & >();
//    printTypeOf< R >();
//    printTypeOf< F >();
//    printTypeOf< R1 >();
//    printTypeOf< F1 >();
//    printTypeOf< decltype( func ) >();

//    Container container = { 1, 2, 3 };
//    F f = squareBrackets< Container &, size_t >( container, size_t() );
//    F1 f1 = execute< Container & >( container, func, size_t() );


//    //using R1 = ::std::result_of_t< decltype( squareBrackets1< Container &, int > )&( Container &, int ) >;
//}

void testWrapperValue ()
{
    { Wrapper< double > value; (mayby_unused) value; }
    { Wrapper< double > value{ double() }; (mayby_unused) value; }
    { Wrapper< double > value = double(); (mayby_unused) value; }
    { Wrapper< double > value{ Wrapper< double >() }; (mayby_unused) value; }
    { Wrapper< double > value = Wrapper< double >(); (mayby_unused) value; }
    { Wrapper< double > value{ Wrapper< int >() }; (mayby_unused) value; }
    { Wrapper< double > value = Wrapper< int >(); (mayby_unused) value; }
}

void testWrapperContainer ()
{
    using Container = ::std::vector< double >;
    //using Container = Wrapper< ::std::vector< double >, Implicit::SharedTool >;

    { Wrapper< Container > value; (mayby_unused) value; }
    { Wrapper< Container > value{ Container() }; (mayby_unused) value; }
    { Wrapper< Container > value = Container(); (mayby_unused) value; }
    {
        Wrapper< Container > container;
        for ( int i = 0; i < 10; ++i )
            container->push_back( i );

        auto v0 = container[5];
        auto v1 = asConst( container )[5];
        auto v2 = &(asConst( container )[5]);
        auto v3 = *v2;

        printTypeOf( v0 );
        printTypeOf( v1 );
        printTypeOf( v2 );
        printTypeOf( v3 );

        for ( int i = 0; i < 10; ++i )
            container[ i ] = asConst( container )[ 9 - i ];

        Wrapper< Container >( 10, 123.456 )[0];
        asConst( Wrapper< Container >( 10, 123.456 ) )[0];
    }
}

//#include <iostream>
//#include <string>

//using ImplicitTool = Shared::ImplicitTool;
////using RelationTool = Shared::RelationTool;
//using Tool = ::Cpp::Inplace::DefaultTool;

//struct Item
//{
//    using Int = int;
//    using String = ::std::string;
//    using UniqueString = CompositeEnd< String >;
//    using SharedString = SharedEnd< String >;
//    using ReferString = NoneEnd< String >;

//    Wrapper< Int > m_int;
//    Wrapper< String > m_string;
//    Wrapper< UniqueString > m_unique_string;
//    Wrapper< SharedString > m_shared_string;
//    Wrapper< ReferString > m_refer_string;

//    Item ()
//        : m_int()
//    , m_string()
//    , m_unique_string( UniqueString::make( "Unique string" ) )
//    , m_shared_string( SharedString::make( "Shared string" ) )
//    , m_refer_string( ReferString::refer( m_unique_string ) )
//    {
//    }

//    Item ( const Item & other )
//        : m_int( other.m_int )
//    , m_string( other.m_string )
//    , m_unique_string( UniqueString::copy( other.m_unique_string ) )
//    , m_shared_string( SharedString::share( other.m_shared_string ) )
//    , m_refer_string( ReferString::refer( other.m_refer_string ) )
//    {
//    }

//    Item ( Item && other ) = default;

//    Item & operator = ( const Item & other )
//    {
//        m_int = other.m_int;
//        m_string = other.m_string;
//        m_unique_string = other.m_unique_string;
//        m_shared_string = other.m_shared_string;
//        m_refer_string = other.m_refer_string;
//        return *this;
//    }

////    ReturnUpdate< StringRef > stringRefer ();
////    ReturnRead< StringRef > stringRefer () const;
//};

////using ItemRawRefer = ReferEnd< Item, ::Cpp::Raw::RelationTool >;

////using BaseItemRefer = ReferEnd< BaseItem, RelationTool >;
////using ItemRefer = ReferEnd< Item, RelationTool >;
////using ItemUnique = UniqueEnd< Item, RelationTool >;
////using ItemShared = SharedEnd< Item, RelationTool >;

////using ItemAggregation = AggregationEnd< Item, RelationTool >;
////using ItemAssociation = AssociationEnd< Item, RelationTool >;

////using BaseItemAggregation = AggregationEnd< BaseItem, RelationTool >;
////using BaseItemAssociation = AssociationEnd< BaseItem, RelationTool >;


////void testItem ()
////{
////    ItemShared shared_item = ItemShared::make(); // make new shared end and new value
////    ItemShared shared_other_item = ItemShared::share( shared_item ); // copy shared end | share value
////    ItemUnique unique_item = ItemUnique::make(); // make new unique end and new value

//////    ItemRefer unique_refer_item = ItemRefer::refer( unique_item );
//////    ItemRefer shared_refer_item = ItemRefer::refer( shared_item );
//////    ItemRefer refer_refer_item = ItemRefer::refer( unique_refer_item );
//////    BaseItemRefer base_refer_item = BaseItemRefer::refer( shared_other_item );

////    ::std::cout
////        << "OK" << ::std::endl;
////}

//extern void testAllTool ();
//extern void testRelation ();

//template < typename _Type >
//void foo ( const _Type & ) { ::std::cout << ::std::endl; }

//template < typename _Type >
//void overloaded( _Type const & ) { ::std::cout << "by lvalue" << ::std::endl; }

//template < typename _Type >
//void overloaded( _Type && ) { ::std::cout << "by rvalue" << ::std::endl; }

//struct TestType
//{
//    ::std::string m_string;


//    TestType () : m_string() { ::std::cout << "construct default" << ::std::endl; }
//    TestType ( ::std::string && other ) : m_string( other ) { ::std::cout << "construct from string && "; overloaded( other ); }
//    TestType ( const ::std::string & other ) : m_string( other ) { ::std::cout << "construct from string & "; overloaded( other ); }

//    TestType & operator = ( ::std::string && other ) { ::std::cout << "operate from string && "; overloaded( other ); return *this; }
//    TestType & operator = ( const ::std::string & other ) { ::std::cout << "operate from string & "; overloaded( other ); return *this; }

//    TestType & operator = ( TestType && other ) { ::std::cout << "operate from TestType && "; overloaded( other ); return *this; }
//    TestType & operator = ( const TestType & other ) { ::std::cout << "operate from TestType & "; overloaded( other ); return *this; }

//    TestType ( TestType && other ) : m_string( ::std::forward< ::std::string >( other.m_string ) ) { ::std::cout << "construct from TestType && "; overloaded( other ); }
//    TestType ( const TestType & other ) : m_string( ::std::forward< const ::std::string & >( other.m_string ) ) { ::std::cout << "construct from TestType & "; overloaded( other ); }
//};

//void testConstructor ()
//{
////    using TestWrapper = Wrapper< TestType >;
//    using TestWrapper = Wrapper< Optimal< TestType > >;

//    // Конструктор без инициализации
//    {
//        TestWrapper value( InitializeType::NotInitialized );
//        foo( value );
//    }

//    // Конструктор по умолчанию
//    {
//        TestWrapper value;
//        foo( value );
//    }

//    // Конструктор инициализации по значению и соответсвующий оператор равенства
//    {
//        TestWrapper value( "Hello!" );
//        value = ::std::string( "Hello Memory!" );
//        foo( value );
//    }

//    // Конструктор инициализации по копии значения и соответсвующий оператор равенства
//    {
//        const TestType first_text = ::std::string( "Hello!" );
//        const TestType second_text = ::std::string( "Hello Memory!" );
//        TestWrapper value( first_text );
//        value = second_text;
//        foo( value );
//    }

//    // Конструктор перемещения и соответсвующий оператор равенства
//    {
//        TestWrapper first( "Move test" );
//        TestWrapper second( ::std::move( first ) );
//        first = ::std::move( second );
//        foo( first );
//    }

//    // Конструктор копирования и соответсвующий оператор равенства
//    {
//        TestWrapper first( "Copy test" );
//        TestWrapper second( first );
//        second = first;
//        foo( first );
//    }
//}


//void testRelation ()
//{
//    // Значения определенного типа
////    using UniqueInt = UniqueEnd< int, ::Cpp::Inplace::DefaultTool >;
////    using SharedInt = SharedEnd< int, ::Cpp::Inplace::DefaultTool >;
//    using UniqueInt = Wrapper< int, ::Cpp::Inplace::DefaultTool >;
//    using SharedInt = Wrapper< int, ::Cpp::Inplace::DefaultTool >;

//    Wrapper< UniqueInt > unique_int;
//    Wrapper< SharedInt > shared_int;

//    shared_int = unique_int; // равенство на уровне внутренних значений.
//}

//extern void testMemberOperators ();

