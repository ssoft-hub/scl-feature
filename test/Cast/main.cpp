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

TEST_CASE("Testing ::ScL::Feature::cast function with different types")
{
    using namespace ::ScL::Feature;

    testCast<MyData>();
    testCast<Wrapper<MyData, Inplace::Optional>>();
    testCast<Wrapper<MyData, Inplace::Optional, Inplace::Default>>();
    testCast<Wrapper<MyData, Inplace::Debug, Inplace::Optional, Inplace::Default>>();
}
