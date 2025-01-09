#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <ScL/Feature.h>

struct MyData {};

enum class Result
{
    LValue,
    LValueConst,
    LValueVolatile,
    LValueConstVolatile,
    RValue,
    RValueConst,
    RValueVolatile,
    RValueConstVolatile
};

auto foo ( MyData & ) { return Result::LValue; }
auto foo ( MyData const & ) { return Result::LValueConst; }
auto foo ( MyData volatile & ) { return Result::LValueVolatile; }
auto foo ( MyData const volatile & ) { return Result::LValueConstVolatile; }
auto foo ( MyData && ) { return Result::RValue; }
auto foo ( MyData const && ) { return Result::RValueConst; }
auto foo ( MyData volatile && ) { return Result::RValueVolatile; }
auto foo ( MyData const volatile && ) { return Result::RValueConstVolatile; }

template <typename Type_>
void testCast ()
{
    using ::ScL::Feature::cast;

    Type_ data = MyData{};
    Type_ const c_data = MyData{};
    Type_ volatile v_data = MyData{};
    Type_ const volatile cv_data = MyData{};

    CHECK( foo( cast( data ) )                  == Result::LValue );
    CHECK( foo( cast( ::std::move( data ) ) )   == Result::RValue );
    CHECK( foo( cast( c_data ) )                == Result::LValueConst );

#ifdef __clang__
    // CLANG makes error without static_cast
    CHECK( foo( static_cast< MyData const && >( cast( ::std::move( c_data ) ) ) )           == Result::RValueConst );
    CHECK( foo( static_cast< MyData volatile & >( cast( v_data ) ) )                        == Result::LValueVolatile );
    CHECK( foo( static_cast< MyData volatile && >( cast( ::std::move( v_data ) ) ) )        == Result::RValueVolatile );
    CHECK( foo( static_cast< MyData const volatile & >( cast( cv_data ) ) )                 == Result::LValueConstVolatile );
    CHECK( foo( static_cast< MyData const volatile && >( cast( ::std::move( cv_data ) ) ) ) == Result::RValueConstVolatile );
#else
    // MSVC and GCC do not require static_cast
    CHECK( foo( cast( ::std::move( c_data ) ) )     == Result::RValueConst );
    CHECK( foo( cast( v_data ) )                    == Result::LValueVolatile );
    CHECK( foo( cast( ::std::move( v_data ) ) )     == Result::RValueVolatile );
    CHECK( foo( cast( cv_data ) )                   == Result::LValueConstVolatile );
    CHECK( foo( cast( ::std::move( cv_data ) ) )    == Result::RValueConstVolatile );
#endif
}

TEST_CASE("Testing ::ScL::Feature::cast function with different base value types")
{
    using namespace ::ScL::Feature;

    testCast<MyData>();
    testCast<Wrapper<MyData, Inplace::Optional>>();
    testCast<Wrapper<MyData, Inplace::Optional, Inplace::Default>>();
    testCast<Wrapper<MyData, Inplace::Optional, Inplace::Debug, Inplace::Default>>();
}

using WrappedData = ::ScL::Feature::Wrapper<MyData, ::ScL::Feature::Inplace::Optional>;

auto bar ( WrappedData & ) { return Result::LValue; }
auto bar ( WrappedData const & ) { return Result::LValueConst; }
auto bar ( WrappedData volatile & ) { return Result::LValueVolatile; }
auto bar ( WrappedData const volatile & ) { return Result::LValueConstVolatile; }
auto bar ( WrappedData && ) { return Result::RValue; }
auto bar ( WrappedData const && ) { return Result::RValueConst; }
auto bar ( WrappedData volatile && ) { return Result::RValueVolatile; }
auto bar ( WrappedData const volatile && ) { return Result::RValueConstVolatile; }

template <typename Type_>
void testCastForBar ()
{
    using ::ScL::Feature::cast;

    Type_ data = MyData{};
    Type_ const c_data = MyData{};
    Type_ volatile v_data = MyData{};
    Type_ const volatile cv_data = MyData{};

    CHECK( bar( cast( data ) )                  == Result::LValue );
    CHECK( bar( cast( ::std::move( data ) ) )   == Result::RValue );
    CHECK( bar( cast( c_data ) )                == Result::LValueConst );

#ifdef __clang__
    // CLANG makes error without static_cast
    CHECK( bar( static_cast< WrappedData const && >( cast( ::std::move( c_data ) ) ) )           == Result::RValueConst );
    CHECK( bar( static_cast< WrappedData volatile & >( cast( v_data ) ) )                        == Result::LValueVolatile );
    CHECK( bar( static_cast< WrappedData volatile && >( cast( ::std::move( v_data ) ) ) )        == Result::RValueVolatile );
    CHECK( bar( static_cast< WrappedData const volatile & >( cast( cv_data ) ) )                 == Result::LValueConstVolatile );
    CHECK( bar( static_cast< WrappedData const volatile && >( cast( ::std::move( cv_data ) ) ) ) == Result::RValueConstVolatile );
#else
    // MSVC and GCC do not require static_cast
    CHECK( bar( cast( ::std::move( c_data ) ) )     == Result::RValueConst );
    CHECK( bar( cast( v_data ) )                    == Result::LValueVolatile );
    CHECK( bar( cast( ::std::move( v_data ) ) )     == Result::RValueVolatile );
    CHECK( bar( cast( cv_data ) )                   == Result::LValueConstVolatile );
    CHECK( bar( cast( ::std::move( cv_data ) ) )    == Result::RValueConstVolatile );
#endif
}

TEST_CASE("Testing ::ScL::Feature::cast function with different wrapped types")
{
    using namespace ::ScL::Feature;

    testCastForBar<Wrapper<MyData, Inplace::Optional>>();
    testCastForBar<Wrapper<MyData, Inplace::Optional, Inplace::Default>>();
    testCastForBar<Wrapper<MyData, Inplace::Optional, Inplace::Debug, Inplace::Default>>();
}
