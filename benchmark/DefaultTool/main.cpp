#include <chrono>
#include <iostream>
#include <vector>
#include <ScL/Feature.h>

struct Timer
{
    using Clock = ::std::chrono::steady_clock;
    using TimePoint =  ::std::chrono::time_point<Clock>;
    using TimeDuration = TimePoint::duration;

    TimePoint m_start{};

    Timer ()
        : m_start{ Clock::now() }
    {}

    ~Timer ()
    {
        auto stop{ Clock::now() };
        TimeDuration duration = stop - m_start;
        ::std::cout << "Time duration: " << duration.count() << ::std::endl;
    }
};

template <typename T_>
void foo ()
{
    using Container = T_;
    using Count = ::std::size_t;
    using Index = Count;

    Count constexpr value_count = 100000000;

    Container container;
    container.reserve(value_count);

    [[maybe_unused]]
    Timer timer;

    for (Index i = 0; i < value_count; ++i)
        container.emplace_back(int(i));

    for (Index i = 0; i < value_count; ++i)
        container[i] += container[value_count - i - 1];
}

int main (int, char**)
{
    using namespace ::std;
    using namespace ::ScL::Feature;

    for ( int i = 0; i < 10; ++i )
    {
        foo<::std::vector<int>>();
        foo<::std::vector<Wrapper<int>>>();
        foo<Wrapper<::std::vector<int>>>();
        foo<Wrapper<::std::vector<Wrapper<int>>>>();
    }
}
