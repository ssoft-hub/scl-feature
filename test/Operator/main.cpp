#include <scl/feature.h>

class Test
{
    int m_int;

public:
    int const & operator[](int) const { return m_int; }
};

using namespace ::ScL::Feature;
static_assert(::scl::is_detected_v<::scl::subscript_operation, Test, int>, "");
static_assert(::scl::is_detected_v<::scl::subscript_member_exact_operation, Test const, int>, "");

#include <iomanip>
#include <ostream>
#include <sstream>
#include <vector>

::std::ostream & my(::std::ostream & stream) { return stream; }

void wrapper()
{
    using Test = ::std::vector<int>;
    Wrapper<Test> test{
        {0, 1, 2, 3, 4, 5}
    };
    Wrapper<int> i{1};
    auto n = test[1];
    ::std::cout << ::std::setbase(16);

    static_assert(
        ::scl::is_detected_v<::scl::left_shift_member_exact_operation, decltype(::std::cout), int>,
        "");

    ::std::cout << i;
    ::std::cout << n << n << test[1] << ::std::flush << ::std::endl;

    // using SStream = Wrapper< std::ostringstream >;
    using SStream = std::ostringstream;

    SStream sstream;
    sstream << n << n << test[1] << ::std::flush << ::std::endl;

    using Stream = decltype(::std::cout);
    using Endl = decltype(::std::endl(::std::declval<Stream &>())) (*)(Stream &);

    // sstream.operator << < Endl >( ::std::endl );
    sstream << Endl(::std::endl);
    sstream << ::std::endl;

    static_assert(::scl::is_detected_v<::scl::left_shift_operation, SStream, Endl>, "");
}

void test()
{
    Test test{};
    auto n = test[1];
    ::std::cout << n << ::std::endl;
}

int main(int, char **)
{
    wrapper();
    return 0;
}
