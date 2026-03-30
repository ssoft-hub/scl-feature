#include <gtest_utils.h>

#include <scl/feature/inplace/uninitialized.h>
#include <scl/feature/wrapper.h>

#include <string>

using namespace ::scl;

TEST(UninitializedWrapper, Create)
{
    [[maybe_unused]]
    constexpr wrapper<int, feature::inplace::uninitialized> w_int{};
    [[maybe_unused]]
    constexpr wrapper<double, feature::inplace::uninitialized> w_double{};
    [[maybe_unused]]
    constexpr wrapper<::std::string, feature::inplace::uninitialized> w_string{};
}
