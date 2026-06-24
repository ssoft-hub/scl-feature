#include <scl/feature/inplace/optional.h>
#include <scl/feature/wrapper.h>

#include <iostream>
#include <string>

int main()
{
    using namespace ::scl;

    // --- Empty state ---------------------------------------------------------
    wrapper<int, feature::inplace::optional> empty{};
    std::cout << "empty.has_value() = " << empty.has_value() << '\n'; // 0

    // --- Value construction --------------------------------------------------
    wrapper<int, feature::inplace::optional> w{42};
    std::cout << "w.has_value()  = " << w.has_value() << '\n'; // 1
    std::cout << "w.value_or(0)  = " << w.value_or(0) << '\n'; // 42

    // --- emplace / reset -----------------------------------------------------
    empty.emplace(7);
    std::cout << "after emplace: empty.value_or(-1) = " << empty.value_or(-1) << '\n'; // 7
    empty.reset();
    std::cout << "after reset:   empty.has_value()  = " << empty.has_value() << '\n';  // 0
    std::cout << "after reset:   empty.value_or(-1) = " << empty.value_or(-1) << '\n'; // -1

    // --- Copy independence ---------------------------------------------------
    wrapper<int, feature::inplace::optional> a{10};
    wrapper<int, feature::inplace::optional> b{a};
    a.emplace(20);
    auto & exec_b = feature::detail::executor_access::get(b);
    std::cout << "b after mutating a: " << feature::inplace::optional<int>::value(exec_b) << '\n'; // 10

    // --- String round-trip ---------------------------------------------------
    wrapper<::std::string, feature::inplace::optional> sw{};
    sw.emplace("hello, optional");
    auto & sexec = feature::detail::executor_access::get(sw);
    std::cout << "string value: " << feature::inplace::optional<::std::string>::value(sexec) << '\n';
    sw.reset();
    std::cout << "string after reset: has_value = " << sw.has_value() << '\n'; // 0

    return 0;
}
